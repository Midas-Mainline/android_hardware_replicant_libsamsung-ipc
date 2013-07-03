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
 *
 */

#ifndef __MAGURO_IPC_H__
#define __MAGURO_IPC_H__

#define MAGURO_MODEM_IMAGE_SIZE                                 0x1000000
#define MAGURO_PSI_OFFSET                                       0
#define MAGURO_PSI_SIZE                                         0xF000
#define MAGURO_EBL_OFFSET                                       0xF000
#define MAGURO_EBL_SIZE                                         0x19000
#define MAGURO_SEC_START_OFFSET                                 0x9FF800
#define MAGURO_SEC_START_SIZE                                   0x800
#define MAGURO_FIRMWARE_OFFSET                                  0x28000
#define MAGURO_FIRMWARE_SIZE                                    0x9D8000
#define MAGURO_NV_DATA_OFFSET                                   0xA00000
#define MAGURO_NV_DATA_SIZE                                     0x200000
#define MAGURO_MPS_DATA_SIZE                                    3

#define MAGURO_MODEM_IMAGE_DEVICE               "/dev/block/platform/omap/omap_hsmmc.0/by-name/radio"
#define MAGURO_MPS_DATA_DEVICE                  "/factory/imei/mps_code.dat"

#define MAGURO_NV_DATA_PATH                     "/factory/nv_data.bin"
#define MAGURO_NV_DATA_MD5_PATH                 "/factory/nv_data.bin.md5"
#define MAGURO_NV_STATE_PATH                    "/factory/.nv_state"
#define MAGURO_NV_DATA_BAK_PATH                 "/factory/.nv_data.bak"
#define MAGURO_NV_DATA_MD5_BAK_PATH             "/factory/.nv_data.bak.md5"

struct maguro_ipc_transport_data {
    int fd;
};

extern struct ipc_ops maguro_ipc_fmt_ops;
extern struct ipc_ops maguro_ipc_rfs_ops;
extern struct ipc_handlers maguro_ipc_handlers;
extern struct ipc_gprs_specs maguro_ipc_gprs_specs;
extern struct ipc_nv_data_specs maguro_ipc_nv_data_specs;

#endif

// vim:ts=4:sw=4:expandtab
