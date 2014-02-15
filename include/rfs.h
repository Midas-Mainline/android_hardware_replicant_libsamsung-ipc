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

struct ipc_rfs_nv_read_item_request_data {
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_read_item_response_header {
    unsigned char confirm;
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_write_item_request_header {
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_nv_write_item_response_data {
    unsigned char confirm;
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

/*
 * Helpers
 */

char *ipc_nv_data_md5_calculate(const char *path, const char *secret,
    size_t size, size_t chunk_size);
int ipc_nv_data_path_check(struct ipc_client *client);
int ipc_nv_data_md5_path_check(struct ipc_client *client);
int ipc_nv_data_backup_path_check(struct ipc_client *client);
int ipc_nv_data_backup_md5_path_check(struct ipc_client *client);
int ipc_nv_data_check(struct ipc_client *client);
int ipc_nv_data_backup_check(struct ipc_client *client);
int ipc_nv_data_backup(struct ipc_client *client);
int ipc_nv_data_restore(struct ipc_client *client);
void *ipc_nv_data_load(struct ipc_client *client);
void *ipc_nv_data_read(struct ipc_client *client, size_t size, size_t offset);
int ipc_nv_data_write(struct ipc_client *client, const void *data, size_t size,
    size_t offset);
void *ipc_rfs_nv_read_item_response_setup(const void *data, size_t size,
    size_t offset);

#endif

// vim:ts=4:sw=4:expandtab
