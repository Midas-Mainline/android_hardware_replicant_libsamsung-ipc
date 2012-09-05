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

#ifndef __XMM6260_IPC_H__
#define __XMM6260_IPC_H__

#define IPC_MAX_XFER        4096
#define GPRS_IFACE_PREFIX   "rmnet"
#define GPRS_IFACE_COUNT    3

struct rfs_hdr {
        uint32_t size;
        uint8_t cmd;
        uint8_t id;
} __attribute__ ((packed));

int xmm6260_ipc_fmt_client_send(struct ipc_client *client, struct ipc_message_info *request);
int xmm6260_ipc_fmt_client_recv(struct ipc_client *client, struct ipc_message_info *response);
int xmm6260_ipc_rfs_client_recv(struct ipc_client *client, struct ipc_message_info *response);
int xmm6260_ipc_rfs_client_send(struct ipc_client *client, struct ipc_message_info *request);
int xmm6260_ipc_open(int type, void *io_data);
int xmm6260_ipc_close(void *io_data);
int xmm6260_ipc_read(void *data, unsigned int size, void *io_data);
int xmm6260_ipc_write(void *data, unsigned int size, void *io_data);
int xmm6260_ipc_power_on(void *io_data);
int xmm6260_ipc_power_off(void *io_data);
char* xmm6260_ipc_gprs_get_iface(int cid);
int xmm6260_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *cap);
void *xmm6260_ipc_common_data_create(void);
int xmm6260_ipc_common_data_destroy(void *io_data);
int xmm6260_ipc_common_data_set_fd(void *io_data, int fd);
int xmm6260_ipc_common_data_get_fd(void *io_data);

#endif

// vim:ts=4:sw=4:expandtab
