/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __XMM6260_MIPI_H__
#define __XMM6260_MIPI_H__

#define XMM6260_MIPI_BOOT0_ACK                                  0xFFFF
#define XMM6260_MIPI_BOOT1_MAGIC                                0x02
#define XMM6260_MIPI_BOOT1_ACK                                  0xAA00
#define XMM6260_MIPI_PSI_ACK                                    0xDD01
#define XMM6260_MIPI_EBL_SIZE_ACK                               0xCCCC
#define XMM6260_MIPI_EBL_ACK                                    0xA551
#define XMM6260_MIPI_EBL_CHUNK                                  0xDFC
#define XMM6260_MIPI_MODEM_DATA_CHUNK                           0xDF2
#define XMM6260_MIPI_COMMAND_HEADER_MAGIC                       0x02
#define XMM6260_MIPI_COMMAND_TAIL_MAGIC                         0x03
#define XMM6260_MIPI_COMMAND_TAIL_UNKNOWN                       0xEAEA

struct xmm6260_mipi_psi_header {
    unsigned char padding;
    unsigned short length;
    unsigned char magic;
} __attribute__((packed));

struct xmm6260_mipi_command_header {
    unsigned int size;
    unsigned short magic;
    unsigned short code;
    unsigned short data_size;
} __attribute__((packed));

struct xmm6260_mipi_command_tail {
    unsigned short checksum;
    unsigned short magic;
    unsigned short unknown;
} __attribute__((packed));

int xmm6260_mipi_psi_send(struct ipc_client *client, int device_fd,
    void *psi_data, unsigned short psi_size);
int xmm6260_mipi_ebl_send(struct ipc_client *client, int device_fd,
    void *ebl_data, int ebl_size);

int xmm6260_mipi_port_config_send(struct ipc_client *client, int device_fd);
int xmm6260_mipi_sec_start_send(struct ipc_client *client, int device_fd,
    void *sec_data, int sec_size);
int xmm6260_mipi_sec_end_send(struct ipc_client *client, int device_fd);
int xmm6260_mipi_firmware_send(struct ipc_client *client, int device_fd,
    void *firmware_data, int firmware_size);
int xmm6260_mipi_nv_data_send(struct ipc_client *client, int device_fd);
int xmm6260_mipi_mps_data_send(struct ipc_client *client, int device_fd,
    void *mps_data, int mps_size);
int xmm6260_mipi_hw_reset_send(struct ipc_client *client, int device_fd);

#endif

// vim:ts=4:sw=4:expandtab
