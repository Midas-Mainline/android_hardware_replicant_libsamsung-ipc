/*
 * This file is part of libsamsung-ipc.
 *
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

#include "xmm6260.h"
#include "xmm6260_hsic.h"
#include "xmm6260_sec_modem.h"
#include "galaxys3_ipc.h"

int galaxys3_ipc_bootstrap(struct ipc_client *client)
{
    void *modem_image_data = NULL;
    int modem_image_fd = -1;
    int modem_boot_fd = -1;
    int modem_link_fd = -1;

    unsigned char *p;
    int rc;

    if (client == NULL)
        return -1;

    ipc_client_log(client, "Starting galaxys3 modem bootstrap");

    modem_image_fd = open(GALAXYS3_MODEM_IMAGE_DEVICE, O_RDONLY);
    if (modem_image_fd < 0) {
        ipc_client_log(client, "Opening modem image device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem image device");

    modem_image_data = mmap(0, GALAXYS3_MODEM_IMAGE_SIZE, PROT_READ, MAP_SHARED, modem_image_fd, 0);
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

    modem_link_fd = open(XMM6260_SEC_MODEM_LINK_PM_DEVICE, O_RDWR);
    if (modem_link_fd < 0) {
        ipc_client_log(client, "Opening modem link device failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem link device");

    rc = xmm6260_sec_modem_hci_power(0);
    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem off");

    rc = xmm6260_sec_modem_power(modem_boot_fd, 1);
    rc |= xmm6260_sec_modem_hci_power(1);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }
    ipc_client_log(client, "Turned the modem on");

    rc = xmm6260_sec_modem_link_connected_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for link connected failed");
        goto error;
    }
    ipc_client_log(client, "Waited for link connected");

    p = (unsigned char *) modem_image_data + GALAXYS3_PSI_OFFSET;

    rc = xmm6260_hsic_psi_send(client, modem_boot_fd, (void *) p, GALAXYS3_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC PSI failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC PSI");

    p = (unsigned char *) modem_image_data + GALAXYS3_EBL_OFFSET;

    rc = xmm6260_hsic_ebl_send(client, modem_boot_fd, (void *) p, GALAXYS3_EBL_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC EBL failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC EBL");

    rc = xmm6260_hsic_port_config_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC port config failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC port config");

    p = (unsigned char *) modem_image_data + GALAXYS3_SEC_START_OFFSET;

    rc = xmm6260_hsic_sec_start_send(client, modem_boot_fd, (void *) p, GALAXYS3_SEC_START_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC SEC start failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC SEC start");

    p = (unsigned char *) modem_image_data + GALAXYS3_FIRMWARE_OFFSET;

    rc = xmm6260_hsic_firmware_send(client, modem_boot_fd, (void *) p, GALAXYS3_FIRMWARE_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC firmware failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC firmware");

    rc = xmm6260_hsic_nv_data_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC nv_data");

    rc = xmm6260_hsic_sec_end_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC SEC end failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC SEC end");

    rc = xmm6260_hsic_hw_reset_send(client, modem_boot_fd);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6260 HSIC HW reset failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6260 HSIC HW reset");

    usleep(300000);

    rc = xmm6260_sec_modem_link_get_hostwake_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for host wake failed");
    }

    rc = xmm6260_sec_modem_link_control_enable(modem_link_fd, 0);
    rc |= xmm6260_sec_modem_hci_power(0);
    rc |= xmm6260_sec_modem_link_control_active(modem_link_fd, 0);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem off failed");
        goto error;
    }

    rc = xmm6260_sec_modem_link_get_hostwake_wait(modem_link_fd);
    if (rc < 0) {
        ipc_client_log(client, "Waiting for host wake failed");
        goto error;
    }
    ipc_client_log(client, "Waited for host wake");

    rc = xmm6260_sec_modem_link_control_enable(modem_link_fd, 1);
    rc |= xmm6260_sec_modem_hci_power(1);
    rc |= xmm6260_sec_modem_link_control_active(modem_link_fd, 1);

    if (rc < 0) {
        ipc_client_log(client, "Turning the modem on failed");
        goto error;
    }

    rc = xmm6260_sec_modem_link_connected_wait(modem_link_fd);
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
        munmap(modem_image_data, GALAXYS3_MODEM_IMAGE_SIZE);

    if (modem_image_fd >= 0)
        close(modem_image_fd);

    if (modem_boot_fd >= 0)
        close(modem_boot_fd);

    if (modem_link_fd >= 0)
        close(modem_link_fd);

    return rc;
}


int galaxys3_ipc_fmt_send(struct ipc_client *client, struct ipc_message_info *request)
{
    return xmm6260_sec_modem_ipc_fmt_send(client, request);
}

int galaxys3_ipc_fmt_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    return xmm6260_sec_modem_ipc_fmt_recv(client, response);
}

int galaxys3_ipc_rfs_send(struct ipc_client *client, struct ipc_message_info *request)
{
    return xmm6260_sec_modem_ipc_rfs_send(client, request);
}

int galaxys3_ipc_rfs_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    return xmm6260_sec_modem_ipc_rfs_recv(client, response);
}

int galaxys3_ipc_open(void *data, int type)
{
    struct galaxys3_ipc_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct galaxys3_ipc_transport_data *) data;

    transport_data->fd = xmm6260_sec_modem_ipc_open(type);
    if (transport_data->fd < 0)
        return -1;

    return 0;
}

int galaxys3_ipc_close(void *data)
{
    struct galaxys3_ipc_transport_data *transport_data;

    if (data == NULL)
        return -1;

    transport_data = (struct galaxys3_ipc_transport_data *) data;

    xmm6260_sec_modem_ipc_close(transport_data->fd);
    transport_data->fd = -1;

    return 0;
}

int galaxys3_ipc_read(void *data, void *buffer, unsigned int length)
{
    struct galaxys3_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct galaxys3_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_read(transport_data->fd, buffer, length);
    return rc;
}

int galaxys3_ipc_write(void *data, void *buffer, unsigned int length)
{
    struct galaxys3_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct galaxys3_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_write(transport_data->fd, buffer, length);
    return rc;
}

int galaxys3_ipc_poll(void *data, struct timeval *timeout)
{
    struct galaxys3_ipc_transport_data *transport_data;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct galaxys3_ipc_transport_data *) data;

    rc = xmm6260_sec_modem_ipc_poll(transport_data->fd, timeout);
    return rc;
}

int galaxys3_ipc_power_on(void *data)
{
    return 0;
}

int galaxys3_ipc_power_off(void *data)
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

int galaxys3_ipc_data_create(void **transport_data, void **power_data, void **gprs_data)
{
    if (transport_data == NULL)
        return -1;

    *transport_data = (void *) malloc(sizeof(struct galaxys3_ipc_transport_data));
    memset(*transport_data, 0, sizeof(struct galaxys3_ipc_transport_data));

    return 0;
}

int galaxys3_ipc_data_destroy(void *transport_data, void *power_data, void *gprs_data)
{
    if (transport_data == NULL)
        return -1;

    free(transport_data);

    return 0;
}

char *galaxys3_ipc_gprs_get_iface(int cid)
{
    return xmm6260_sec_modem_ipc_gprs_get_iface(cid);
}


int galaxys3_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    return xmm6260_sec_modem_ipc_gprs_get_capabilities(capabilities);
}

struct ipc_ops galaxys3_ipc_fmt_ops = {
    .bootstrap = galaxys3_ipc_bootstrap,
    .send = galaxys3_ipc_fmt_send,
    .recv = galaxys3_ipc_fmt_recv,
};

struct ipc_ops galaxys3_ipc_rfs_ops = {
    .bootstrap = NULL,
    .send = galaxys3_ipc_rfs_send,
    .recv = galaxys3_ipc_rfs_recv,
};

struct ipc_handlers galaxys3_ipc_handlers = {
    .read = galaxys3_ipc_read,
    .write = galaxys3_ipc_write,
    .open = galaxys3_ipc_open,
    .close = galaxys3_ipc_close,
    .poll = galaxys3_ipc_poll,
    .transport_data = NULL,
    .power_on = galaxys3_ipc_power_on,
    .power_off = galaxys3_ipc_power_off,
    .power_data = NULL,
    .gprs_activate = NULL,
    .gprs_deactivate = NULL,
    .gprs_data = NULL,
    .data_create = galaxys3_ipc_data_create,
    .data_destroy = galaxys3_ipc_data_destroy,
};

struct ipc_gprs_specs galaxys3_ipc_gprs_specs = {
    .gprs_get_iface = galaxys3_ipc_gprs_get_iface,
    .gprs_get_capabilities = galaxys3_ipc_gprs_get_capabilities,
};

// vim:ts=4:sw=4:expandtab
