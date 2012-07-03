/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 * based on crespo IPC code which is:
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *                    Joerie de Gram <j.de.gram@gmail.com>
 *                    Simon Busch <morphis@gravedo.de>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <mtd/mtd-abi.h>
#include <assert.h>

#include <radio.h>

#include "xmm6260_ipc.h"
#include "ipc_private.h"

#include "modemctl_common.h"

#define IPC_MAX_XFER 4096

int i9100_modem_bootstrap(struct ipc_client *client)
{
    return boot_modem_i9100();
}

int i9250_modem_bootstrap(struct ipc_client *client)
{
    return boot_modem_i9250();
}

int xmm6260_ipc_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct ipc_header *hdr;
    unsigned char *frame;
    unsigned char *payload;
    size_t frame_length;

    /* Frame IPC header + payload length */
    frame_length = (sizeof(*hdr) + request->length);
    
    frame = (unsigned char*)malloc(frame_length);
    hdr = (struct ipc_header*)frame;

    /* IPC header */
    hdr->length = frame_length;
    hdr->mseq = request->mseq;
    hdr->aseq = request->aseq;
    hdr->group = request->group;
    hdr->index = request->index;
    hdr->type = request->type;

    /* IPC payload */
    payload = (frame + sizeof(*hdr));
    memcpy(payload, request->data, request->length);

    ipc_client_log(client, "sending %s %s\n",
            ipc_command_to_str(IPC_COMMAND(request)),
            ipc_response_type_to_str(request->type));

    ipc_hex_dump(client, frame, frame_length);

    client->handlers->write(frame, frame_length,  client->handlers->write_data);

    free(frame);

    return 0;
}

int xmm6260_ipc_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    unsigned char buf[IPC_MAX_XFER] = {};
    unsigned char *data;
    unsigned short *frame_length;

    struct ipc_header ipc = {
        .length = 0,
    };
    int num_read = 0;
    int left = 0;

    num_read = client->handlers->read(buf, IPC_MAX_XFER,
        client->handlers->read_data);

    if (num_read < 0) {
        ipc_client_log(client, "read failed to read ipc length: %d", num_read);
        response->data = 0;
        response->length = 0;
        return 0;
    }

    memcpy(&ipc, buf, sizeof(ipc));
    left = ipc.length - num_read;
   
    if (left > 0) {
        num_read = client->handlers->read(buf + num_read, left,
            client->handlers->read_data);
    }

    memcpy(&ipc, buf, sizeof(ipc));

    response->mseq = ipc.mseq;
    response->aseq = ipc.aseq;
    response->group = ipc.group;
    response->index = ipc.index;
    response->type = ipc.type;
    response->length = ipc.length - sizeof(ipc);
    
    response->data = (unsigned char*)malloc(response->length);
    memcpy(response->data, buf + sizeof(ipc), response->length);

    ipc_client_log(client, "received %s %s\n",
        ipc_command_to_str(IPC_COMMAND(response)),
    
    ipc_response_type_to_str(response->type));
    ipc_hex_dump(client, response->data, left);

    return 0;
}

int xmm6260_rfs_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    unsigned char buf[IPC_MAX_XFER] = {};
    struct rfs_hdr header;
    int header_recv = 0;
    unsigned count=0;
    int rc;

    do {
        rc = client->handlers->read(buf, IPC_MAX_XFER, client->handlers->read_data);

        if (rc < 0) {
            ipc_client_log(client, "Failed to read RFS data.");
            return -1;
        }
        ipc_client_log(client, "received %d bytes", rc);

        // We didn't recieve the header yet
        if (!header_recv) {
            if ((unsigned)rc < sizeof(struct rfs_hdr)) {
                ipc_client_log(client, "Failed to read RFS data.");
                return -1;
            }

            memcpy((void *) &header, (void *) buf, sizeof(struct rfs_hdr));

            if (header.size < sizeof(struct rfs_hdr)) {
                ipc_client_log(client, "Invalid size in header");
                return -1;
            }

            response->mseq = 0;
            response->aseq = header.id;
            response->group = IPC_GROUP_RFS;
            response->index = header.cmd;
            response->type = 0;
            response->length = header.size - sizeof(struct rfs_hdr);
            response->data = NULL;

            ipc_client_log(client, "READ %d bytes, header size says %d",
                rc, header.size);
            ipc_client_log(client, "%s: RECV RFS (id=%d cmd=%d size=%d)!",
                __func__, header.id, header.cmd, header.size);
            ipc_client_log(client,
                "%s: IPC response (aseq=0x%02x command=%s (0x%04x))",
                __func__, response->mseq,
                ipc_command_to_str(IPC_COMMAND(response)),
                IPC_COMMAND(response));

            if (response->length > 0) {
                response->data = malloc(response->length);
                memcpy(response->data,
                    (void *) (buf + sizeof(struct rfs_hdr)),
                    rc - sizeof(struct rfs_hdr));
                ipc_client_log(client, "writing at offset 0 %d bytes",
                    rc - sizeof(struct rfs_hdr));
            }

            header_recv = 1;
        } else {
            // Still reading data, with no header
            memcpy((void *) (response->data + count - sizeof(struct rfs_hdr)), buf, rc);
        }

        count += rc;
    } while (count < header.size);

    return 0;
}

int xmm6260_rfs_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct rfs_hdr *header = NULL;
    char *data = NULL;
    int data_length;
    int rc;

    data_length = sizeof(struct rfs_hdr) + request->length;
    data = malloc(data_length);
    memset(data, 0, data_length);

    header = (struct rfs_hdr *) data;
    header->id = request->mseq;
    header->cmd = request->index;
    header->size = data_length;

    memcpy((void *) (data + sizeof(struct rfs_hdr)), request->data, request->length);

    ipc_client_log(client, "%s: SEND RFS (id=%d cmd=%d size=%d)!",
        __func__, header->id, header->cmd, header->size);
    ipc_client_log(client, "%s: IPC request (mseq=0x%02x command=%s (0x%04x))",
        __func__, request->mseq, ipc_command_to_str(IPC_COMMAND(request)),
        IPC_COMMAND(request));

    rc = client->handlers->write(data, data_length, client->handlers->write_data);
    return rc;
}

int xmm6260_ipc_open(void *data, unsigned int size, void *io_data)
{
    int type = *((int *) data);
    int fd = -1;

    switch(type)
    {
        case IPC_CLIENT_TYPE_FMT:
            fd = open("/dev/umts_ipc0", O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        case IPC_CLIENT_TYPE_RFS:
            fd = open("/dev/umts_rfs0", O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        default:
            break;
    }

    if(fd < 0)
        return -1;

    if(io_data == NULL)
        return -1;

    memcpy(io_data, &fd, sizeof(int));

    return 0;
}

int xmm6260_ipc_close(void *data, unsigned int size, void *io_data)
{
    int fd = -1;

    if(io_data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    close(fd);

    return 0;
}

int xmm6260_ipc_read(void *data, unsigned int size, void *io_data)
{
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    if(data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    rc = read_select(fd, 100);
    if (rc < 0)
        return -1;

    rc = read(fd, data, size);
    if(rc < 0)
        return -1;

    return rc;
}

int xmm6260_ipc_write(void *data, unsigned int size, void *io_data)
{
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    rc = write(fd, data, size);

    if(rc < 0)
        return -1;

    return rc;
}

int xmm6260_ipc_power_on(void *io_data)
{
    return 0;
}

int xmm6260_ipc_power_off(void *io_data)
{
    return 0;
}

void *xmm6260_ipc_common_data_create(void)
{
    void *io_data;
    int io_data_len;

    io_data_len = sizeof(int);
    io_data = malloc(io_data_len);

    if(io_data == NULL)
        return NULL;

    memset(io_data, 0, io_data_len);

    return io_data;
}

int xmm6260_ipc_common_data_destroy(void *io_data)
{
    // This was already done, not an error but we need to return
    if(io_data == NULL)
        return 0;

    free(io_data);

    return 0;
}

int xmm6260_ipc_common_data_set_fd(void *io_data, int fd)
{
    int *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (int *) io_data;
    // won't work
    common_data = &fd;

    return 0;
}

int xmm6260_ipc_common_data_get_fd(void *io_data)
{
    int *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (int *) io_data;

    return (int) *(common_data);
}

struct ipc_handlers xmm6260_default_handlers = {
    .read = xmm6260_ipc_read,
    .write = xmm6260_ipc_write,
    .open = xmm6260_ipc_open,
    .close = xmm6260_ipc_close,
    .power_on = xmm6260_ipc_power_on,
    .power_off = xmm6260_ipc_power_off,
    .common_data = NULL,
    .common_data_create = xmm6260_ipc_common_data_create,
    .common_data_destroy = xmm6260_ipc_common_data_destroy,
    .common_data_set_fd = xmm6260_ipc_common_data_set_fd,
    .common_data_get_fd = xmm6260_ipc_common_data_get_fd,
};

struct ipc_ops xmm6260_i9100_fmt_ops = {
    .send = xmm6260_ipc_send,
    .recv = xmm6260_ipc_recv,
    .bootstrap = i9100_modem_bootstrap,
};

struct ipc_ops xmm6260_i9250_fmt_ops = {
    .send = xmm6260_ipc_send,
    .recv = xmm6260_ipc_recv,
    .bootstrap = i9250_modem_bootstrap,
};

struct ipc_ops xmm6260_rfs_ops = {
    .send = xmm6260_rfs_send,
    .recv = xmm6260_rfs_recv,
    .bootstrap = NULL,
};

// vim:ts=4:sw=4:expandtab
