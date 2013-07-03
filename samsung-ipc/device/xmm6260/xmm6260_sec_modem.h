/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef __XMM6260_SEC_MODEM_H__
#define __XMM6260_SEC_MODEM_H__

#define XMM6260_SEC_MODEM_BOOT0_DEVICE          "/dev/umts_boot0"
#define XMM6260_SEC_MODEM_BOOT1_DEVICE          "/dev/umts_boot1"
#define XMM6260_SEC_MODEM_IPC0_DEVICE           "/dev/umts_ipc0"
#define XMM6260_SEC_MODEM_RFS0_DEVICE           "/dev/umts_rfs0"
#define XMM6260_SEC_MODEM_LINK_PM_DEVICE        "/dev/link_pm"
#define XMM6260_SEC_MODEM_EHCI_POWER_SYSFS      "/sys/devices/platform/s5p-ehci/ehci_power"
#define XMM6260_SEC_MODEM_OHCI_POWER_SYSFS      "/sys/devices/platform/s5p-ehci/ohci_power"

#define XMM6260_SEC_MODEM_GPRS_IFACE_PREFIX     "rmnet"
#define XMM6260_SEC_MODEM_GPRS_IFACE_COUNT                      3

int xmm6260_sec_modem_power(int device_fd, int power);
int xmm6260_sec_modem_boot_power(int device_fd, int power);
int xmm6260_sec_modem_status_online_wait(int device_fd);
int xmm6260_sec_modem_hci_power(int power);
int xmm6260_sec_modem_link_control_enable(int device_fd, int enable);
int xmm6260_sec_modem_link_control_active(int device_fd, int active);
int xmm6260_sec_modem_link_connected_wait(int device_fd);
int xmm6260_sec_modem_link_get_hostwake_wait(int device_fd);

int xmm6260_sec_modem_ipc_fmt_send(struct ipc_client *client, struct ipc_message_info *request);
int xmm6260_sec_modem_ipc_fmt_recv(struct ipc_client *client, struct ipc_message_info *response);
int xmm6260_sec_modem_ipc_rfs_send(struct ipc_client *client, struct ipc_message_info *request);
int xmm6260_sec_modem_ipc_rfs_recv(struct ipc_client *client, struct ipc_message_info *response);

int xmm6260_sec_modem_ipc_open(int type);
int xmm6260_sec_modem_ipc_close(int fd);
int xmm6260_sec_modem_ipc_read(int fd, void *buffer, unsigned int length);
int xmm6260_sec_modem_ipc_write(int fd, void *buffer, unsigned int length);
int xmm6260_sec_modem_ipc_poll(int fd, struct timeval *timeout);

char *xmm6260_sec_modem_ipc_gprs_get_iface(int cid);
int xmm6260_sec_modem_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities);

#endif

// vim:ts=4:sw=4:expandtab
