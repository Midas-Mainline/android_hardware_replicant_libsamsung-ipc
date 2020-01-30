/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ipc.h>
#include <samsung-ipc.h>

#include "piranha.h"
#include "xmm626.h"
#include "xmm626_mipi.h"
#include "xmm626_sec_modem.h"

int piranha_boot(struct ipc_client *client)
{
    void *modem_image_data = NULL;
    int modem_image_fd = -1;
    int modem_boot_fd = -1;
    unsigned char *p;
    int rc;

    if (client == NULL)
        return -1;

    ipc_client_log(client, "Starting piranha modem boot");

    modem_image_fd = open(PIRANHA_MODEM_IMAGE_DEVICE, O_RDONLY);
    if (modem_image_fd < 0) {
        ipc_client_log(client, "Opening modem image device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem image device");

    modem_image_data = mmap(0, PIRANHA_MODEM_IMAGE_SIZE, PROT_READ, MAP_SHARED,
                            modem_image_fd, 0);
    if (modem_image_data == NULL || modem_image_data == (void *) 0xffffffff) {
            ipc_client_log(client, "Mapping modem image data to memory failed");
            goto error;
    }
    ipc_client_log(client, "Mapped modem image data to memory");

    modem_boot_fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE,
                         O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (modem_boot_fd < 0) {
        ipc_client_log(client, "Opening modem boot device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem boot device");

    rc = xmm626_sec_modem_power(client, modem_boot_fd, 0);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem off");

    rc = xmm626_sec_modem_power(client, modem_boot_fd, 1);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem on");

    p = (unsigned char *) modem_image_data + PIRANHA_PSI_OFFSET;

    rc = xmm626_mipi_psi_send(client, modem_boot_fd, (void *) p,
                              PIRANHA_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI PSI failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI PSI");

    close(modem_boot_fd);

    modem_boot_fd = open(XMM626_SEC_MODEM_BOOT1_DEVICE,
                         O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (modem_boot_fd < 0) {
        ipc_client_log(client, "Opening modem boot device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem boot device");

    p = (unsigned char *) modem_image_data + PIRANHA_EBL_OFFSET;

    rc = xmm626_mipi_ebl_send(client, modem_boot_fd, (void *) p,
                              PIRANHA_EBL_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI EBL failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI EBL");

    rc = xmm626_mipi_port_config_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI port config failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI port config");

    p = (unsigned char *) modem_image_data + PIRANHA_SEC_START_OFFSET;

    rc = xmm626_mipi_sec_start_send(client, modem_boot_fd, (void *) p,
                                    PIRANHA_SEC_START_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI SEC start failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI SEC start");

    p = (unsigned char *) modem_image_data + PIRANHA_FIRMWARE_OFFSET;

    rc = xmm626_mipi_firmware_send(client, modem_boot_fd, (void *) p,
                                   PIRANHA_FIRMWARE_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI firmware failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI firmware");

    rc = xmm626_mipi_nv_data_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI nv_data");

    rc = xmm626_mipi_sec_end_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI SEC end failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI SEC end");

    rc = xmm626_mipi_hw_reset_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 MIPI HW reset failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 MIPI HW reset");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (modem_image_data != NULL)
        munmap(modem_image_data, PIRANHA_MODEM_IMAGE_SIZE);

    if (modem_image_fd >= 0)
        close(modem_image_fd);

    if (modem_boot_fd >= 0)
        close(modem_boot_fd);

    return rc;
}

int piranha_open(__attribute__((unused)) struct ipc_client *client, void *data,
                 int type)
{
    struct piranha_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct piranha_transport_data *) data;

    transport_data->fd = xmm626_sec_modem_open(type);
    if (transport_data->fd < 0)
        return -1;

    return 0;
}

int piranha_close(__attribute__((unused)) struct ipc_client *client, void *data)
{
    struct piranha_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct piranha_transport_data *) data;

    xmm626_sec_modem_close(transport_data->fd);
    transport_data->fd = -1;

    return 0;
}

int piranha_read(__attribute__((unused)) struct ipc_client *client, void *data,
                 void *buffer, size_t length)
{
    struct piranha_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct piranha_transport_data *) data;

    rc = xmm626_sec_modem_read(transport_data->fd, buffer, length);

    return rc;
}

int piranha_write(__attribute__((unused)) struct ipc_client *client, void *data,
                  const void *buffer, size_t length)
{
    struct piranha_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct piranha_transport_data *) data;

    rc = xmm626_sec_modem_write(transport_data->fd, buffer, length);

    return rc;
}

int piranha_poll(__attribute__((unused)) struct ipc_client *client, void *data,
                 struct ipc_poll_fds *fds, struct timeval *timeout)
{
    struct piranha_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct piranha_transport_data *) data;

    rc = xmm626_sec_modem_poll(transport_data->fd, fds, timeout);

    return rc;
}

int piranha_power_on(__attribute__((unused)) struct ipc_client *client,
                     __attribute__((unused)) void *data)
{
    return 0;
}

int piranha_power_off(__attribute__((unused)) struct ipc_client *client,
                      __attribute__((unused)) void *data)
{
    int fd;
    int rc;

    fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
        return -1;

    rc = xmm626_sec_modem_power(client, fd, 0);

    close(fd);

    if (rc < 0)
        return -1;

    return 0;
}

int piranha_gprs_activate(__attribute__((unused)) void *data,
                          __attribute__((unused)) unsigned int cid)
{
    return 0;
}

int piranha_gprs_deactivate(__attribute__((unused)) void *data,
                            __attribute__((unused)) unsigned int cid)
{
    return 0;
}

int piranha_data_create(void **transport_data,
                        __attribute__((unused)) void **power_data,
                        __attribute__((unused)) void **gprs_data)
{
    if (transport_data == NULL)
        return -1;

    *transport_data = calloc(1, sizeof(struct piranha_transport_data));

    return 0;
}

int piranha_data_destroy(void *transport_data,
                         __attribute__((unused)) void *power_data,
                         __attribute__((unused)) void *gprs_data)
{
    if (transport_data == NULL)
        return -1;

    free(transport_data);

    return 0;
}

struct ipc_client_ops piranha_fmt_ops = {
    .boot = piranha_boot,
    .send = xmm626_sec_modem_fmt_send,
    .recv = xmm626_sec_modem_fmt_recv,
};

struct ipc_client_ops piranha_rfs_ops = {
    .boot = NULL,
    .send = xmm626_sec_modem_rfs_send,
    .recv = xmm626_sec_modem_rfs_recv,
};

struct ipc_client_handlers piranha_handlers = {
    .read = piranha_read,
    .write = piranha_write,
    .open = piranha_open,
    .close = piranha_close,
    .poll = piranha_poll,
    .transport_data = NULL,
    .power_on = piranha_power_on,
    .power_off = piranha_power_off,
    .power_data = NULL,
    .gprs_activate = piranha_gprs_activate,
    .gprs_deactivate = piranha_gprs_deactivate,
    .gprs_data = NULL,
    .data_create = piranha_data_create,
    .data_destroy = piranha_data_destroy,
};

struct ipc_client_gprs_specs piranha_gprs_specs = {
    .gprs_get_iface = xmm626_sec_modem_gprs_get_iface,
    .gprs_get_capabilities = xmm626_sec_modem_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs piranha_nv_data_specs = {
    .nv_data_path = XMM626_NV_DATA_PATH,
    .nv_data_md5_path = XMM626_NV_DATA_MD5_PATH,
    .nv_data_backup_path = XMM626_NV_DATA_BACKUP_PATH,
    .nv_data_backup_md5_path = XMM626_NV_DATA_BACKUP_MD5_PATH,
    .nv_data_secret = XMM626_NV_DATA_SECRET,
    .nv_data_size = XMM626_NV_DATA_SIZE,
    .nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};

// vim:ts=4:sw=4:expandtab
