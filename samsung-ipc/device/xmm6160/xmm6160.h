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
 *
 */

#include <samsung-ipc.h>

#ifndef __XMM6160_H__
#define __XMM6160_H__

#define XMM6160_AT                              "AT"
#define XMM6160_AT_COUNT                                        20
#define XMM6160_PSI_MAGIC                                       0x30
#define XMM6160_PSI_ACK                                         0x01
#define XMM6160_BOOTCORE_VERSION                                0xF0

int xmm6160_psi_send(struct ipc_client *client, int serial_fd,
    void *psi_data, unsigned short psi_size);
int xmm6160_firmware_send(struct ipc_client *client, int device_fd,
    void *device_address, void *firmware_data, int firmware_size);
int xmm6160_nv_data_send(struct ipc_client *client, int device_fd,
    void *device_address);

#endif

// vim:ts=4:sw=4:expandtab
