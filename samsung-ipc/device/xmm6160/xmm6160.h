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

#define MODEM_AT                                "AT"
#define MODEM_AT_COUNT                                          20
#define MODEM_PSI_SIZE                                          0x5000
#define MODEM_PSI_MAGIC                                         0x30
#define MODEM_PSI_ACK                                           0x01
#define MODEM_BOOTCORE_VERSION                                  0xF0

int xmm6160_psi_send(struct ipc_client *client, int serial_fd,
    void *modem_image_data, int modem_image_size);
int xmm6160_modem_image_send(struct ipc_client *client, int device_fd,
    void *device_address, void *modem_image_data, int modem_image_size,int modem_image_offset);
int xmm6160_nv_data_send(struct ipc_client *client, int device_fd,
    void *device_address, int modem_image_offset);

#endif

// vim:ts=4:sw=4:expandtab
