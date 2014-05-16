/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __XMM626_MIPI_H__
#define __XMM626_MIPI_H__

#define XMM626_MIPI_BOOT0_ACK                                  0xFFFF
#define XMM626_MIPI_BOOT1_MAGIC                                0x02
#define XMM626_MIPI_BOOT1_ACK                                  0xAA00
#define XMM626_MIPI_PSI_ACK                                    0xDD01
#define XMM626_MIPI_EBL_SIZE_ACK                               0xCCCC
#define XMM626_MIPI_EBL_ACK                                    0xA551
#define XMM626_MIPI_EBL_CHUNK                                  0xDFC
#define XMM626_MIPI_MODEM_DATA_CHUNK                           0xDF2
#define XMM626_MIPI_COMMAND_HEADER_MAGIC                       0x02
#define XMM626_MIPI_COMMAND_FOOTER_MAGIC                       0x03
#define XMM626_MIPI_COMMAND_FOOTER_UNKNOWN                     0xEAEA

struct xmm626_mipi_psi_header {
    unsigned char padding;
    unsigned short length;
    unsigned char magic;
} __attribute__((packed));

struct xmm626_mipi_command_header {
    unsigned int size;
    unsigned short magic;
    unsigned short code;
    unsigned short data_size;
} __attribute__((packed));

struct xmm626_mipi_command_footer {
    unsigned short checksum;
    unsigned short magic;
    unsigned short unknown;
} __attribute__((packed));

int xmm626_mipi_psi_send(struct ipc_client *client, int device_fd,
    const void *psi_data, unsigned short psi_size);
int xmm626_mipi_ebl_send(struct ipc_client *client, int device_fd,
    const void *ebl_data, size_t ebl_size);

int xmm626_mipi_port_config_send(struct ipc_client *client, int device_fd);
int xmm626_mipi_sec_start_send(struct ipc_client *client, int device_fd,
    const void *sec_data, size_t sec_size);
int xmm626_mipi_sec_end_send(struct ipc_client *client, int device_fd);
int xmm626_mipi_firmware_send(struct ipc_client *client, int device_fd,
    const void *firmware_data, size_t firmware_size);
int xmm626_mipi_nv_data_send(struct ipc_client *client, int device_fd);
int xmm626_mipi_mps_data_send(struct ipc_client *client, int device_fd,
    const void *mps_data, size_t mps_size);
int xmm626_mipi_hw_reset_send(struct ipc_client *client, int device_fd);

#endif

// vim:ts=4:sw=4:expandtab
