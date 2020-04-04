/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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
 */

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_DISP_H__
#define __SAMSUNG_IPC_DISP_H__

/*
 * Commands
 */

#define IPC_DISP_ICON_INFO					0x0701
#define IPC_DISP_HOMEZONE_INFO					0x0702
#define IPC_DISP_RSSI_INFO					0x0706

/*
 * Values
 */


#define IPC_DISP_ICON_INFO_FLAG_RSSI				0x01
#define IPC_DISP_ICON_INFO_FLAG_BATTERY			0x02
#define IPC_DISP_ICON_INFO_FLAG_HDR_RSSI			0x03
#define IPC_DISP_ICON_INFO_FLAG_ALL				0xFF

/*
 * Structures
 */

struct ipc_disp_icon_info_response_data {
	unsigned char flags;		/* IPC_DISP_ICON_INFO_FLAG */
	unsigned char rssi;
	unsigned char hdr_rssi;
	unsigned char battery;
} __attribute__((__packed__));

struct ipc_disp_icon_info_request_data {
	unsigned char flags;		/* IPC_DISP_ICON_INFO_FLAG */
} __attribute__((__packed__));

struct ipc_disp_rssi_info_data {
	unsigned char rssi;
} __attribute__((__packed__));

#endif /* __SAMSUNG_IPC_DISP_H__ */
