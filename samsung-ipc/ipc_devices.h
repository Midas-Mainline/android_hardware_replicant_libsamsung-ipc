/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __IPC_DEVICES_H__
#define __IPC_DEVICES_H__

#include <radio.h>

struct ipc_device_desc {
    char *name;
    char *board_name;
    char *kernel_version;

    struct ipc_ops *fmt_ops;
    struct ipc_ops *rfs_ops;
    struct ipc_handlers *handlers;
    struct ipc_gprs_specs *gprs_specs;
    struct ipc_nv_data_specs *nv_data_specs;
};

extern struct ipc_device_desc ipc_devices[];
extern int ipc_devices_count;

/* h1 */
extern struct ipc_ops h1_fmt_ops;
extern struct ipc_ops h1_rfs_ops;
extern struct ipc_handlers h1_default_handlers;

/* crespo */
extern struct ipc_ops crespo_fmt_ops;
extern struct ipc_ops crespo_rfs_ops;
extern struct ipc_handlers crespo_default_handlers;
extern struct ipc_gprs_specs crespo_gprs_specs;

/* aries */
extern struct ipc_ops aries_fmt_ops;
extern struct ipc_ops aries_rfs_ops;
extern struct ipc_handlers aries_default_handlers;
extern struct ipc_gprs_specs aries_gprs_specs;

/* galaxys2 */
extern struct ipc_ops galaxys2_fmt_ops;
extern struct ipc_ops galaxys2_fmt_ops;
extern struct ipc_ops galaxys2_rfs_ops;
extern struct ipc_handlers galaxys2_default_handlers;
extern struct ipc_gprs_specs galaxys2_gprs_specs;

/* maguro */
extern struct ipc_ops maguro_fmt_ops;
extern struct ipc_ops maguro_rfs_ops;
extern struct ipc_handlers maguro_default_handlers;
extern struct ipc_gprs_specs maguro_gprs_specs;
extern struct ipc_nv_data_specs maguro_nv_data_specs;

#endif

// vim:ts=4:sw=4:expandtab
