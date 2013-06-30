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

#ifndef __CRESPO_IPC_H__
#define __CRESPO_IPC_H__

#define MODEM_IMAGE_SIZE                                        0xD80000
#define NV_DATA_OFFSET                                          0xD80000
#define MODEM_DATA_SIZE                                         0x50000

#define MODEM_IMAGE_DEVICE                      "/dev/mtd/mtd5ro"
#define MODEM_SERIAL_DEVICE                     "/dev/s3c2410_serial3"
#define MODEM_CTL_DEVICE                        "/dev/modem_ctl"
#define MODEM_FMT_DEVICE                        "/dev/modem_fmt"
#define MODEM_RFS_DEVICE                        "/dev/modem_rfs"

#define GPRS_IFACE_PREFIX                       "rmnet"
#define GPRS_IFACE_COUNT                                        3

struct crespo_ipc_transport_data {
    int fd;
};

#endif

// vim:ts=4:sw=4:expandtab
