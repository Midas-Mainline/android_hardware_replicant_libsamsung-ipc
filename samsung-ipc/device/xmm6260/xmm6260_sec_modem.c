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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <samsung-ipc.h>
#include <ipc.h>
#include <util.h>

#include "modem.h"
#include "modem_prj.h"
#include "modem_link_device_hsic.h"

#include "xmm6260.h"
#include "xmm6260_sec_modem.h"

int xmm6260_sec_modem_power(int device_fd, int power)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, power ? IOCTL_MODEM_ON : IOCTL_MODEM_OFF, 0);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_sec_modem_boot_power(int device_fd, int power)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, power ? IOCTL_MODEM_BOOT_ON : IOCTL_MODEM_BOOT_OFF, 0);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_sec_modem_status_online_wait(int device_fd)
{
    int status;
    int i;

    if (device_fd < 0)
        return -1;

    i = 0;
    for (i = 0; i < 100; i++) {
        status = ioctl(device_fd, IOCTL_MODEM_STATUS, 0);
        if (status == STATE_ONLINE)
            return 0;

        usleep(50000);
    }

    return -1;
}

int xmm6260_sec_modem_hci_power(int power)
{
    int ehci_rc, ohci_rc;

    ehci_rc = sysfs_value_write(XMM6260_SEC_MODEM_EHCI_POWER_SYSFS, !!power);
    if (ehci_rc >= 0)
        usleep(50000);

    ohci_rc = sysfs_value_write(XMM6260_SEC_MODEM_OHCI_POWER_SYSFS, !!power);
    if (ohci_rc >= 0)
        usleep(50000);

    if (ehci_rc < 0 && ohci_rc < 0)
        return -1;

    return 0;
}

int xmm6260_sec_modem_link_control_enable(int device_fd, int enable)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, IOCTL_LINK_CONTROL_ENABLE, &enable);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_sec_modem_link_control_active(int device_fd, int active)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, IOCTL_LINK_CONTROL_ACTIVE, &active);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_sec_modem_link_connected_wait(int device_fd)
{
    int status;
    int i;

    if (device_fd < 0)
        return -1;

    i = 0;
    for (i = 0; i < 100; i++) {
        status = ioctl(device_fd, IOCTL_LINK_CONNECTED, 0);
        if (status)
            return 0;

        usleep(50000);
    }

    return -1;
}

int xmm6260_sec_modem_link_get_hostwake_wait(int device_fd)
{
    int status;
    int i;

    if (device_fd < 0)
        return -1;

    i = 0;
    for (i = 0; i < 10; i++) {
        status = ioctl(device_fd, IOCTL_LINK_GET_HOSTWAKE, 0);
        if (status)
            return 0;

        usleep(50000);
    }

    return -1;
}

int xmm6260_sec_modem_ipc_fmt_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct ipc_fmt_header header;
    void *buffer;
    unsigned char *p;
    int count;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->write == NULL || request == NULL)
        return -1;

    ipc_fmt_header_fill(&header, request);

    buffer = malloc(header.length);

    memcpy(buffer, &header, sizeof(struct ipc_fmt_header));
    if (request->data != NULL && request->length > 0)
        memcpy((void *) ((unsigned char *) buffer + sizeof(struct ipc_fmt_header)), request->data, request->length);

    ipc_client_log_send(client, request, __func__);

    p = (unsigned char *) buffer;

    count = 0;
    while (count < header.length) {
        rc = client->handlers->write(client->handlers->transport_data, p, header.length - count);
        if (rc <= 0) {
            ipc_client_log(client, "Writing FMT data to the modem failed");
            goto error;
        }

        count += rc;
        p += rc;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_sec_modem_ipc_fmt_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct ipc_fmt_header *header;
    void *buffer = NULL;
    unsigned char *p;
    int length;
    int count;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->read == NULL || response == NULL)
        return -1;

    length = XMM6260_DATA_SIZE;
    buffer = malloc(length);

    rc = client->handlers->read(client->handlers->transport_data, buffer, length);
    if (rc < (int) sizeof(struct ipc_fmt_header)) {
        ipc_client_log(client, "Reading FMT header from the modem failed");
        goto error;
    }

    header = (struct ipc_fmt_header *) buffer;

    ipc_fmt_message_fill(header, response);

    if (header->length > sizeof(struct ipc_fmt_header)) {
        response->length = header->length - sizeof(struct ipc_fmt_header);
        response->data = malloc(response->length);

        p = (unsigned char *) response->data;

        count = rc - sizeof(struct ipc_fmt_header);
        if (count > 0) {
            memcpy(p, (void *) ((unsigned char *) buffer + sizeof(struct ipc_fmt_header)), count);
            p += count;
        }

        while (count < (int) response->length) {
            rc = client->handlers->read(client->handlers->transport_data, p, response->length - count);
            if (rc <= 0) {
                ipc_client_log(client, "Reading FMT data from the modem failed");
                goto error;
            }

            count += rc;
            p += rc;
        }
    }

    ipc_client_log_recv(client, response, __func__);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_sec_modem_ipc_rfs_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct ipc_rfs_header header;
    void *buffer;
    unsigned char *p;
    int count;
    int rc;


    if (client == NULL || client->handlers == NULL || client->handlers->write == NULL || request == NULL)
        return -1;

    ipc_rfs_header_fill(&header, request);

    buffer = malloc(header.length);

    memcpy(buffer, &header, sizeof(struct ipc_rfs_header));
    if (request->data != NULL && request->length > 0)
        memcpy((void *) ((unsigned char *) buffer + sizeof(struct ipc_rfs_header)), request->data, request->length);

    ipc_client_log_send(client, request, __func__);

    p = (unsigned char *) buffer;

    count = 0;
    while (count < (int) header.length) {
        rc = client->handlers->write(client->handlers->transport_data, p, header.length - count);
        if (rc <= 0) {
            ipc_client_log(client, "Writing RFS data to the modem failed");
            goto error;
        }

        count += rc;
        p += rc;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_sec_modem_ipc_rfs_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct ipc_rfs_header *header;
    void *buffer = NULL;
    unsigned char *p;
    int length;
    int count;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->read == NULL || response == NULL)
        return -1;

    length = XMM6260_DATA_SIZE;
    buffer = malloc(length);

    rc = client->handlers->read(client->handlers->transport_data, buffer, length);
    if (rc < (int) sizeof(struct ipc_rfs_header)) {
        ipc_client_log(client, "Reading RFS header from the modem failed");
        goto error;
    }

    header = (struct ipc_rfs_header *) buffer;

    ipc_rfs_message_fill(header, response);

    if (header->length > sizeof(struct ipc_rfs_header)) {
        response->length = header->length - sizeof(struct ipc_rfs_header);
        response->data = malloc(response->length);

        p = (unsigned char *) response->data;

        count = rc - sizeof(struct ipc_rfs_header);
        if (count > 0) {
            memcpy(p, (void *) ((unsigned char *) buffer + sizeof(struct ipc_rfs_header)), count);
            p += count;
        }

        while (count < (int) response->length) {
            rc = client->handlers->read(client->handlers->transport_data, p, response->length - count);
            if (rc <= 0) {
                ipc_client_log(client, "Reading RFS data from the modem failed");
                goto error;
            }

            count += rc;
            p += rc;
        }
    }

    ipc_client_log_recv(client, response, __func__);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_sec_modem_ipc_open(int type)
{
    int fd;

    switch (type) {
        case IPC_CLIENT_TYPE_FMT:
            fd = open(XMM6260_SEC_MODEM_IPC0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        case IPC_CLIENT_TYPE_RFS:
            fd = open(XMM6260_SEC_MODEM_RFS0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        default:
            return -1;
    }

    return fd;
}

int xmm6260_sec_modem_ipc_close(int fd)
{
    if (fd < 0)
        return -1;

    close(fd);

    return 0;
}

int xmm6260_sec_modem_ipc_read(int fd, void *buffer, unsigned int length)
{
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    rc = read(fd, buffer, length);
    return rc;
}

int xmm6260_sec_modem_ipc_write(int fd, void *buffer, unsigned int length)
{
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    rc = write(fd, buffer, length);
    return rc;
}

int xmm6260_sec_modem_ipc_poll(int fd, struct timeval *timeout)
{
    fd_set fds;
    int rc;
    int status;

    if (fd < 0)
        return -1;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    rc = select(fd + 1, &fds, NULL, NULL, timeout);
    if (FD_ISSET(fd, &fds)) {
        status = ioctl(fd, IOCTL_MODEM_STATUS, 0);
        if (status != STATE_ONLINE && status != STATE_BOOTING)
            return 0;
    }

    return rc;
}

char *xmm6260_sec_modem_ipc_gprs_get_iface(int cid)
{
    char *iface = NULL;

    if (cid > XMM6260_SEC_MODEM_GPRS_IFACE_COUNT)
        return NULL;

    asprintf(&iface, "%s%d", XMM6260_SEC_MODEM_GPRS_IFACE_PREFIX, cid - 1);

    return iface;
}

int xmm6260_sec_modem_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    if (capabilities == NULL)
        return -1;

    capabilities->port_list = 0;
    capabilities->cid_max = XMM6260_SEC_MODEM_GPRS_IFACE_COUNT;

    return 0;
}

// vim:ts=4:sw=4:expandtab
