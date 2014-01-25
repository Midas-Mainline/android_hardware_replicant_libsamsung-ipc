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

#ifndef __GALAXYS2_IPC_H__
#define __GALAXYS2_IPC_H__

#define GALAXYS2_MODEM_IMAGE_SIZE                               0x1000000
#define GALAXYS2_PSI_OFFSET                                     0
#define GALAXYS2_PSI_SIZE                                       0xF000
#define GALAXYS2_EBL_OFFSET                                     0xF000
#define GALAXYS2_EBL_SIZE                                       0x19000
#define GALAXYS2_SEC_START_OFFSET                               0x9FF800
#define GALAXYS2_SEC_START_SIZE                                 0x800
#define GALAXYS2_FIRMWARE_OFFSET                                0x28000
#define GALAXYS2_FIRMWARE_SIZE                                  0x9D8000
#define GALAXYS2_NV_DATA_OFFSET                                 0xA00000
#define GALAXYS2_NV_DATA_SIZE                                   0x200000

#define GALAXYS2_MODEM_IMAGE_DEVICE             "/dev/block/mmcblk0p8"

struct galaxys2_ipc_transport_data {
    int fd;
};

extern struct ipc_ops galaxys2_ipc_fmt_ops;
extern struct ipc_ops galaxys2_ipc_rfs_ops;
extern struct ipc_handlers galaxys2_ipc_handlers;
extern struct ipc_gprs_specs galaxys2_ipc_gprs_specs;

#endif

// vim:ts=4:sw=4:expandtab
