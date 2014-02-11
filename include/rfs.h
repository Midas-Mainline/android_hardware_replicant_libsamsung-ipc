/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
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

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_RFS_H__
#define __SAMSUNG_IPC_RFS_H__

/*
 * Commands
 */

#define IPC_RFS_NV_READ_ITEM                                    0x4201
#define IPC_RFS_NV_WRITE_ITEM                                   0x4202

/*
 * Structures
 */

struct ipc_rfs_nv_read_item_data {
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_read_item_confirm_header {
    unsigned char confirm;
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_write_item_header {
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_write_item_confirm_data {
    unsigned char confirm;
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

/*
 * Helpers
 */

void md5hash2string(char *out, unsigned char *in);
void nv_data_generate(struct ipc_client *client);
void nv_data_md5_compute(void *data_p, int size, char *secret, void *hash);
void nv_data_md5_generate(struct ipc_client *client);
void nv_data_backup_create(struct ipc_client *client);
void nv_data_backup_restore(struct ipc_client *client);
int nv_data_check(struct ipc_client *client);
int nv_data_md5_check(struct ipc_client *client);
int nv_data_read(struct ipc_client *client, int offset, int length, char *buf);
int nv_data_write(struct ipc_client *client, int offset, int length, char *buf);

void ipc_rfs_send_io_confirm_for_nv_write_item(struct ipc_client *client,
    struct ipc_message_info *info);
void ipc_rfs_send_io_confirm_for_nv_read_item(struct ipc_client *client,
    struct ipc_message_info *info);

#endif

// vim:ts=4:sw=4:expandtab
