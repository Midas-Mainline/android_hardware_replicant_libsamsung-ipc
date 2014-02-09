/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
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

#ifndef __SAMSUNG_IPC_MISC_H__
#define __SAMSUNG_IPC_MISC_H__

/*
 * Commands
 */

#define IPC_MISC_ME_VERSION                                     0x0A01
#define IPC_MISC_ME_IMSI                                        0x0A02
#define IPC_MISC_ME_SN                                          0x0A03
#define IPC_MISC_TIME_INFO                                      0x0A05
#define IPC_MISC_DEBUG_LEVEL                                    0x0A0C

/*
 * Values
 */

#define IPC_MISC_ME_SN_SERIAL_NUM                               0x01
#define IPC_MISC_ME_SN_SERIAL_NUM_SERIAL                        0x04
#define IPC_MISC_ME_SN_SERIAL_NUM_MANUFACTURE_DATE              0x05
#define IPC_MISC_ME_SN_SERIAL_NUM_BARCODE                       0x06

/*
 * Structures
 */

struct ipc_misc_me_version_data {
    unsigned char unknown;
    char software_version[32];
    char hardware_version[32];
    char cal_date[32];
    char misc[32];
} __attribute__((__packed__));

struct ipc_misc_me_imsi_header {
    unsigned char length;
} __attribute__((__packed__));

struct ipc_misc_me_sn_data {
    unsigned char type; // IPC_MISC_ME_SN_SERIAL_NUM
    unsigned char length;
    char data[32];
} __attribute__((__packed__));

struct ipc_misc_time_info_data {
    unsigned char tz_valid, daylight_valid;
    unsigned char year, mon, day;
    unsigned char hour, min, sec;
    signed char tz, dl, dv;
    char plmn[6];
} __attribute__((__packed__));

/*
 * Helpers
 */

char *ipc_misc_me_imsi_get_imsi(const void *data, size_t size);

#endif

// vim:ts=4:sw=4:expandtab
