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

#ifndef __GENERIC_H__
#define __GENERIC_H__

#define GENERIC_MODEM_IMAGE_SIZE		0x1000000
#define GENERIC_PSI_OFFSET			0x0001000
#define GENERIC_PSI_SIZE			0x000E000
#define GENERIC_EBL_OFFSET			0x000F000
#define GENERIC_EBL_SIZE			0x0019000
#define GENERIC_SEC_START_OFFSET		0x09FF800
#define GENERIC_SEC_START_SIZE			0x0000800
#define GENERIC_FIRMWARE_OFFSET		0x0028000
#define GENERIC_FIRMWARE_SIZE			0x09D7800
#define GENERIC_NV_DATA_OFFSET			0x0A00000
#define GENERIC_NV_DATA_SIZE			0x0200000

#define XMM626_KERNEL_LINUX_BOOT0_DEVICE			"/dev/xmm6262_boot0"
#define XMM626_KERNEL_LINUX_BOOT1_DEVICE	"/dev/umts_boot1"
#define XMM626_KERNEL_LINUX_IPC0_DEVICE	"/dev/umts_ipc"
#define XMM626_KERNEL_LINUX_RFS0_DEVICE	"/dev/umts_rfs"
#define XMM626_KERNEL_LINUX_LINK_PM_DEVICE	"/dev/link_pm"
#define XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS	"/sys/devices/platform/soc/12580000.ehci/ehci_power"
#define XMM626_KERNEL_LINUX_HOSTWAKE_PATH	"/sys/devices/platform/xmm6262/hostwake"
#define XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH	"/sys/devices/platform/xmm6262/link_active"
#define XMM626_KERNEL_LINUX_POWER_PATH		"/sys/devices/platform/xmm6262/modem_power"
#define XMM626_KERNEL_LINUX_PDA_ACTIVE_SYSFS	"/sys/devices/platform/xmm6262/pda_active"
#define XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS	"/sys/devices/platform/xmm6262/slavewake"
//#define XMM626_KERNEL_LINUX_OHCI_POWER_SYSFS	"/sys/devices/platform/s5p-ohci/ohci_power"


struct generic_transport_data {
    int fd;
};

extern struct ipc_client_ops generic_fmt_ops;
extern struct ipc_client_ops generic_rfs_ops;
extern struct ipc_client_handlers generic_handlers;
extern struct ipc_client_gprs_specs generic_gprs_specs;
extern struct ipc_client_nv_data_specs generic_nv_data_specs;

int xmm626_kernel_linux_modem_power(__attribute__((unused)) int device_fd, int power);
int xmm626_kernel_linux_modem_hci_power(struct ipc_client *client, int power);
int xmm626_kernel_linux_modem_link_connected_wait(__attribute__((unused)) int device_fd);
int xmm626_kernel_linux_modem_open(struct ipc_client *client, int type);
int xmm626_kernel_linux_modem_read(int fd, void *buffer, size_t length);
int xmm626_kernel_linux_modem_write(int fd, const void *buffer, size_t length);
int xmm626_kernel_linux_modem_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities);

#endif /* __GENERIC_H__ */

// vim:ts=4:sw=4:expandtab
