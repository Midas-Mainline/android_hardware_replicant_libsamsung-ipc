/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2017 Wolfgang Wiedmeyer <wolfgit@wiedmeyer.de>
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

#include "xmm626.h"
#include "xmm626_hsic.h"
#include "xmm626_sec_modem.h"
#include "n5100.h"

int n5100_boot(struct ipc_client *client)
{
    void *modem_image_data = NULL;
    int modem_image_fd = -1;
    int modem_boot_fd = -1;
    int modem_link_fd = -1;
    unsigned char *p;
    int rc;

    if (client == NULL)
        return -1;

    ipc_client_log(client, "Starting n5100 modem boot");

    modem_image_fd = open(N5100_MODEM_IMAGE_DEVICE, O_RDONLY);
    if (modem_image_fd < 0) {
        ipc_client_log(client, "Opening modem image device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem image device");

    modem_image_data = mmap(0, N5100_MODEM_IMAGE_SIZE, PROT_READ, MAP_SHARED, modem_image_fd, 0);
    if (modem_image_data == NULL || modem_image_data == (void *) 0xffffffff) {
            ipc_client_log(client, "Mapping modem image data to memory failed");
            goto error;
    }
    ipc_client_log(client, "Mapped modem image data to memory");

    modem_boot_fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (modem_boot_fd < 0) {
        ipc_client_log(client, "Opening modem boot device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem boot device");

    modem_link_fd = open(XMM626_SEC_MODEM_LINK_PM_DEVICE, O_RDWR);
    if (modem_link_fd < 0) {
        ipc_client_log(client, "Opening modem link device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem link device");

    rc = xmm626_sec_modem_hci_power(0);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem off");

    rc = xmm626_sec_modem_power(modem_boot_fd, 1);
    rc |= xmm626_sec_modem_hci_power(1);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem on");

    rc = xmm626_sec_modem_link_connected_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for link connected failed");
        goto error;
    }
    ipc_client_log(client, "Waited for link connected");

    p = (unsigned char *) modem_image_data + N5100_PSI_OFFSET;

    rc = xmm626_hsic_psi_send(client, modem_boot_fd, (void *) p, N5100_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC PSI failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC PSI");

    p = (unsigned char *) modem_image_data + N5100_EBL_OFFSET;

    rc = xmm626_hsic_ebl_send(client, modem_boot_fd, (void *) p, N5100_EBL_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC EBL failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC EBL");

    rc = xmm626_hsic_port_config_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC port config failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC port config");

    p = (unsigned char *) modem_image_data + N5100_SEC_START_OFFSET;

    rc = xmm626_hsic_sec_start_send(client, modem_boot_fd, (void *) p, N5100_SEC_START_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC SEC start failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC SEC start");

    p = (unsigned char *) modem_image_data + N5100_FIRMWARE_OFFSET;

    rc = xmm626_hsic_firmware_send(client, modem_boot_fd, (void *) p, N5100_FIRMWARE_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC firmware failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC firmware");

    rc = xmm626_hsic_nv_data_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC nv_data");

    rc = xmm626_hsic_sec_end_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC SEC end failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC SEC end");

    rc = xmm626_hsic_hw_reset_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM626 HSIC HW reset failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM626 HSIC HW reset");

    usleep(300000);

    rc = xmm626_sec_modem_link_get_hostwake_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for host wake failed");
    }

    rc = xmm626_sec_modem_link_control_enable(modem_link_fd, 0);
    rc |= xmm626_sec_modem_hci_power(0);
    rc |= xmm626_sec_modem_link_control_active(modem_link_fd, 0);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }

    rc = xmm626_sec_modem_link_get_hostwake_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for host wake failed");
        goto error;
    }
    ipc_client_log(client, "Waited for host wake");

    rc = xmm626_sec_modem_link_control_enable(modem_link_fd, 1);
    rc |= xmm626_sec_modem_hci_power(1);
    rc |= xmm626_sec_modem_link_control_active(modem_link_fd, 1);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }

    rc = xmm626_sec_modem_link_connected_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for link connected failed");
        goto error;
    }
    ipc_client_log(client, "Waited for link connected");

    usleep(300000);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (modem_image_data != NULL)
        munmap(modem_image_data, N5100_MODEM_IMAGE_SIZE);

    if (modem_image_fd >= 0)
        close(modem_image_fd);

    if (modem_boot_fd >= 0)
        close(modem_boot_fd);

    if (modem_link_fd >= 0)
        close(modem_link_fd);

    return rc;
}

int n5100_open(void *data, int type)
{
    struct n5100_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct n5100_transport_data *) data;

    transport_data->fd = xmm626_sec_modem_open(type);
    if (transport_data->fd < 0)
        return -1;

    return 0;
}

int n5100_close(void *data)
{
    struct n5100_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct n5100_transport_data *) data;

    xmm626_sec_modem_close(transport_data->fd);
    transport_data->fd = -1;

    return 0;
}

int n5100_read(void *data, void *buffer, size_t length)
{
    struct n5100_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct n5100_transport_data *) data;

    rc = xmm626_sec_modem_read(transport_data->fd, buffer, length);

    return rc;
}

int n5100_write(void *data, const void *buffer, size_t length)
{
    struct n5100_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct n5100_transport_data *) data;

    rc = xmm626_sec_modem_write(transport_data->fd, buffer, length);

    return rc;
}

int n5100_poll(void *data, struct ipc_poll_fds *fds, struct timeval *timeout)
{
    struct n5100_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct n5100_transport_data *) data;

    rc = xmm626_sec_modem_poll(transport_data->fd, fds, timeout);

    return rc;
}

int n5100_power_on(__attribute__((unused)) void *data)
{
    return 0;
}

int n5100_power_off(__attribute__((unused)) void *data)
{
    int fd;
    int rc;

    fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
        return -1;

    rc = xmm626_sec_modem_power(fd, 0);

    close(fd);

    if (rc < 0)
        return -1;

    return 0;
}

int n5100_gprs_activate(__attribute__((unused)) void *data,
			__attribute__((unused)) unsigned int cid)
{
    return 0;
}

int n5100_gprs_deactivate(__attribute__((unused)) void *data,
			  __attribute__((unused)) unsigned int cid)
{
    return 0;
}

int n5100_data_create(void **transport_data,
		      __attribute__((unused)) void **power_data,
		      __attribute__((unused)) void **gprs_data)
{
    if (transport_data == NULL)
        return -1;

    *transport_data = calloc(1, sizeof(struct n5100_transport_data));

    return 0;
}

int n5100_data_destroy(void *transport_data,
		       __attribute__((unused)) void *power_data,
		       __attribute__((unused)) void *gprs_data)
{
    if (transport_data == NULL)
        return -1;

    free(transport_data);

    return 0;
}

struct ipc_client_ops n5100_fmt_ops = {
    .boot = n5100_boot,
    .send = xmm626_sec_modem_fmt_send,
    .recv = xmm626_sec_modem_fmt_recv,
};

struct ipc_client_ops n5100_rfs_ops = {
    .boot = NULL,
    .send = xmm626_sec_modem_rfs_send,
    .recv = xmm626_sec_modem_rfs_recv,
};

struct ipc_client_handlers n5100_handlers = {
    .read = n5100_read,
    .write = n5100_write,
    .open = n5100_open,
    .close = n5100_close,
    .poll = n5100_poll,
    .transport_data = NULL,
    .power_on = n5100_power_on,
    .power_off = n5100_power_off,
    .power_data = NULL,
    .gprs_activate = n5100_gprs_activate,
    .gprs_deactivate = n5100_gprs_deactivate,
    .gprs_data = NULL,
    .data_create = n5100_data_create,
    .data_destroy = n5100_data_destroy,
};

struct ipc_client_gprs_specs n5100_gprs_specs = {
    .gprs_get_iface = xmm626_sec_modem_gprs_get_iface,
    .gprs_get_capabilities = xmm626_sec_modem_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs n5100_nv_data_specs = {
    .nv_data_path = XMM626_NV_DATA_PATH,
    .nv_data_md5_path = XMM626_NV_DATA_MD5_PATH,
    .nv_data_backup_path = XMM626_NV_DATA_BACKUP_PATH,
    .nv_data_backup_md5_path = XMM626_NV_DATA_BACKUP_MD5_PATH,
    .nv_data_secret = XMM626_NV_DATA_SECRET,
    .nv_data_size = XMM626_NV_DATA_SIZE,
    .nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};

// vim:ts=4:sw=4:expandtab
