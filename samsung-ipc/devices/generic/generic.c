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
#define _GNU_SOURCE         /* See feature_test_macros(7) */

#define GENERIC_DEBUG 1

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include <samsung-ipc.h>
#include <ipc.h>

#include "modems/xmm626/xmm626.h"
#include "modems/xmm626/xmm626_hsic.h"
#include "modems/xmm626/xmm626_kernel_smdk4412.h"
#include "partitions/android/android.h"

#include "generic.h"

int xmm626_kernel_linux_modem_power(__attribute__((unused)) int device_fd,
				    int power)
{
	int rc;

//	ipc_client_log(client, "ENTER %s", __func__);

	rc = sysfs_value_write(XMM626_KERNEL_LINUX_POWER_PATH, !!power);
	if (rc == -1) {
//		ipc_client_log(client, "%s: sysfs_value_write failed with error -1",
//			       __func__);
		return rc;
	}

//	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int xmm626_kernel_linux_modem_hci_power(struct ipc_client *client, int power)
{
	int ehci_rc, ohci_rc = -1;

	ipc_client_log(client, "ENTER %s", __func__);

	if (!!power) {
		ohci_rc = sysfs_value_write(
			XMM626_KERNEL_LINUX_PDA_ACTIVE_SYSFS, 1);
		if (sysfs_value_read(XMM626_KERNEL_LINUX_HOSTWAKE_PATH)) {
			ohci_rc |= sysfs_value_write(
				XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS, 0);
			usleep(10000);
			ohci_rc |= sysfs_value_write(
				XMM626_KERNEL_LINUX_SLAVEWAKE_SYSFS, 1);
		}
		ehci_rc = sysfs_value_write(
			XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS, !!power);
		if (ehci_rc >= 0)
			usleep(50000);

		ohci_rc |= sysfs_value_write(
			XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, 1);
	} else {
		ehci_rc = sysfs_value_write(
			XMM626_KERNEL_LINUX_EHCI_POWER_SYSFS, !!power);
		if (ehci_rc >= 0)
			usleep(50000);

		ohci_rc = sysfs_value_write(
			XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, 0);
	}

	if (ohci_rc < 0) {
		ipc_client_log(client, "%s: ohci_rc < 0", __func__);
	}

	if (ehci_rc < 0 && ohci_rc < 0) {
		ipc_client_log(client, "%s: error: ehci_rc < 0 && ohci_rc < 0",
			       __func__);

		return -1;
	}

	ipc_client_log(client, "%s: DONE", __func__);
	return 0;
}

int xmm626_kernel_linux_modem_link_control_enable(
	__attribute__((unused)) int device_fd, int enable)
{
//	ipc_client_log(client, "ENTER %s: dummy function", __func__);

	return 0;
}

int xmm626_kernel_linux_modem_link_control_active(
	__attribute__((unused)) int device_fd, int active)
{
	int rc;

//	ipc_client_log(client, "ENTER %s", __func__);

	rc = sysfs_value_write(XMM626_KERNEL_LINUX_LINK_ACTIVE_PATH, !!active);
	if (rc < 0) {
//		ipc_client_log(client, "%s: sysfs_value_write failed with error -1",
//			       __func__);
		return -1;
	}

//	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int xmm626_kernel_linux_modem_link_connected_wait(
	__attribute__((unused)) int device_fd)
{
	int i;

//	ipc_client_log(client, "ENTER %s", __func__);

	i = 0;
	for (i = 0; i < 10; i++) {

		usleep(50000);
	}

//	ipc_client_log(client, "EXIT %s", __func__);

	return 0;
}

int xmm626_kernel_linux_modem_link_get_hostwake_wait(
	__attribute__((unused)) int device_fd)
{
	int i;

//	ipc_client_log(client, "ENTER %s", __func__);
	
	i = 0;
	for (i = 0; i < 10; i++) {
		/* TODO: read host wake GPIOs */
		usleep(500000);
	}

//	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int xmm626_kernel_linux_modem_open(struct ipc_client *client, int type)
{
	int fd = -2;
	int i = 0;
	int err = 0;

	ipc_client_log(client, "ENTER %s", __func__);
	while (fd < 0 && i < 30) {
		i++;
		usleep(30000);
		ipc_client_log(client, "%s: client type: %s", __func__,
			       ipc_client_type_string(type));
		switch (type) {
		case IPC_CLIENT_TYPE_FMT:
			err = 0;
			fd = open(XMM626_KERNEL_LINUX_IPC0_DEVICE,
				  O_RDWR | O_NOCTTY | O_NONBLOCK);
			if (fd < 0) {
				err = errno;
				ipc_client_log(client, "%s: open(%s, "
				       "O_RDWR | O_NOCTTY | O_NONBLOCK) => "
				       "%d errno: %d",
				       __func__,
				       XMM626_KERNEL_LINUX_IPC0_DEVICE, fd,
				       err);
			}

			break;
		case IPC_CLIENT_TYPE_RFS:
			err = 0;
			fd = open(XMM626_KERNEL_LINUX_RFS0_DEVICE,
				  O_RDWR | O_NOCTTY | O_NONBLOCK);
			if (fd < 0) {
				err  = errno;
				ipc_client_log(client, "%s: open(%s, "
				       "O_RDWR | O_NOCTTY | O_NONBLOCK) => "
				       "%d errno: %d",
				       __func__,
				       XMM626_KERNEL_LINUX_RFS0_DEVICE,
				       fd, err);
			}
			break;
		default:
			ipc_client_log(client, "%s: unknown client type %s",
				       __func__, ipc_client_type_string(type));
			return -1;
		}
	}

	ipc_client_log(client, "EXIT %s", __func__);

	return fd;
}

int xmm626_kernel_linux_modem_read(int fd, void *buffer, size_t length)
{
	int rc;

//	ipc_client_log(client, "ENTER %s", __func__);

	if (fd < 0 || buffer == NULL || length <= 0)
		return -1;

	rc = read(fd, buffer, length);

//	ipc_client_log(client, "%s DONE", __func__);

	return rc;
}

int xmm626_kernel_linux_modem_write(int fd, const void *buffer, size_t length)
{
	int rc;

//	ipc_client_log(client, "ENTER %s", __func__);

	if (fd < 0 || buffer == NULL || length <= 0) {
//		ipc_client_log(client, "%s: error: fd < 0 || buffer == NULL || length <= 0",
//			       __func__);

		return -1;
	}

	rc = write(fd, buffer, length);

//	ipc_client_log(client, "%s DONE", __func__);

	return rc;
}

char *xmm626_kernel_linux_modem_gprs_get_iface(unsigned int cid)
{
	char *iface = NULL;

//	ipc_client_log(client, "ENTER %s", __func__);

	if (cid > XMM626_SEC_MODEM_GPRS_IFACE_COUNT) {
//		ipc_client_log(client,
//			       "%s: error: cid > XMM626_SEC_MODEM_GPRS_IFACE_COUNT",
//			       __func__);
		return NULL;
	}

	asprintf(&iface, "%s%d", XMM626_SEC_MODEM_GPRS_IFACE_PREFIX, cid - 1);

//	ipc_client_log(client, "%s DONE", __func__);

	return iface;
}

int generic_gprs_get_capabilities(
	__attribute__((unused)) struct ipc_client *client,
	struct ipc_client_gprs_capabilities *capabilities)
{
	ipc_client_log(client, "ENTER %s", __func__);

	if (capabilities == NULL) {
		ipc_client_log(client, "%s: error: capabilities == NULL",
			       __func__);
		return -1;
	}

	capabilities->cid_count = XMM626_SEC_MODEM_GPRS_IFACE_COUNT;

	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int generic_boot(struct ipc_client *client)
{
	void *modem_image_data = NULL;
	int modem_image_fd = -1;
	int modem_boot_fd = -1;
	int modem_link_fd = -1;
	unsigned char *p;
	char *path;
	int rc;
	int unused = 0;

	ipc_client_log(client, "ENTER %s", __func__);

	if (client == NULL)
		return -1;

	ipc_client_log(client, "Starting generic modem boot");

	system("lsusb");

	modem_image_fd = open_android_modem_partition_by_name(client, "RADIO",
							      &path);
	if (modem_image_fd == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: open_android_modem_partition_by_name: "
			       "failed to open %s with error %d: %s",
			       __func__, path, rc, strerror(rc));
		goto error;
	}

	ipc_client_log(client,
		       "%s: Opened the %s partition", __func__, "RADIO");

	modem_image_data = mmap(0, GENERIC_MODEM_IMAGE_SIZE, PROT_READ,
				MAP_SHARED, modem_image_fd, 0);
	if (modem_image_data == NULL ||
	    modem_image_data == (void *) 0xffffffff) {
		ipc_client_log(client,
			       "Mapping modem image data to memory failed");
		goto error;
	}
	ipc_client_log(client, "Mapped modem image data to memory");

	rc = xmm626_kernel_linux_modem_power(unused, 0);
	if (rc < 0) {
		ipc_client_log(client,
			       "xmm626_kernel_linux_modem_power: "
			       "Turning the modem off failed with error %d",
			       rc);
		goto error;
	}

	rc = xmm626_kernel_linux_modem_hci_power(client, 0);
	if (rc < 0) {
		ipc_client_log(client,
			       "xmm626_kernel_linux_modem_hci_power: "
			       "Turning the modem off failed");
		goto error;
	}

	ipc_client_log(client, "Turned the modem off");
	system("lsusb");

	rc = xmm626_kernel_linux_modem_power(unused, 1);
	ipc_client_log(client,
		       "%s: xmm626_kernel_linux_modem_power(NULL, 1) = %d",
	       __func__, rc);
	if (rc < 0) {
		ipc_client_log(client,
			       "xmm626_kernel_linux_modem_power "
			       "failed with error %d",
			       rc);
	}

	rc = xmm626_kernel_linux_modem_hci_power(client, 1);
	ipc_client_log(client,
		       "%s: xmm626_kernel_linux_modem_hci_power(client, 1) = %d",
		       __func__, rc);
	if (rc < 0) {
		ipc_client_log(client,
			       "xmm626_kernel_linux_modem_hci_power on failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem on");

	system("lsusb");

	rc = 0;
	do {
		modem_boot_fd = open(XMM626_KERNEL_LINUX_BOOT0_DEVICE,
				     O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (modem_boot_fd >= 0) {
			break;
		}
		usleep(5000);
		rc++;
	} while (rc < 10000);
	if (modem_boot_fd < 0) {
		ipc_client_log(client, "Failed to open boot device");
		goto error;
	}
	ipc_client_log(client, "Opened modem boot device");

	p = (unsigned char *) modem_image_data + GENERIC_PSI_OFFSET;

	rc = xmm626_hsic_psi_send(client, modem_boot_fd, (void *) p,
				  GENERIC_PSI_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC PSI failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC PSI");

	p = (unsigned char *) modem_image_data + GENERIC_EBL_OFFSET;

	rc = xmm626_hsic_ebl_send(client, modem_boot_fd, (void *) p,
				  GENERIC_EBL_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC EBL failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC EBL");

	rc = xmm626_hsic_port_config_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client,
			       "Sending XMM626 HSIC port config failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC port config");

	p = (unsigned char *) modem_image_data + GENERIC_SEC_START_OFFSET;

	rc = xmm626_hsic_sec_start_send(client, modem_boot_fd, (void *) p,
					GENERIC_SEC_START_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC SEC start failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC SEC start");

	p = (unsigned char *) modem_image_data + GENERIC_FIRMWARE_OFFSET;

	rc = xmm626_hsic_firmware_send(client, modem_boot_fd, (void *) p,
				       GENERIC_FIRMWARE_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC firmware failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC firmware");

	rc = xmm626_hsic_nv_data_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC nv_data failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC nv_data");

	rc = xmm626_hsic_sec_end_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC SEC end failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC SEC end");

	rc = xmm626_hsic_hw_reset_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC HW reset failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC HW reset");

	usleep(300000);

	rc = xmm626_kernel_linux_modem_link_get_hostwake_wait(modem_link_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for host wake failed");
	}
	ipc_client_log(client, "Waited for host wake");

	rc = xmm626_kernel_linux_modem_hci_power(client, 0);

	if (rc < 0) {
		ipc_client_log(client, "Turning the modem off failed");
		goto error;
	}
	ipc_client_log(client, "Turned off the modem");

	rc = xmm626_kernel_linux_modem_link_get_hostwake_wait(modem_link_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for host wake failed");
	}
	ipc_client_log(client, "Waited for host wake");

	rc = xmm626_kernel_linux_modem_hci_power(client, 1);
	if (rc < 0) {
		ipc_client_log(client, "Turning the modem on failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem on");
	system("lsusb");
	ipc_client_log(client, "Wait for the modem to come up again",
		       __func__);
	sleep(10);

	system("lsusb");

	ipc_client_log(client, "%s complete", __func__);
	sleep(7);

	rc = 0;
	goto complete;

error:
	ipc_client_log(client,
		       "+-------------------------+\n"
		       "| /!\\ generic_boot failed |\n"
		       "+-------------------------+\n");
	rc = -1;

complete:
	if (modem_image_data != NULL)
		munmap(modem_image_data, GENERIC_MODEM_IMAGE_SIZE);

	if (modem_image_fd >= 0)
		close(modem_image_fd);

	if (modem_boot_fd >= 0)
		close(modem_boot_fd);

	if (modem_link_fd >= 0)
		close(modem_link_fd);

	if (rc != -1)
		ipc_client_log(client, "%s DONE", __func__);

	return rc;
}

int generic_open(struct ipc_client *client, void *data, int type)
{
	struct generic_transport_data *transport_data;

	ipc_client_log(client, "ENTER %s", __func__);
	if (data == NULL) {
		ipc_client_log(client, "%s: error: data == NULL",
			       __func__);
		return -1;
	}

	transport_data = (struct generic_transport_data *) data;

	transport_data->fd = xmm626_kernel_linux_modem_open(client, type);
	if (transport_data->fd < 0) {
		ipc_client_log(client, "%s: transport_data->fd < 0",
			       __func__);
		return -1;
	}

	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int generic_close(__attribute__((unused)) struct ipc_client *client, void *data)
{
	struct generic_transport_data *transport_data;

	ipc_client_log(client, "ENTER %s", __func__);

	if (data == NULL){
		ipc_client_log(client, "%s: error: data == NULL",
			       __func__);

		return -1;
	}

	transport_data = (struct generic_transport_data *) data;

	xmm626_kernel_smdk4412_close(client, transport_data->fd);
	transport_data->fd = -1;

	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

int generic_read(__attribute__((unused)) struct ipc_client *client,
		 void *data, void *buffer, size_t length)
{
	struct generic_transport_data *transport_data;
	int rc;

	ipc_client_log(client, "ENTER %s", __func__);

	if (data == NULL) {
		ipc_client_log(client, "%s: error: data == NULL",
			       __func__);

		return -1;
	}

	transport_data = (struct generic_transport_data *) data;

	rc = xmm626_kernel_linux_modem_read(transport_data->fd, buffer, length);

	ipc_client_log(client, "%s DONE", __func__);

	return rc;
}

int generic_write(__attribute__((unused)) struct ipc_client *client,
		  void *data, const void *buffer, size_t length)
{
	struct generic_transport_data *transport_data;
	int rc;

	ipc_client_log(client, "ENTER %s", __func__);

	if (data == NULL) {
		ipc_client_log(client, "%s: error: data == NULL", __func__);
		return -1;
	}

	transport_data = (struct generic_transport_data *) data;

	rc = xmm626_kernel_linux_modem_write(transport_data->fd,
					     buffer, length);

	ipc_client_log(client, "%s DONE", __func__);
	return rc;
}

int generic_poll(__attribute__((unused)) struct ipc_client *client,
		 void *data, __attribute__((unused)) struct ipc_poll_fds *fds,
		 __attribute__((unused)) struct timeval *timeout)
{
	struct generic_transport_data *transport_data;
	int rc;
	struct pollfd fd;

#if GENERIC_DEBUG
	ipc_client_log(client, "ENTER %s", __func__);
#endif
	if (data == NULL)
		return -1;

	transport_data = (struct generic_transport_data *) data;

	fd.fd = transport_data->fd;
	fd.events = POLLRDNORM | POLLIN;

//#if GENERIC_DEBUG
//	ipc_client_log(client, "%s: transport_data->fd: %d", __func__, transport_data->fd);
//#endif
	rc = poll(&fd, 1, -1);
	if (rc == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: poll failed with error %d: %s", __func__,
			       rc, strerror(rc));
		return -1;
	}

#if GENERIC_DEBUG
	ipc_client_log(client, "%s: poll: %d", __func__, rc);
#endif

	return 0;
}


int generic_smdk_poll(__attribute__((unused)) struct ipc_client *client, void *data,
		      struct ipc_poll_fds *fds, struct timeval *timeout)
{
	struct generic_transport_data *transport_data;
	int rc;

	ipc_client_log(client, "ENTER %s", __func__);

	if (data == NULL)
		return -1;

	transport_data = (struct generic_transport_data *) data;

	rc = xmm626_kernel_smdk4412_poll(client, transport_data->fd, fds,
					 timeout);

	ipc_client_log(client, "%s DONE: poll: %d", __func__, rc);

	return rc;
}

int generic_power_on(__attribute__((unused)) struct ipc_client *client,
		     __attribute__((unused)) void *data)
{
	ipc_client_log(client, "ENTER %s: dummy function", __func__);

	return 0;
}

int generic_power_off(__attribute__((unused)) struct ipc_client *client,
		      __attribute__((unused)) void *data)
{
	int fd;
	int rc;

	ipc_client_log(client, "ENTER %s", __func__);

	fd = open(XMM626_KERNEL_LINUX_BOOT0_DEVICE,
		  O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
		return -1;

	rc = xmm626_kernel_linux_modem_power(fd, 0);

	close(fd);

	if (rc < 0) {
		ipc_client_log(client, "%s: xmm626_kernel_linux_modem_power failed with error %d",
			       __func__, rc);
		return -1;
	}

	ipc_client_log(client, "%s DONE", __func__);
	return 0;
}

int generic_gprs_activate(__attribute__((unused)) struct ipc_client *client,
			  __attribute__((unused)) void *data,
			  __attribute__((unused)) unsigned int cid)
{
	ipc_client_log(client, "ENTER %s: dummy function", __func__);
	
	return 0;
}

int generic_gprs_deactivate(__attribute__((unused)) struct ipc_client *client,
			    __attribute__((unused)) void *data,
			    __attribute__((unused)) unsigned int cid)
{
	ipc_client_log(client, "ENTER %s: dummy function", __func__);

	return 0;
}

int generic_data_create(__attribute__((unused)) struct ipc_client *client,
			void **transport_data,
			__attribute__((unused)) void **power_data,
			__attribute__((unused)) void **gprs_data)
{
	ipc_client_log(client, "ENTER %s", __func__);

	if (transport_data == NULL) {
		ipc_client_log(client, "%s: error: transport_data == NULL",
			       __func__);
		return -1;
	}

	*transport_data = calloc(1, sizeof(struct generic_transport_data));

	ipc_client_log(client, "%s DONE", __func__);
	return 0;
}

int generic_data_destroy(__attribute__((unused)) struct ipc_client *client,
			 void *transport_data,
			 __attribute__((unused)) void *power_data,
			 __attribute__((unused)) void *gprs_data)
{
	ipc_client_log(client, "ENTER %s", __func__);

	if (transport_data == NULL) {
		ipc_client_log(client, "%s: error: transport_data == NULL",
			       __func__);
		return -1;
	}

	free(transport_data);

	ipc_client_log(client, "%s DONE", __func__);

	return 0;
}

struct ipc_client_ops generic_fmt_ops = {
	.boot = generic_boot,
	.send = xmm626_kernel_smdk4412_fmt_send,
	.recv = xmm626_kernel_smdk4412_fmt_recv,
};

struct ipc_client_ops generic_rfs_ops = {
	.boot = NULL,
	.send = xmm626_kernel_smdk4412_rfs_send,
	.recv = xmm626_kernel_smdk4412_rfs_recv,
};

struct ipc_client_handlers generic_handlers = {
	.read = generic_read,
	.write = generic_write,
	.open = generic_open,
	.close = generic_close,
	.poll = generic_poll,
	.transport_data = NULL,
	.power_on = generic_power_on,
	.power_off = generic_power_off,
	.power_data = NULL,
	.gprs_activate = generic_gprs_activate,
	.gprs_deactivate = generic_gprs_deactivate,
	.gprs_data = NULL,
	.data_create = generic_data_create,
	.data_destroy = generic_data_destroy,
};

struct ipc_client_gprs_specs generic_gprs_specs = {
	.gprs_get_iface = xmm626_kernel_smdk4412_gprs_get_iface,
	.gprs_get_capabilities = generic_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs generic_nv_data_specs = {
	.nv_data_path = XMM626_NV_DATA_PATH,
	.nv_data_md5_path = XMM626_NV_DATA_MD5_PATH,
	.nv_data_backup_path = XMM626_NV_DATA_BACKUP_PATH,
	.nv_data_backup_md5_path = XMM626_NV_DATA_BACKUP_MD5_PATH,
	.nv_data_secret = XMM626_NV_DATA_SECRET,
	.nv_data_size = XMM626_NV_DATA_SIZE,
	.nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};

// vim:ts=4:sw=4:expandtab
