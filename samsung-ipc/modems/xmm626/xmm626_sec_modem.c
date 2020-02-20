/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
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

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <samsung-ipc.h>
#include <ipc.h>

#include "xmm626_modem_if.h"
#include "xmm626_modem_prj.h"
#include "xmm626_modem_link_device_hsic.h"

#include "xmm626.h"
#include "xmm626_sec_modem.h"

int xmm626_sec_modem_power(__attribute__((unused)) struct ipc_client *client,
                           int device_fd, int power)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, power ? IOCTL_MODEM_ON : IOCTL_MODEM_OFF, 0);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_sec_modem_boot_power(__attribute__((unused)) struct ipc_client *client,
                                int device_fd, int power)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, power ? IOCTL_MODEM_BOOT_ON : IOCTL_MODEM_BOOT_OFF,
               0);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_sec_modem_status_online_wait(__attribute__((unused)) struct ipc_client *client,
                                        int device_fd)
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

int xmm626_sec_modem_hci_power(__attribute__((unused)) struct ipc_client *client,
                               int power)
{
    int ehci_rc, ohci_rc;

    ehci_rc = sysfs_value_write(XMM626_SEC_MODEM_EHCI_POWER_SYSFS, !!power);
    if (ehci_rc >= 0)
        usleep(50000);

    ohci_rc = sysfs_value_write(XMM626_SEC_MODEM_OHCI_POWER_SYSFS, !!power);
    if (ohci_rc >= 0)
        usleep(50000);

    if (ehci_rc < 0 && ohci_rc < 0)
        return -1;

    return 0;
}

int xmm626_sec_modem_link_control_enable(__attribute__((unused)) struct ipc_client *client,
                                         int device_fd, int enable)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, IOCTL_LINK_CONTROL_ENABLE, &enable);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_sec_modem_link_control_active(__attribute__((unused)) struct ipc_client *client,
                                         int device_fd, int active)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = ioctl(device_fd, IOCTL_LINK_CONTROL_ACTIVE, &active);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_sec_modem_link_connected_wait(__attribute__((unused)) struct ipc_client *client,
                                         int device_fd)
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

int xmm626_sec_modem_link_get_hostwake_wait(__attribute__((unused)) struct ipc_client *client,
                                            int device_fd)
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

int xmm626_sec_modem_fmt_send(struct ipc_client *client,
    struct ipc_message *message)
{
    struct ipc_fmt_header header;
    void *buffer;
    size_t length;
    size_t count;
    unsigned char *p;
    int rc;

    if (client == NULL || client->handlers == NULL ||
        client->handlers->write == NULL || message == NULL) {
      return -1;
    }

    ipc_fmt_header_setup(&header, message);

    length = header.length;
    buffer = calloc(1, length);

    memcpy(buffer, &header, sizeof(struct ipc_fmt_header));
    if (message->data != NULL && message->size > 0)
        memcpy((void *) ((unsigned char *) buffer +
                         sizeof(struct ipc_fmt_header)),
               message->data, message->size);

    ipc_client_log_send(client, message, __func__);

    p = (unsigned char *) buffer;

    count = 0;
    while (count < length) {
        rc = client->handlers->write(client, client->handlers->transport_data, p, length - count);
        if (rc <= 0) {
            ipc_client_log(client, "Writing FMT data failed");
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

int xmm626_sec_modem_fmt_recv(struct ipc_client *client,
    struct ipc_message *message)
{
    struct ipc_fmt_header *header;
    void *buffer = NULL;
    size_t length;
    size_t count;
    unsigned char *p;
    int rc;

    if (client == NULL || client->handlers == NULL ||
        client->handlers->read == NULL || message == NULL) {
      return -1;
    }

    length = XMM626_DATA_SIZE;
    buffer = calloc(1, length);

    rc = client->handlers->read(client, client->handlers->transport_data, buffer, length);
    if (rc < (int) sizeof(struct ipc_fmt_header)) {
        ipc_client_log(client, "Reading FMT header failed");
        goto error;
    }

    header = (struct ipc_fmt_header *) buffer;

    ipc_fmt_message_setup(header, message);

    if (header->length > sizeof(struct ipc_fmt_header)) {
        message->size = header->length - sizeof(struct ipc_fmt_header);
        message->data = calloc(1, message->size);

        p = (unsigned char *) message->data;

        count = rc - sizeof(struct ipc_fmt_header);
        if (count > 0) {
            memcpy(p, (void *) ((unsigned char *) buffer +
                                sizeof(struct ipc_fmt_header)), count);
            p += count;
        }

        while (count < message->size) {
            rc = client->handlers->read(client,
                                        client->handlers->transport_data, p,
                                        message->size - count);
            if (rc <= 0) {
                ipc_client_log(client, "Reading FMT data failed");
                goto error;
            }

            count += rc;
            p += rc;
        }
    }

    ipc_client_log_recv(client, message, __func__);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm626_sec_modem_rfs_send(struct ipc_client *client,
    struct ipc_message *message)
{
    struct ipc_rfs_header header;
    void *buffer;
    size_t length;
    size_t count;
    unsigned char *p;
    int rc;

    if (client == NULL || client->handlers == NULL ||
        client->handlers->write == NULL || message == NULL) {
      return -1;
    }

    ipc_rfs_header_setup(&header, message);

    length = header.length;
    buffer = calloc(1, length);

    memcpy(buffer, &header, sizeof(struct ipc_rfs_header));
    if (message->data != NULL && message->size > 0)
        memcpy((void *) ((unsigned char *) buffer +
                         sizeof(struct ipc_rfs_header)),
               message->data, message->size);

    ipc_client_log_send(client, message, __func__);

    p = (unsigned char *) buffer;

    count = 0;
    while (count < length) {
        rc = client->handlers->write(client, client->handlers->transport_data,
                                     p, length - count);
        if (rc <= 0) {
            ipc_client_log(client, "Writing RFS data failed");
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

int xmm626_sec_modem_rfs_recv(struct ipc_client *client,
    struct ipc_message *message)
{
    struct ipc_rfs_header *header;
    void *buffer = NULL;
    size_t length;
    size_t count;
    unsigned char *p;
    int rc;

    if (client == NULL || client->handlers == NULL ||
        client->handlers->read == NULL || message == NULL) {
      return -1;
    }

    length = XMM626_DATA_SIZE;
    buffer = calloc(1, length);

    rc = client->handlers->read(client, client->handlers->transport_data,
                                buffer, length);
    if (rc < (int) sizeof(struct ipc_rfs_header)) {
        ipc_client_log(client, "Reading RFS header failed");
        goto error;
    }

    header = (struct ipc_rfs_header *) buffer;
    if (header->length > XMM626_DATA_SIZE_LIMIT) {
        ipc_client_log(client, "Invalid RFS header length: %u", header->length);
        goto error;
    }

    ipc_rfs_message_setup(header, message);

    if (header->length > sizeof(struct ipc_rfs_header)) {
        message->size = header->length - sizeof(struct ipc_rfs_header);
        message->data = calloc(1, message->size);

        p = (unsigned char *) message->data;

        count = rc - sizeof(struct ipc_rfs_header);
        if (count > 0) {
            memcpy(p, (void *) ((unsigned char *) buffer +
                                sizeof(struct ipc_rfs_header)),
                   count);
            p += count;
        }

        while (count < message->size) {
            rc = client->handlers->read(client,
                                        client->handlers->transport_data, p,
                                        message->size - count);
            if (rc <= 0) {
                ipc_client_log(client, "Reading RFS data failed");
                goto error;
            }

            count += rc;
            p += rc;
        }
    }

    ipc_client_log_recv(client, message, __func__);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm626_sec_modem_open(int type)
{
    int fd;

    switch (type) {
        case IPC_CLIENT_TYPE_FMT:
            fd = open(XMM626_SEC_MODEM_IPC0_DEVICE,
                      O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        case IPC_CLIENT_TYPE_RFS:
            fd = open(XMM626_SEC_MODEM_RFS0_DEVICE,
                      O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        default:
            return -1;
    }

    return fd;
}

int xmm626_sec_modem_close(int fd)
{
    if (fd < 0)
        return -1;

    close(fd);

    return 0;
}

int xmm626_sec_modem_read(int fd, void *buffer, size_t length)
{
    int status;
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    status = ioctl(fd, IOCTL_MODEM_STATUS, 0);
    if (status != STATE_ONLINE && status != STATE_BOOTING)
        return -1;

    rc = read(fd, buffer, length);

    return rc;
}

int xmm626_sec_modem_write(int fd, const void *buffer, size_t length)
{
    int status;
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    status = ioctl(fd, IOCTL_MODEM_STATUS, 0);
    if (status != STATE_ONLINE && status != STATE_BOOTING)
        return -1;

    rc = write(fd, buffer, length);

    return rc;
}

int xmm626_sec_modem_poll(int fd, struct ipc_poll_fds *fds,
    struct timeval *timeout)
{
    int status;
    fd_set set;
    int fd_max;
    unsigned int i;
    unsigned int count;
    int rc;

    if (fd < 0)
        return -1;

    FD_ZERO(&set);
    FD_SET(fd, &set);

    fd_max = fd;

    if (fds != NULL && fds->fds != NULL && fds->count > 0) {
        for (i = 0; i < fds->count; i++) {
            if (fds->fds[i] >= 0) {
                FD_SET(fds->fds[i], &set);

                if (fds->fds[i] > fd_max)
                    fd_max = fds->fds[i];
            }
        }
    }

    rc = select(fd_max + 1, &set, NULL, NULL, timeout);

    if (FD_ISSET(fd, &set)) {
        status = ioctl(fd, IOCTL_MODEM_STATUS, 0);
        if (status != STATE_ONLINE && status != STATE_BOOTING)
            return -1;
    }

    if (fds != NULL && fds->fds != NULL && fds->count > 0) {
        count = fds->count;

        for (i = 0; i < fds->count; i++) {
            if (!FD_ISSET(fds->fds[i], &set)) {
                fds->fds[i] = -1;
                count--;
            }
        }

        fds->count = count;
    }

    return rc;
}

char *xmm626_sec_modem_gprs_get_iface(unsigned int cid)
{
    char *iface = NULL;

    if (cid > XMM626_SEC_MODEM_GPRS_IFACE_COUNT)
        return NULL;

    asprintf(&iface, "%s%d", XMM626_SEC_MODEM_GPRS_IFACE_PREFIX, cid - 1);

    return iface;
}

int xmm626_sec_modem_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    if (capabilities == NULL)
        return -1;

    capabilities->cid_count = XMM626_SEC_MODEM_GPRS_IFACE_COUNT;

    return 0;
}

// vim:ts=4:sw=4:expandtab
