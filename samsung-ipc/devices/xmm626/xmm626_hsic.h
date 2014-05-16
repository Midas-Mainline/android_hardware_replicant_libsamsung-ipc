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

#ifndef __XMM626_HSIC_H__
#define __XMM626_HSIC_H__

#define XMM626_HSIC_BOOT0_ACK                                  0xF0
#define XMM626_HSIC_PSI_UNKNOWN_COUNT                          22
#define XMM626_HSIC_PSI_CRC_ACK                                0x01
#define XMM626_HSIC_PSI_CRC_ACK_COUNT                          2
#define XMM626_HSIC_PSI_ACK                                    0xAA00
#define XMM626_HSIC_EBL_SIZE_ACK                               0xCCCC
#define XMM626_HSIC_EBL_ACK                                    0xA551
#define XMM626_HSIC_EBL_CHUNK                                  0x4000
#define XMM626_HSIC_PORT_CONFIG_SIZE                           0x4C
#define XMM626_HSIC_SET_PORT_CONFIG_SIZE                       0x800
#define XMM626_HSIC_SEC_START_SIZE                             0x4000
#define XMM626_HSIC_SEC_END_SIZE                               0x4000
#define XMM626_HSIC_HW_RESET_SIZE                              0x4000
#define XMM626_HSIC_FLASH_SET_ADDRESS_SIZE                     0x4000
#define XMM626_HSIC_FLASH_WRITE_BLOCK_SIZE                     0x4000
#define XMM626_HSIC_MODEM_DATA_CHUNK                           0x4000

struct xmm626_hsic_psi_header {
    unsigned char magic;
    unsigned short length;
    unsigned char padding;
} __attribute__((packed));

struct xmm626_hsic_command_header {
    unsigned short checksum;
    unsigned short code;
    unsigned int data_size;
} __attribute__((packed));

int xmm626_hsic_psi_send(struct ipc_client *client, int device_fd,
    const void *psi_data, unsigned short psi_size);
int xmm626_hsic_ebl_send(struct ipc_client *client, int device_fd,
    const void *ebl_data, size_t ebl_size);

int xmm626_hsic_port_config_send(struct ipc_client *client, int device_fd);
int xmm626_hsic_sec_start_send(struct ipc_client *client, int device_fd,
    const void *sec_data, size_t sec_size);
int xmm626_hsic_sec_end_send(struct ipc_client *client, int device_fd);
int xmm626_hsic_firmware_send(struct ipc_client *client, int device_fd,
    const void *firmware_data, size_t firmware_size);
int xmm626_hsic_nv_data_send(struct ipc_client *client, int device_fd);
int xmm626_hsic_hw_reset_send(struct ipc_client *client, int device_fd);

#endif

// vim:ts=4:sw=4:expandtab
