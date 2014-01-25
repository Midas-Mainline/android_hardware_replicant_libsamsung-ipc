/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <samsung-ipc.h>
#include <ipc.h>
#include <util.h>

#include "xmm6260.h"
#include "xmm6260_mipi.h"
#include "xmm6260_sec_modem.h"
#include "maguro_ipc.h"

int maguro_ipc_bootstrap(struct ipc_client *client)
{
    void *modem_image_data = NULL;
    int modem_image_fd = -1;
    int modem_boot_fd = -1;
    void *mps_data = NULL;

    unsigned char *p;
    int rc;

    if (client == NULL)
        return -1;

    ipc_client_log(client, "Starting maguro modem bootstrap");

    modem_image_fd = open(MAGURO_MODEM_IMAGE_DEVICE, O_RDONLY);
    if (modem_image_fd < 0) {
        ipc_client_log(client, "Opening modem image device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem image device");

    modem_image_data = mmap(0, MAGURO_MODEM_IMAGE_SIZE, PROT_READ, MAP_SHARED, modem_image_fd, 0);
    if (modem_image_data == NULL || modem_image_data == (void *) 0xffffffff) {
            ipc_client_log(client, "Mapping modem image data to memory failed");
            goto error;
    }
    ipc_client_log(client, "Mapped modem image data to memory");

    modem_boot_fd = open(XMM6260_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (modem_boot_fd < 0) {
        ipc_client_log(client, "Opening modem boot device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem boot device");

    rc = xmm6260_sec_modem_power(modem_boot_fd, 0);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem off");

    rc = xmm6260_sec_modem_power(modem_boot_fd, 1);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem on");

    p = (unsigned char *) modem_image_data + MAGURO_PSI_OFFSET;

    rc = xmm6260_mipi_psi_send(client, modem_boot_fd, (void *) p, MAGURO_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI PSI failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI PSI");

    close(modem_boot_fd);

    modem_boot_fd = open(XMM6260_SEC_MODEM_BOOT1_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (modem_boot_fd < 0) {
        ipc_client_log(client, "Opening modem boot device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem boot device");

    p = (unsigned char *) modem_image_data + MAGURO_EBL_OFFSET;

    rc = xmm6260_mipi_ebl_send(client, modem_boot_fd, (void *) p, MAGURO_EBL_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI EBL failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI EBL");

    rc = xmm6260_mipi_port_config_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI port config failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI port config");

    p = (unsigned char *) modem_image_data + MAGURO_SEC_START_OFFSET;

    rc = xmm6260_mipi_sec_start_send(client, modem_boot_fd, (void *) p, MAGURO_SEC_START_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI SEC start failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI SEC start");

    p = (unsigned char *) modem_image_data + MAGURO_FIRMWARE_OFFSET;

    rc = xmm6260_mipi_firmware_send(client, modem_boot_fd, (void *) p, MAGURO_FIRMWARE_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI firmware failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI firmware");

    rc = xmm6260_mipi_nv_data_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI nv_data");

    mps_data = file_data_read(MAGURO_MPS_DATA_DEVICE, MAGURO_MPS_DATA_SIZE, MAGURO_MPS_DATA_SIZE);
    if (mps_data == NULL) {
        ipc_client_log(client, "Reading MPS data failed");
        goto error;
    }
    ipc_client_log(client, "Read MPS data");

    rc = xmm6260_mipi_mps_data_send(client, modem_boot_fd, mps_data, MAGURO_MPS_DATA_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI MPS data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI MPS data");

    rc = xmm6260_mipi_sec_end_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI SEC end failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI SEC end");

    rc = xmm6260_mipi_hw_reset_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 MIPI HW reset failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 MIPI HW reset");

    rc = xmm6260_sec_modem_status_online_wait(modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for online status failed");
        goto error;
    }
    ipc_client_log(client, "Waited for online status");

    rc = xmm6260_sec_modem_boot_power(modem_boot_fd, 0);
    if (rc < 0) {
        ipc_client_log(client, "Turning modem boot off failed");
        goto error;
    }
    ipc_client_log(client, "Turned modem boot off");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (mps_data != NULL)
        free(mps_data);

    if (modem_image_data != NULL)
        munmap(modem_image_data, MAGURO_MODEM_IMAGE_SIZE);

    if (modem_image_fd >= 0)
        close(modem_image_fd);

    if (modem_boot_fd >= 0)
        close(modem_boot_fd);

    return rc;
}


int maguro_ipc_fmt_send(struct ipc_client *client, struct ipc_message_info *request)
{
    return xmm6260_sec_modem_ipc_fmt_send(client, request);
}

int maguro_ipc_fmt_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    return xmm6260_sec_modem_ipc_fmt_recv(client, response);
}

int maguro_ipc_rfs_send(struct ipc_client *client, struct ipc_message_info *request)
{
    return xmm6260_sec_modem_ipc_rfs_send(client, request);
}

int maguro_ipc_rfs_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    return xmm6260_sec_modem_ipc_rfs_recv(client, response);
}

int maguro_ipc_open(void *data, int type)
{
    struct maguro_ipc_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct maguro_ipc_transport_data *) data;

    transport_data->fd = xmm6260_sec_modem_ipc_open(type);
    if (transport_data->fd < 0)
        return -1;

    return 0;
}

int maguro_ipc_close(void *data)
{
    struct maguro_ipc_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct maguro_ipc_transport_data *) data;

    xmm6260_sec_modem_ipc_close(transport_data->fd);
    transport_data->fd = -1;

    return 0;
}

int maguro_ipc_read(void *data, void *buffer, unsigned int length)
{
    struct maguro_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct maguro_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_read(transport_data->fd, buffer, length);
    return rc;
}

int maguro_ipc_write(void *data, void *buffer, unsigned int length)
{
    struct maguro_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct maguro_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_write(transport_data->fd, buffer, length);
    return rc;
}

int maguro_ipc_poll(void *data, struct timeval *timeout)
{
    struct maguro_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct maguro_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_poll(transport_data->fd, timeout);
    return rc;
}

int maguro_ipc_power_on(void *data)
{
    return 0;
}

int maguro_ipc_power_off(void *data)
{
    int fd;
    int rc;

    fd = open(XMM6260_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
        return -1;

    rc = xmm6260_sec_modem_power(fd, 0);

    close(fd);

    if (rc < 0)
        return -1;

    return 0;
}

int maguro_ipc_data_create(void **transport_data, void **power_data, void **gprs_data)
{
    if (transport_data == NULL)
        return -1;

    *transport_data = (void *) malloc(sizeof(struct maguro_ipc_transport_data));
    memset(*transport_data, 0, sizeof(struct maguro_ipc_transport_data));

    return 0;
}

int maguro_ipc_data_destroy(void *transport_data, void *power_data, void *gprs_data)
{
    if (transport_data == NULL)
        return -1;

    free(transport_data);

    return 0;
}

char *maguro_ipc_gprs_get_iface(int cid)
{
    return xmm6260_sec_modem_ipc_gprs_get_iface(cid);
}


int maguro_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    return xmm6260_sec_modem_ipc_gprs_get_capabilities(capabilities);
}

struct ipc_ops maguro_ipc_fmt_ops = {
    .bootstrap = maguro_ipc_bootstrap,
    .send = maguro_ipc_fmt_send,
    .recv = maguro_ipc_fmt_recv,
};

struct ipc_ops maguro_ipc_rfs_ops = {
    .bootstrap = NULL,
    .send = maguro_ipc_rfs_send,
    .recv = maguro_ipc_rfs_recv,
};

struct ipc_handlers maguro_ipc_handlers = {
    .read = maguro_ipc_read,
    .write = maguro_ipc_write,
    .open = maguro_ipc_open,
    .close = maguro_ipc_close,
    .poll = maguro_ipc_poll,
    .transport_data = NULL,
    .power_on = maguro_ipc_power_on,
    .power_off = maguro_ipc_power_off,
    .power_data = NULL,
    .gprs_activate = NULL,
    .gprs_deactivate = NULL,
    .gprs_data = NULL,
    .data_create = maguro_ipc_data_create,
    .data_destroy = maguro_ipc_data_destroy,
};

struct ipc_gprs_specs maguro_ipc_gprs_specs = {
    .gprs_get_iface = maguro_ipc_gprs_get_iface,
    .gprs_get_capabilities = maguro_ipc_gprs_get_capabilities,
};

struct ipc_nv_data_specs maguro_ipc_nv_data_specs = {
    .nv_data_path = MAGURO_NV_DATA_PATH,
    .nv_data_md5_path = MAGURO_NV_DATA_MD5_PATH,
    .nv_state_path = MAGURO_NV_STATE_PATH,
    .nv_data_bak_path = MAGURO_NV_DATA_BAK_PATH,
    .nv_data_md5_bak_path = MAGURO_NV_DATA_MD5_BAK_PATH,
};

// vim:ts=4:sw=4:expandtab
