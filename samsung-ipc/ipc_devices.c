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

#include <radio.h>

#include "ipc_devices.h"

    struct ipc_gprs_specs *gprs_specs;
    struct ipc_nv_data_specs *nv_data_specs;

struct ipc_device_desc ipc_devices[] = {
    {
        .name = "crespo",
        .board_name = "herring",
        .fmt_ops = &crespo_fmt_ops,
        .rfs_ops = &crespo_rfs_ops,
        .handlers = &crespo_default_handlers,
        .gprs_specs = &crespo_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "aries",
        .board_name = "aries",
        .fmt_ops = &aries_fmt_ops,
        .rfs_ops = &aries_rfs_ops,
        .handlers = &aries_default_handlers,
        .gprs_specs = &aries_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "aries",
        .board_name = "gt-p1000",
        .fmt_ops = &aries_fmt_ops,
        .rfs_ops = &aries_rfs_ops,
        .handlers = &aries_default_handlers,
        .gprs_specs = &aries_gprs_specs,
        .nv_data_specs = NULL,
    },
    {   
        .name = "galaxys2",
        .board_name = "i9100",
        .fmt_ops = &xmm6260_i9100_fmt_ops,
        .rfs_ops = &xmm6260_rfs_ops,
        .handlers = &xmm6260_default_handlers,
        .gprs_specs = &xmm6260_gprs_specs,
        .nv_data_specs = NULL,
    },
    {   
        .name = "galaxys2",
        .board_name = "smdk4210",
        .fmt_ops = &xmm6260_i9100_fmt_ops,
        .rfs_ops = &xmm6260_rfs_ops,
        .handlers = &xmm6260_default_handlers,
        .gprs_specs = &xmm6260_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "maguro",
        .board_name = "tuna",
        .fmt_ops = &xmm6260_i9250_fmt_ops,
        .rfs_ops = &xmm6260_rfs_ops,
        .handlers = &xmm6260_default_handlers,
        .gprs_specs = &xmm6260_gprs_specs,
        .nv_data_specs = &xmm6260_nv_data_specs,
    }
};

int ipc_devices_count = sizeof(ipc_devices) / sizeof(struct ipc_device_desc);

// vim:ts=4:sw=4:expandtab
