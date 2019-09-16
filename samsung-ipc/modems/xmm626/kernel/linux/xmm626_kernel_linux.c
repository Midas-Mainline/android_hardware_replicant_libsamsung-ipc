/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
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

#define _GNU_SOURCE
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <samsung-ipc.h>
#include <ipc.h>

#include <modem.h>
#include <modem_prj.h>
#include <modem_link_device_hsic.h>

#include <xmm626.h>
#include <xmm626_kernel_linux.h>
#include <xmm626_sec_modem.h>

int xmm626_kernel_linux_modem_power(__attribute__((unused)) int device_fd, int power)
{
    int rc;

    rc = sysfs_value_write(XMM626_KERNEL_LINUX_POWER_PATH, !!power);
    if (rc < 0)
        return -99;

    return 0;
}

int xmm626_kernel_linux_modem_boot_power(int device_fd, int power)
{
    int rc;

    if (device_fd < 0)
        return -1;

    rc = sysfs_value_write(XMM626_KERNEL_LINUX_POWER_PATH, !!power);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_kernel_linux_modem_hci_power(int power)
{
    int ehci_rc, ohci_rc = -1;

    
    /*ohci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_OHCI_POWER_SYSFS, !!power);
    if (ohci_rc >= 0)
        usleep(50000);
*/

    if (!!power) {
	ohci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_PDA_ACTIVE_SYSFS, 1);
	if (sysfs_value_read(XMM626_KERNEL_LINUX_HOSTWAKE_PATH)) {
		ohci_rc |= sysfs_value_write(XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS, 0);
		usleep(10000);
		ohci_rc |= sysfs_value_write(XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS, 1);
	}
	ehci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS, !!power);
	if (ehci_rc >= 0)
		usleep(50000);

	ohci_rc |= sysfs_value_write(XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, 1);
    } else {
	    ehci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS, !!power);
	if (ehci_rc >= 0)
		usleep(50000);

	//ohci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_PDA_ACTIVE_SYSFS, 0);
	ohci_rc = sysfs_value_write(XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, 0);
    }


    if (ohci_rc < 0) {
	printf("ohci_rc < 0\n");
    }
    if (ehci_rc < 0 && ohci_rc < 0)
        return -1;

    return 0;
}

int xmm626_kernel_linux_modem_link_control_enable(__attribute__((unused)) int device_fd,
					 int enable)
{
	if (enable) {
	}
    return 0;
}

int xmm626_kernel_linux_modem_link_control_active(__attribute__((unused)) int device_fd,
					 int active)
{
    int rc;

    rc = sysfs_value_write(XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, !!active);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm626_kernel_linux_modem_link_connected_wait(__attribute__((unused)) int device_fd)
{
    int i;

    i = 0;
    for (i = 0; i < 10; i++) {

        usleep(50000);
    }

    return 0;
}

int xmm626_kernel_linux_modem_link_get_hostwake_wait(
	__attribute__((unused)) int device_fd)
{
    int status;
    int i;

    i = 0;
    for (i = 0; i < 10; i++) {
        /* !gpio_get_value (hostwake) */
//        status = sysfs_value_read(XMM626_KERNEL_LINUX_HOSTWAKE_PATH);
	printf("%s: i=%d read(%s) => status: %d\n", __FUNCTION__, i, XMM626_KERNEL_LINUX_HOSTWAKE_PATH, status);
//        if (status == 0) /* invert: return true when hostwake is low */
 //           return 0;

        usleep(500000);
    }

    return 0;
}

int xmm626_kernel_linux_modem_open(int type)
{
    int fd = -2;
    int i = 0;
    int err = 0;

    printf("ENTER %s\n", __FUNCTION__);
    while (fd < 0 && i < 30) {
	    i++;
	    usleep(30000);
	    printf("%s: type: %d\n", __FUNCTION__, type);
	    switch (type) {
		case IPC_CLIENT_TYPE_FMT:
		    err = 0;
		    fd = open(XMM626_KERNEL_LINUX_IPC0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
		    if (fd < 0) {
		      err = errno;
		      printf("%s: open(%s, O_RDWR | O_NOCTTY | O_NONBLOCK) => %d errno: %d\n",
				      __FUNCTION__, XMM626_KERNEL_LINUX_IPC0_DEVICE, fd, err);
		    }

		    break;
		case IPC_CLIENT_TYPE_RFS:
		    err = 0;
		    fd = open(XMM626_KERNEL_LINUX_RFS0_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
		    if (fd < 0) {
			    err  = errno;
		            printf("%s: open(%s, O_RDWR | O_NOCTTY | O_NONBLOCK) => %d errno: %d\n", 
					    __FUNCTION__, XMM626_KERNEL_LINUX_RFS0_DEVICE, fd, err);
		    }
		    break;
		default:
		    printf("%s: unknown type\n", __FUNCTION__);
		    return -1;
	    }
    }

    return fd;
}

int xmm626_kernel_linux_modem_read(int fd, void *buffer, size_t length)
{
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    rc = read(fd, buffer, length);

    return rc;
}

int xmm626_kernel_linux_modem_write(int fd, const void *buffer, size_t length)
{
    int rc;

    if (fd < 0 || buffer == NULL || length <= 0)
        return -1;

    rc = write(fd, buffer, length);

    return rc;
}

char *xmm626_kernel_linux_modem_gprs_get_iface(unsigned int cid)
{
    char *iface = NULL;

    if (cid > XMM626_SEC_MODEM_GPRS_IFACE_COUNT)
        return NULL;

    asprintf(&iface, "%s%d", XMM626_SEC_MODEM_GPRS_IFACE_PREFIX, cid - 1);

    return iface;
}

int xmm626_kernel_linux_modem_gprs_get_capabilities(struct ipc_client_gprs_capabilities *capabilities)
{
    if (capabilities == NULL)
        return -1;

    capabilities->cid_count = XMM626_SEC_MODEM_GPRS_IFACE_COUNT;

    return 0;
}

// vim:ts=4:sw=4:expandtab
