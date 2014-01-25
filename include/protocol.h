/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

#ifndef __SAMSUNG_IPC_PROTOCOL_H__
#define __SAMSUNG_IPC_PROTOCOL_H__

/*
 * Commands
 */

/* AP -> Baseband */
#define IPC_TYPE_EXEC                                           0x01
#define IPC_TYPE_GET                                            0x02
#define IPC_TYPE_SET                                            0x03
#define IPC_TYPE_CFRM                                           0x04
#define IPC_TYPE_EVENT                                          0x05

/* Baseband -> AP */
#define IPC_TYPE_INDI                                           0x01
#define IPC_TYPE_RESP                                           0x02
#define IPC_TYPE_NOTI                                           0x03

/*
 * Groups
 */

#define IPC_GROUP_PWR                                           0x01
#define IPC_GROUP_CALL                                          0x02
#define IPC_GROUP_SMS                                           0x04
#define IPC_GROUP_SEC                                           0x05
#define IPC_GROUP_PB                                            0x06
#define IPC_GROUP_DISP                                          0x07
#define IPC_GROUP_NET                                           0x08
#define IPC_GROUP_SND                                           0x09
#define IPC_GROUP_MISC                                          0x0A
#define IPC_GROUP_SVC                                           0x0B
#define IPC_GROUP_SS                                            0x0C
#define IPC_GROUP_GPRS                                          0x0D
#define IPC_GROUP_SAT                                           0x0E
#define IPC_GROUP_CFG                                           0x0F
#define IPC_GROUP_IMEI                                          0x10
#define IPC_GROUP_GPS                                           0x11
#define IPC_GROUP_SAP                                           0x12
#define IPC_GROUP_RFS                                           0x42
#define IPC_GROUP_GEN                                           0x80

/*
 * Macros
 */

#define IPC_COMMAND(f)                          ((f->group << 8) | f->index)
#define IPC_GROUP(m)                            (m >> 8)
#define IPC_INDEX(m)                            (m & 0xff)

/*
 * Structures
 */

struct ipc_fmt_header {
    unsigned short length;
    unsigned char mseq;
    unsigned char aseq;
    unsigned char group;
    unsigned char index;
    unsigned char type;
} __attribute__((__packed__));

struct ipc_rfs_header {
    unsigned int length;
    unsigned char index;
    unsigned char id;
} __attribute__((__packed__));

#endif

// vim:ts=4:sw=4:expandtab
