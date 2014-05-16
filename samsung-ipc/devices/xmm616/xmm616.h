/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __XMM616_H__
#define __XMM616_H__

#define XMM616_AT                              "AT"
#define XMM616_AT_COUNT                                        20
#define XMM616_PSI_MAGIC                                       0x30
#define XMM616_PSI_ACK                                         0x01
#define XMM616_BOOTCORE_VERSION                                0xF0

#define XMM616_NV_DATA_PATH                    "/efs/nv_data.bin"
#define XMM616_NV_DATA_MD5_PATH                "/efs/nv_data.bin.md5"
#define XMM616_NV_DATA_BACKUP_PATH             "/efs/.nv_data.bak"
#define XMM616_NV_DATA_BACKUP_MD5_PATH         "/efs/.nv_data.bak.md5"
#define XMM616_NV_DATA_SECRET                  "Samsung_Android_RIL"
#define XMM616_NV_DATA_SIZE                                    0x200000
#define XMM616_NV_DATA_CHUNK_SIZE                              0x1000

int xmm616_psi_send(struct ipc_client *client, int serial_fd,
    const void *psi_data, unsigned short psi_size);
int xmm616_firmware_send(struct ipc_client *client, int device_fd,
    void *device_address, const void *firmware_data, size_t firmware_size);
int xmm616_nv_data_send(struct ipc_client *client, int device_fd,
    void *device_address);

#endif

// vim:ts=4:sw=4:expandtab
