/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef __GALAXYSMTD_IPC_H__
#define __GALAXYSMTD_IPC_H__

//from trace/logcat: nCoreVer = 0xf0
#define BOOTCORE_VERSION        0xf0
//from trace: write(14, "0", 1) before PSI_DATA_LEN
#define PSI_MAGIC               0x30
//from trace/logcat: Bootloader IMG Size: 0x00005000, sent after PSI_MAGIC, LSB first
#define PSI_DATA_LEN            0x5000
//from /radio/modem.bin size (0xc00000) - /efs/nv_data.bin size (0x2...)
#define RADIO_IMG_SIZE          0xc00000-0x200000

//should be the same from crespo
#define MAX_MODEM_DATA_SIZE     0x1000

struct samsung_rfs_msg
{
    uint32_t offset;
    uint32_t size;
};

struct samsung_rfs_cfrm
{
    uint8_t confirmation;
    struct samsung_rfs_msg msg;
};

int wake_lock(char *lock_name, int len);
int wake_unlock(char *lock_name, int len);

extern struct ipc_handlers galaxysmtd_ipc_default_handlers;

#endif

// vim:ts=4:sw=4:expandtab
