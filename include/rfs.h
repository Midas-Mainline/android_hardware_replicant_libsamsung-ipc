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
 * Values
 */

#define NV_DATA_SIZE_DEFAULT                                    0x200000
#define NV_DATA_CHUNK_SIZE_DEFAULT                              0x1000

#define NV_DATA_PATH_DEFAULT                    "/efs/nv_data.bin"
#define NV_DATA_MD5_PATH_DEFAULT                "/efs/nv_data.bin.md5"
#define NV_DATA_BAK_PATH_DEFAULT                "/efs/.nv_data.bak"
#define NV_DATA_MD5_BAK_PATH_DEFAULT            "/efs/.nv_data.bak.md5"
#define NV_STATE_PATH_DEFAULT                   "/efs/.nv_state"
#define NV_DATA_SECRET_DEFAULT                  "Samsung_Android_RIL"

#define MD5_STRING_SIZE                         MD5_DIGEST_LENGTH * 2 + 1

/*
 * Structures
 */

struct ipc_rfs_io {
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

struct ipc_rfs_io_confirm {
    unsigned char confirm;
    unsigned int offset;
    unsigned int length;
} __attribute__((__packed__));

/*
 * Helpers
 */

void md5hash2string(char *out, unsigned char *in);
char *nv_data_path(struct ipc_client *client);
char *nv_data_md5_path(struct ipc_client *client);
char *nv_data_bak_path(struct ipc_client *client);
char *nv_data_md5_bak_path(struct ipc_client *client);
char *nv_state_path(struct ipc_client *client);
char *nv_data_secret(struct ipc_client *client);
int nv_data_size(struct ipc_client *client);
int nv_data_chunk_size(struct ipc_client *client);
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
