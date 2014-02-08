/*
 * This file is part of libsamsung-ipc.
 *
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

#ifndef __XMM6260_H__
#define __XMM6260_H__

#define XMM6260_AT                              "ATAT"
#define XMM6260_PSI_PADDING                                     0xFF
#define XMM6260_PSI_MAGIC                                       0x30
#define XMM6260_SEC_END_MAGIC                                   0x0000
#define XMM6260_HW_RESET_MAGIC                                  0x111001
#define XMM6260_DATA_SIZE                                       0x1000

#define XMM6260_COMMAND_SET_PORT_CONFIG                         0x86
#define XMM6260_COMMAND_SEC_START                               0x204
#define XMM6260_COMMAND_SEC_END                                 0x205
#define XMM6260_COMMAND_HW_RESET                                0x208
#define XMM6260_COMMAND_FLASH_SET_ADDRESS                       0x802
#define XMM6260_COMMAND_FLASH_WRITE_BLOCK                       0x804

#define XMM6260_FIRMWARE_ADDRESS                                0x60300000
#define XMM6260_NV_DATA_ADDRESS                                 0x60E80000
#define XMM6260_MPS_DATA_ADDRESS                                0x61080000

unsigned char xmm6260_crc_calculate(void *buffer, int length);

#endif

// vim:ts=4:sw=4:expandtab
