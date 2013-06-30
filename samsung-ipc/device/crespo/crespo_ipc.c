/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011 Joerie de Gram <j.de.gram@gmail.com>
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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <samsung-ipc.h>
#include <ipc.h>
#include <util.h>

#include "crespo_modem_ctl.h"

#include "xmm6160.h"
#include "crespo_ipc.h"

int crespo_ipc_bootstrap(struct ipc_client *client)
{
    void *modem_image_data = NULL;

    int modem_ctl_fd = -1;
    int serial_fd = -1;

    unsigned char *p;
    int rc;

    if (client == NULL)
        return -1;

    ipc_client_log(client, "Starting crespo modem bootstrap");

    modem_image_data = file_data_read(CRESPO_MODEM_IMAGE_DEVICE, CRESPO_MODEM_IMAGE_SIZE, 0x1000);
    if (modem_image_data == NULL) {
        ipc_client_log(client, "Reading modem image data failed");
        goto error;
    }
    ipc_client_log(client, "Read modem image data");

    modem_ctl_fd = open(CRESPO_MODEM_CTL_DEVICE, O_RDWR | O_NDELAY);
    if (modem_ctl_fd < 0) {
        ipc_client_log(client, "Opening modem ctl failed");
        goto error;
    }
    ipc_client_log(client, "Opened modem ctl");

    rc = ioctl(modem_ctl_fd, IOCTL_MODEM_RESET);
    if (rc < 0) {
        ipc_client_log(client, "Resetting modem failed");
        goto error;
    }
    ipc_client_log(client, "Reset modem");

    serial_fd = open(CRESPO_MODEM_SERIAL_DEVICE, O_RDWR | O_NDELAY);
    if (serial_fd < 0) {
        ipc_client_log(client, "Opening serial failed");
        goto error;
    }
    ipc_client_log(client, "Opened serial");

    usleep(100000);

    p = (unsigned char *) modem_image_data;

    rc = xmm6160_psi_send(client, serial_fd, (void *) p, CRESPO_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6160 PSI failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6160 PSI");

    p += CRESPO_PSI_SIZE;

    lseek(modem_ctl_fd, 0, SEEK_SET);

    rc = xmm6160_modem_image_send(client, modem_ctl_fd, NULL, (void *) p, CRESPO_MODEM_IMAGE_SIZE - CRESPO_PSI_SIZE);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6160 modem image failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6160 modem image");

    lseek(modem_ctl_fd, CRESPO_MODEM_CTL_NV_DATA_OFFSET, SEEK_SET);

    rc = xmm6160_nv_data_send(client, modem_ctl_fd, NULL);
    if (rc < 0) {
        ipc_client_log(client, "Sending XMM6160 nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Sent XMM6160 nv_data");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (modem_image_data != NULL)
        free(modem_image_data);

    if (serial_fd >= 0)
        close(serial_fd);

    if (modem_ctl_fd >= 0)
        close(modem_ctl_fd);

    return rc;
}

int crespo_ipc_fmt_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct ipc_header header;
    struct modem_io mio;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->write == NULL || request == NULL)
        return -1;

    ipc_header_fill(&header, request);

    memset(&mio, 0, sizeof(struct modem_io));
    mio.size = request->length + sizeof(struct ipc_header);
    mio.data = malloc(mio.size);
    memcpy(mio.data, &header, sizeof(struct ipc_header));
    if (request->data != NULL && request->length > 0)
        memcpy((void *) ((unsigned char *) mio.data + sizeof(struct ipc_header)), request->data, request->length);

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write(client->handlers->transport_data, (void *) &mio, sizeof(struct modem_io));

    free(mio.data);

    return rc;
}

int crespo_ipc_fmt_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct ipc_header *header;
    struct modem_io mio;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->read == NULL || response == NULL)
        return -1;

    memset(&mio, 0, sizeof(struct modem_io));
    mio.size = CRESPO_DATA_SIZE;
    mio.data = malloc(mio.size);

    rc = client->handlers->read(client->handlers->transport_data, &mio, sizeof(struct modem_io) + mio.size);
    if (rc < 0 || mio.data == NULL || mio.size < sizeof(struct ipc_header)) {
        ipc_client_log(client, "Reading FMT data from the modem failed");
        goto error;
    }

    header = (struct ipc_header *) mio.data;
    ipc_message_info_fill(header, response);

    if (mio.size > sizeof(struct ipc_header)) {
        response->length = mio.size - sizeof(struct ipc_header);
        response->data = malloc(response->length);
        memcpy(response->data, (void *) ((unsigned char *) mio.data + sizeof(struct ipc_header)), response->length);
    }

    ipc_client_log_recv(client, response, __func__);

    goto complete;

error:
    rc = -1;

complete:
    if (mio.data != NULL)
        free(mio.data);

    return rc;
}

int crespo_ipc_rfs_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct modem_io mio;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->write == NULL || request == NULL)
        return -1;

    memset(&mio, 0, sizeof(struct modem_io));
    mio.id = request->mseq;
    mio.cmd = request->index;
    mio.size = request->length;
    if (request->data != NULL && request->length > 0) {
        mio.data = malloc(mio.size);
        memcpy(mio.data, request->data, request->length);
    }

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write(client->handlers->transport_data, (void *) &mio, sizeof(struct modem_io));

    if (mio.data != NULL)
        free(mio.data);

    return rc;
}

int crespo_ipc_rfs_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct modem_io mio;
    int rc;

    if (client == NULL || client->handlers == NULL || client->handlers->read == NULL || response == NULL)
        return -1;

    memset(&mio, 0, sizeof(struct modem_io));
    mio.size = CRESPO_DATA_SIZE;
    mio.data = malloc(mio.size);

    rc = client->handlers->read(client->handlers->transport_data, &mio, sizeof(struct modem_io) + mio.size);
    if (rc < 0 || mio.data == NULL || mio.size <= 0) {
        ipc_client_log(client, "Reading RFS data from the modem failed");
        goto error;
    }

    memset(response, 0, sizeof(struct ipc_message_info));
    response->aseq = mio.id;
    response->group = IPC_GROUP_RFS;
    response->index = mio.cmd;

    if (mio.size > 0) {
        response->length = mio.size;
        response->data = malloc(response->length);
        memcpy(response->data, mio.data, response->length);
    }

    ipc_client_log_recv(client, response, __func__);

    goto complete;

error:
    rc = -1;

complete:
    if (mio.data != NULL)
        free(mio.data);

    return rc;
}

int crespo_ipc_open(void *data, int type)
{
    struct crespo_ipc_transport_data *transport_data;
    int fd;

    if (data == NULL)
        return -1;

    transport_data = (struct crespo_ipc_transport_data *) data;

    switch(type)
    {
        case IPC_CLIENT_TYPE_FMT:
            fd = open(CRESPO_MODEM_FMT_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        case IPC_CLIENT_TYPE_RFS:
            fd = open(CRESPO_MODEM_RFS_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        default:
            return -1;
    }

    if(fd < 0)
        return -1;

    transport_data->fd = fd;

    return 0;
}

int crespo_ipc_close(void *data)
{
    struct crespo_ipc_transport_data *transport_data;
    int fd;

    if (data == NULL)
        return -1;

    transport_data = (struct crespo_ipc_transport_data *) data;

    fd = transport_data->fd;
    if (fd < 0)
        return -1;

    transport_data->fd = -1;
    close(fd);

    return 0;
}

int crespo_ipc_read(void *data, void *buffer, unsigned int length)
{
    struct crespo_ipc_transport_data *transport_data;
    int fd;
    int rc;

    if (data == NULL || buffer == NULL || length == 0)
        return -1;

    transport_data = (struct crespo_ipc_transport_data *) data;

    fd = transport_data->fd;
    if (fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_RECV, buffer);
    if(rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_write(void *data, void *buffer, unsigned int length)
{
    struct crespo_ipc_transport_data *transport_data;
    int fd;
    int rc;

    if (data == NULL || buffer == NULL || length == 0)
        return -1;

    transport_data = (struct crespo_ipc_transport_data *) data;

    fd = transport_data->fd;
    if (fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_SEND, buffer);
    if (rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_poll(void *data, struct timeval *timeout)
{
    struct crespo_ipc_transport_data *transport_data;
    fd_set fds;
    int fd;
    int rc;

    if (data == NULL)
        return -1;

    transport_data = (struct crespo_ipc_transport_data *) data;

    fd = transport_data->fd;
    if (fd < 0)
        return -1;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    rc = select(FD_SETSIZE, &fds, NULL, NULL, timeout);
    return rc;
}

int crespo_ipc_power_on(void *data)
{
    int fd;
    int rc;

    fd = open(CRESPO_MODEM_CTL_DEVICE, O_RDWR);
    if (fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_START);

    close(fd);

    if (rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_power_off(void *data)
{
    int fd;
    int rc;

    fd = open(CRESPO_MODEM_CTL_DEVICE, O_RDWR);
    if (fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_OFF);

    close(fd);

    if (rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_data_create(void **transport_data, void **power_data, void **gprs_data)
{
    if (transport_data == NULL)
        return -1;

    *transport_data = (void *) malloc(sizeof(struct crespo_ipc_transport_data));
    memset(*transport_data, 0, sizeof(struct crespo_ipc_transport_data));

    return 0;
}

int crespo_ipc_data_destroy(void *transport_data, void *power_data, void *gprs_data)
{
    if (transport_data == NULL)
        return -1;

    free(transport_data);

    return 0;
}

char *crespo_ipc_gprs_get_iface_single(int cid)
{
    char *iface = NULL;

    asprintf(&iface, "%s%d", CRESPO_GPRS_IFACE_PREFIX, 0);

    return iface;
}

int crespo_ipc_gprs_get_capabilities_single(struct ipc_client_gprs_capabilities *capabilities)
{
    if (capabilities == NULL)
        return -1;

    capabilities->port_list = 0;
    capabilities->cid_max = 1;

    return 0;
}

char *crespo_ipc_gprs_get_iface(int cid)
{
    char *iface = NULL;

    if (cid > CRESPO_GPRS_IFACE_COUNT)
        return NULL;

    asprintf(&iface, "%s%d", CRESPO_GPRS_IFACE_PREFIX, cid - 1);

    return iface;
}

int crespo_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    if (capabilities == NULL)
        return -1;

    capabilities->port_list = 0;
    capabilities->cid_max = CRESPO_GPRS_IFACE_COUNT;

    return 0;
}

struct ipc_ops crespo_ipc_fmt_ops = {
    .bootstrap = crespo_ipc_bootstrap,
    .send = crespo_ipc_fmt_send,
    .recv = crespo_ipc_fmt_recv,
};

struct ipc_ops crespo_ipc_rfs_ops = {
    .bootstrap = NULL,
    .send = crespo_ipc_rfs_send,
    .recv = crespo_ipc_rfs_recv,
};

struct ipc_handlers crespo_ipc_handlers = {
    .open = crespo_ipc_open,
    .close = crespo_ipc_close,
    .read = crespo_ipc_read,
    .write = crespo_ipc_write,
    .poll = crespo_ipc_poll,
    .transport_data = NULL,
    .power_on = crespo_ipc_power_on,
    .power_off = crespo_ipc_power_off,
    .power_data = NULL,
    .gprs_activate = NULL,
    .gprs_deactivate = NULL,
    .gprs_data = NULL,
    .data_create = crespo_ipc_data_create,
    .data_destroy = crespo_ipc_data_destroy,
};

struct ipc_gprs_specs crespo_ipc_gprs_specs_single = {
    .gprs_get_iface = crespo_ipc_gprs_get_iface_single,
    .gprs_get_capabilities = crespo_ipc_gprs_get_capabilities_single,
};

struct ipc_gprs_specs crespo_ipc_gprs_specs = {
    .gprs_get_iface = crespo_ipc_gprs_get_iface,
    .gprs_get_capabilities = crespo_ipc_gprs_get_capabilities,
};

// vim:ts=4:sw=4:expandtab
