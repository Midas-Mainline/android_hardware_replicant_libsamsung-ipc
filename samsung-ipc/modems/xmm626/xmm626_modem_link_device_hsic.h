/*
 * Copyright (C) 2010 Google, Inc.
 * Copyright (C) 2010 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MODEM_LINK_DEVICE_USB_H__
#define __MODEM_LINK_DEVICE_USB_H__


enum {
	IF_USB_BOOT_EP = 0,
	IF_USB_FMT_EP = 0,
	IF_USB_RAW_EP,
	IF_USB_RFS_EP,
	IF_USB_CMD_EP,
	IF_USB_DEVNUM_MAX,
};

/* each pipe has 2 ep for in/out */
#define LINKPM_DEV_NUM			(IF_USB_DEVNUM_MAX * 2)
/*******************/
/* xmm626 specific */

#define IOCTL_LINK_CONTROL_ENABLE	_IO('o', 0x30)
#define IOCTL_LINK_CONTROL_ACTIVE	_IO('o', 0x31)
#define IOCTL_LINK_GET_HOSTWAKE	_IO('o', 0x32)
#define IOCTL_LINK_CONNECTED		_IO('o', 0x33)
#define IOCTL_LINK_SET_BIAS_CLEAR	_IO('o', 0x34)

/* VID,PID for IMC - XMM626, XMM6262*/
#define IMC_BOOT_VID			0x058b
#define IMC_BOOT_PID			0x0041
#define IMC_MAIN_VID			0x1519
#define IMC_MAIN_PID			0x0020
/* VID,PID for STE - M7400 */
#define STE_BOOT_VID			0x04cc
#define STE_BOOT_PID			0x7400
#define STE_MAIN_VID			0x04cc
#define STE_MAIN_PID			0x2333

enum {
	BOOT_DOWN = 0,
	IPC_CHANNEL
};

enum ch_state {
	STATE_SUSPENDED,
	STATE_RESUMED,
};

#define HOSTWAKE_TRIGLEVEL		0

#endif /* __MODEM_LINK_DEVICE_USB_H__ */
