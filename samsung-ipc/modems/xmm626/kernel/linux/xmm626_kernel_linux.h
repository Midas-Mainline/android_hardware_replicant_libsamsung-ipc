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

#ifndef __XMM626_KERNEL_LINUX_H__
#define __XMM626_KERNEL_LINUX_H__

#define XMM626_KERNEL_LINUX_BOOT0_DEVICE	"/dev/xmm6262_boot0"
#define XMM626_KERNEL_LINUX_BOOT1_DEVICE	"/dev/umts_boot1"
#define XMM626_KERNEL_LINUX_IPC0_DEVICE		"/dev/umts_ipc"
#define XMM626_KERNEL_LINUX_RFS0_DEVICE		"/dev/umts_rfs"
#define XMM626_KERNEL_LINUX_LINK_PM_DEVICE	"/dev/link_pm"
#define XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS	"/sys/devices/platform/soc/12580000.ehci/ehci_power"
#define XMM626_KERNEL_LINUX_HOSTWAKE_PATH	"/sys/devices/platform/xmm6262/hostwake"
#define XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH	"/sys/devices/platform/xmm6262/link_active"
#define XMM626_KERNEL_LINUX_POWER_PATH		"/sys/devices/platform/xmm6262/modem_power"
#define XMM626_KERNEL_LINUX_PDA_ACTIVE_SYSFS	"/sys/devices/platform/xmm6262/pda_active"
#define XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS	"/sys/devices/platform/xmm6262/slavewake"
//#define XMM626_KERNEL_LINUX_OHCI_POWER_SYSFS	"/sys/devices/platform/s5p-ohci/ohci_power"

int xmm626_kernel_linux_modem_power(__attribute__((unused)) int device_fd, int power);
int xmm626_kernel_linux_modem_boot_power(int device_fd, int power);
int xmm626_kernel_linux_modem_hci_power(int power);
int xmm626_kernel_linux_modem_link_connected_wait(__attribute__((unused)) int device_fd);
int xmm626_kernel_linux_modem_open(int type);
int xmm626_kernel_linux_modem_read(int fd, void *buffer, size_t length);
int xmm626_kernel_linux_modem_write(int fd, const void *buffer, size_t length);
int xmm626_kernel_linux_modem_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities);

#endif /* __XMM626_KERNEL_LINUX_H__ */

// vim:ts=4:sw=4:expandtab
