/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __IPC_DEVICES_H__
#define __IPC_DEVICES_H__

#include <radio.h>

struct ipc_device_desc {
    char *name;
    char *board_name;

    struct ipc_handlers *handlers;
    struct ipc_ops *fmt_ops;
    struct ipc_ops *rfs_ops;
};

extern struct ipc_device_desc ipc_devices[];
extern int ipc_devices_count;

// xmm6260

extern struct ipc_handlers xmm6260_default_handlers;
extern struct ipc_ops xmm6260_i9100_fmt_ops;
extern struct ipc_ops xmm6260_i9250_fmt_ops;
extern struct ipc_ops xmm6260_rfs_ops;


// h1

extern struct ipc_handlers h1_default_handlers;
extern struct ipc_ops h1_fmt_ops;
extern struct ipc_ops h1_rfs_ops;

// crespo

extern struct ipc_handlers crespo_default_handlers;
extern struct ipc_ops crespo_fmt_ops;
extern struct ipc_ops crespo_rfs_ops;

// aries
extern struct ipc_handlers aries_default_handlers;
extern struct ipc_ops aries_fmt_ops;
extern struct ipc_ops aries_rfs_ops;

#endif

// vim:ts=4:sw=4:expandtab
