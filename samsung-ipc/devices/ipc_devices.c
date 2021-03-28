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

#include "devices/ipc_devices.h"

struct ipc_device_desc ipc_devices[] = {
	{
		.name = "i9300",
		.board_name = "smdk4x12",
		.kernel_version = NULL,
		.fmt_ops = &i9300_fmt_ops,
		.rfs_ops = &i9300_rfs_ops,
		.handlers = &i9300_handlers,
		.gprs_specs = &i9300_gprs_specs,
		.nv_data_specs = &i9300_nv_data_specs,
	},
	{
		.name = "n7100",
		.board_name = "smdk4x12",
		.kernel_version = NULL,
		.fmt_ops = &n7100_fmt_ops,
		.rfs_ops = &n7100_rfs_ops,
		.handlers = &n7100_handlers,
		.gprs_specs = &n7100_gprs_specs,
		.nv_data_specs = &n7100_nv_data_specs,
	},
	{
		.name = "n5100",
		.board_name = "smdk4x12",
		.kernel_version = NULL,
		.fmt_ops = &n5100_fmt_ops,
		.rfs_ops = &n5100_rfs_ops,
		.handlers = &n5100_handlers,
		.gprs_specs = &n5100_gprs_specs,
		.nv_data_specs = &n5100_nv_data_specs,
	},
	{
		.name = "herolte",
		.board_name = NULL,
		.kernel_version = NULL,
		.fmt_ops = &herolte_fmt_ops,
		.rfs_ops = &herolte_rfs_ops,
		.handlers = &herolte_handlers,
		.gprs_specs = &herolte_gprs_specs,
		.nv_data_specs = &herolte_nv_data_specs,
	},
	{
		.name = NULL,
		.board_name = "samsung exynos (flattened device tree)",
		.kernel_version = NULL,
		.fmt_ops = &generic_fmt_ops,
		.rfs_ops = &generic_rfs_ops,
		.handlers = &generic_handlers,
		.gprs_specs = &generic_gprs_specs,
		.nv_data_specs = &generic_nv_data_specs,
	},
};

unsigned int ipc_devices_count = sizeof(ipc_devices) /
	sizeof(struct ipc_device_desc);
