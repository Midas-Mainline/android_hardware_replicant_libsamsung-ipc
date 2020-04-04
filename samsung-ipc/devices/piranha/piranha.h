/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __PIRANHA_H__
#define __PIRANHA_H__

#define PIRANHA_MODEM_IMAGE_SIZE			0x1400000
#define PIRANHA_PSI_OFFSET				0x1000
#define PIRANHA_PSI_SIZE				0xE000
#define PIRANHA_EBL_OFFSET				0xF000
#define PIRANHA_EBL_SIZE				0x19000
#define PIRANHA_SEC_START_OFFSET			0x9FF800
#define PIRANHA_SEC_START_SIZE				0x800
#define PIRANHA_FIRMWARE_OFFSET			0x28000
#define PIRANHA_FIRMWARE_SIZE				0x9D7800
#define PIRANHA_NV_DATA_OFFSET				0xA00000
#define PIRANHA_NV_DATA_SIZE				0x200000

#define PIRANHA_MODEM_IMAGE_DEVICE			"/dev/block/mmcblk0p8"

struct piranha_transport_data {
	int fd;
};

extern struct ipc_client_ops piranha_fmt_ops;
extern struct ipc_client_ops piranha_rfs_ops;
extern struct ipc_client_handlers piranha_handlers;
extern struct ipc_client_gprs_specs piranha_gprs_specs;
extern struct ipc_client_nv_data_specs piranha_nv_data_specs;
extern struct ipc_client_nv_data_specs piranha_nv_data_specs;

#endif /* __PIRANHA_H__ */
