/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdlib.h>

#include <samsung-ipc.h>

#include "ipc_devices.h"

struct ipc_device_desc ipc_devices[] = {
    {
        .name = "crespo",
        .board_name = "herring",
        .kernel_version = "2.6.35",
        .fmt_ops = &crespo_ipc_fmt_ops,
        .rfs_ops = &crespo_ipc_rfs_ops,
        .handlers = &crespo_ipc_handlers,
        .gprs_specs = &crespo_ipc_gprs_specs_single,
        .nv_data_specs = NULL,
    },
    {
        .name = "crespo",
        .board_name = "herring",
        .kernel_version = "3.0",
        .fmt_ops = &crespo_ipc_fmt_ops,
        .rfs_ops = &crespo_ipc_rfs_ops,
        .handlers = &crespo_ipc_handlers,
        .gprs_specs = &crespo_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "aries",
        .board_name = "aries",
        .kernel_version = NULL,
        .fmt_ops = &aries_ipc_fmt_ops,
        .rfs_ops = &aries_ipc_rfs_ops,
        .handlers = &aries_ipc_handlers,
        .gprs_specs = &aries_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "aries",
        .board_name = "gt-p1000",
        .kernel_version = NULL,
        .fmt_ops = &aries_ipc_fmt_ops,
        .rfs_ops = &aries_ipc_rfs_ops,
        .handlers = &aries_ipc_handlers,
        .gprs_specs = &aries_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "galaxys2",
        .board_name = "smdk4210",
        .kernel_version = NULL,
        .fmt_ops = &galaxys2_ipc_fmt_ops,
        .rfs_ops = &galaxys2_ipc_rfs_ops,
        .handlers = &galaxys2_ipc_handlers,
        .gprs_specs = &galaxys2_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "galaxys2",
        .board_name = "i9100",
        .kernel_version = NULL,
        .fmt_ops = &galaxys2_ipc_fmt_ops,
        .rfs_ops = &galaxys2_ipc_rfs_ops,
        .handlers = &galaxys2_ipc_handlers,
        .gprs_specs = &galaxys2_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "maguro",
        .board_name = "tuna",
        .kernel_version = NULL,
        .fmt_ops = &maguro_ipc_fmt_ops,
        .rfs_ops = &maguro_ipc_rfs_ops,
        .handlers = &maguro_ipc_handlers,
        .gprs_specs = &maguro_ipc_gprs_specs,
        .nv_data_specs = &maguro_ipc_nv_data_specs,
    },
    {
        .name = "piranha",
        .board_name = "espresso10",
        .kernel_version = NULL,
        .fmt_ops = &piranha_ipc_fmt_ops,
        .rfs_ops = &piranha_ipc_rfs_ops,
        .handlers = &piranha_ipc_handlers,
        .gprs_specs = &piranha_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "piranha",
        .board_name = "espresso",
        .kernel_version = NULL,
        .fmt_ops = &piranha_ipc_fmt_ops,
        .rfs_ops = &piranha_ipc_rfs_ops,
        .handlers = &piranha_ipc_handlers,
        .gprs_specs = &piranha_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "i9300",
        .board_name = "smdk4x12",
        .kernel_version = NULL,
        .fmt_ops = &i9300_ipc_fmt_ops,
        .rfs_ops = &i9300_ipc_rfs_ops,
        .handlers = &i9300_ipc_handlers,
        .gprs_specs = &i9300_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
    {
        .name = "n7100",
        .board_name = "smdk4x12",
        .kernel_version = NULL,
        .fmt_ops = &n7100_ipc_fmt_ops,
        .rfs_ops = &n7100_ipc_rfs_ops,
        .handlers = &n7100_ipc_handlers,
        .gprs_specs = &n7100_ipc_gprs_specs,
        .nv_data_specs = NULL,
    },
};

int ipc_devices_count = sizeof(ipc_devices) / sizeof(struct ipc_device_desc);

// vim:ts=4:sw=4:expandtab
