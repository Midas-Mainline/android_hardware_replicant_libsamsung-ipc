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

#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <samsung-ipc.h>

#include "ipc.h"
#include "devices/n7100/n7100.h"
#include "modems/xmm626/xmm626.h"
#include "modems/xmm626/xmm626_hsic.h"
#include "modems/xmm626/xmm626_kernel_smdk4412.h"

int n7100_boot(struct ipc_client *client)
{
	void *modem_image_data = NULL;
	int modem_image_fd = -1;
	int modem_boot_fd = -1;
	int modem_link_fd = -1;
	unsigned char *p;
	int rc;

	if (client == NULL)
		return -1;

	ipc_client_log(client, "Starting n7100 modem boot");

	modem_image_fd = open(N7100_MODEM_IMAGE_DEVICE, O_RDONLY);
	if (modem_image_fd < 0) {
		ipc_client_log(client, "Opening modem image device failed");
		goto error;
	}
	ipc_client_log(client, "Opened modem image device");

	modem_image_data = mmap(0, N7100_MODEM_IMAGE_SIZE, PROT_READ,
				MAP_SHARED, modem_image_fd, 0);
	if (modem_image_data == NULL ||
	    modem_image_data == (void *) 0xffffffff) {
		ipc_client_log(client,
			       "Mapping modem image data to memory failed");
		goto error;
	}
	ipc_client_log(client, "Mapped modem image data to memory");

	modem_boot_fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE,
			     O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (modem_boot_fd < 0) {
		ipc_client_log(client, "Opening modem boot device failed");
		goto error;
	}
	ipc_client_log(client, "Opened modem boot device");

	modem_link_fd = open(XMM626_SEC_MODEM_LINK_PM_DEVICE, O_RDWR);
	if (modem_link_fd < 0) {
		ipc_client_log(client, "Opening modem link device failed");
		goto error;
	}
	ipc_client_log(client, "Opened modem link device");

	rc = xmm626_kernel_smdk4412_hci_power(client, 0);
	if (rc < 0) {
		ipc_client_log(client, "Turning the modem off failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem off");

	rc = xmm626_kernel_smdk4412_power(client, modem_boot_fd, 1);
	if (rc < 0) {
		ipc_client_log(client, "Powering on the modem failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_hci_power(client, 1);
	if (rc < 0) {
		ipc_client_log(client, "Powering on the HCI bus failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem on");

	rc = xmm626_kernel_smdk4412_link_connected_wait(client, modem_link_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for link connected failed");
		goto error;
	}
	ipc_client_log(client, "Waited for link connected");

	p = (unsigned char *) modem_image_data + N7100_PSI_OFFSET;

	rc = xmm626_hsic_psi_send(client, modem_boot_fd, (void *) p,
				  N7100_PSI_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC PSI failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC PSI");

	p = (unsigned char *) modem_image_data + N7100_EBL_OFFSET;

	rc = xmm626_hsic_ebl_send(client, modem_boot_fd, (void *) p,
				  N7100_EBL_SIZE);
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

	p = (unsigned char *) modem_image_data + N7100_SEC_START_OFFSET;

	rc = xmm626_hsic_sec_start_send(client, modem_boot_fd, (void *) p,
					N7100_SEC_START_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 HSIC SEC start failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 HSIC SEC start");

	p = (unsigned char *) modem_image_data + N7100_FIRMWARE_OFFSET;

	rc = xmm626_hsic_firmware_send(client, modem_boot_fd, (void *) p,
				       N7100_FIRMWARE_SIZE);
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

	rc = xmm626_kernel_smdk4412_link_get_hostwake_wait(client,
							   modem_link_fd);
	if (rc < 0)
		ipc_client_log(client, "Waiting for host wake failed");

	rc = xmm626_kernel_smdk4412_link_control_enable(client, modem_link_fd,
							0);
	if (rc < 0) {
		ipc_client_log(client, "Disabling the modem link failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_hci_power(client, 0);
	if (rc < 0) {
		ipc_client_log(client, "Powering off the HCI bus failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_link_control_active(client, modem_link_fd,
							0);
	if (rc < 0) {
		ipc_client_log(client, "Deactivating the modem link failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_link_get_hostwake_wait(client,
							   modem_link_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for host wake failed");
		goto error;
	}
	ipc_client_log(client, "Waited for host wake");

	rc = xmm626_kernel_smdk4412_link_control_enable(client, modem_link_fd,
							1);
	if (rc < 0) {
		ipc_client_log(client, "Enabling the modem link failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_hci_power(client, 1);
	if (rc < 0) {
		ipc_client_log(client, "Powering on the HCI bus failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_link_control_active(client, modem_link_fd,
							1);
	if (rc < 0) {
		ipc_client_log(client, "Activating the modem link failed");
		goto error;
	}

	rc = xmm626_kernel_smdk4412_link_connected_wait(client, modem_link_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for link connected failed");
		goto error;
	}
	ipc_client_log(client, "Waited for link connected");

	usleep(300000);

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (modem_image_data != NULL)
		munmap(modem_image_data, N7100_MODEM_IMAGE_SIZE);

	if (modem_image_fd >= 0)
		close(modem_image_fd);

	if (modem_boot_fd >= 0)
		close(modem_boot_fd);

	if (modem_link_fd >= 0)
		close(modem_link_fd);

	return rc;
}

int n7100_open(__attribute__((unused)) struct ipc_client *client, void *data,
	       int type)
{
	struct n7100_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct n7100_transport_data *) data;

	transport_data->fd = xmm626_kernel_smdk4412_open(client, type);
	if (transport_data->fd < 0)
		return -1;

	return 0;
}

int n7100_close(__attribute__((unused)) struct ipc_client *client, void *data)
{
	struct n7100_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct n7100_transport_data *) data;

	xmm626_kernel_smdk4412_close(client, transport_data->fd);
	transport_data->fd = -1;

	return 0;
}

int n7100_read(__attribute__((unused)) struct ipc_client *client, void *data,
	       void *buffer, size_t length)
{
	struct n7100_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct n7100_transport_data *) data;

	rc = xmm626_kernel_smdk4412_read(client, transport_data->fd, buffer,
					 length);

	return rc;
}

int n7100_write(__attribute__((unused)) struct ipc_client *client, void *data,
		const void *buffer, size_t length)
{
	struct n7100_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct n7100_transport_data *) data;

	rc = xmm626_kernel_smdk4412_write(client, transport_data->fd, buffer,
					  length);

	return rc;
}

int n7100_poll(__attribute__((unused)) struct ipc_client *client, void *data,
	       struct ipc_poll_fds *fds, struct timeval *timeout)
{
	struct n7100_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct n7100_transport_data *) data;

	rc = xmm626_kernel_smdk4412_poll(client, transport_data->fd, fds,
					 timeout);

	return rc;
}

int n7100_power_on(__attribute__((unused)) struct ipc_client *client,
		   __attribute__((unused)) void *data)
{
	return 0;
}

int n7100_power_off(__attribute__((unused)) struct ipc_client *client,
		    __attribute__((unused)) void *data)
{
	int fd;
	int rc;

	fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY |
		  O_NONBLOCK);
	if (fd < 0)
		return -1;

	rc = xmm626_kernel_smdk4412_power(client, fd, 0);

	close(fd);

	if (rc < 0)
		return -1;

	return 0;
}

int n7100_gprs_activate(__attribute__((unused)) struct ipc_client *client,
			__attribute__((unused)) void *data,
			__attribute__((unused)) unsigned int cid)
{
	return 0;
}

int n7100_gprs_deactivate(__attribute__((unused)) struct ipc_client *client,
			  __attribute__((unused)) void *data,
			  __attribute__((unused)) unsigned int cid)
{
	return 0;
}

int n7100_data_create(__attribute__((unused)) struct ipc_client *client,
		      void **transport_data,
		      __attribute__((unused)) void **power_data,
		      __attribute__((unused)) void **gprs_data)
{
	if (transport_data == NULL)
		return -1;

	*transport_data = calloc(1, sizeof(struct n7100_transport_data));

	return 0;
}

int n7100_data_destroy(__attribute__((unused)) struct ipc_client *client,
		       void *transport_data,
		       __attribute__((unused)) void *power_data,
		       __attribute__((unused)) void *gprs_data)
{
	if (transport_data == NULL)
		return -1;

	free(transport_data);

	return 0;
}

struct ipc_client_ops n7100_fmt_ops = {
	.boot = n7100_boot,
	.send = xmm626_kernel_smdk4412_fmt_send,
	.recv = xmm626_kernel_smdk4412_fmt_recv,
};

struct ipc_client_ops n7100_rfs_ops = {
	.boot = NULL,
	.send = xmm626_kernel_smdk4412_rfs_send,
	.recv = xmm626_kernel_smdk4412_rfs_recv,
};

struct ipc_client_handlers n7100_handlers = {
	.read = n7100_read,
	.write = n7100_write,
	.open = n7100_open,
	.close = n7100_close,
	.poll = n7100_poll,
	.transport_data = NULL,
	.power_on = n7100_power_on,
	.power_off = n7100_power_off,
	.power_data = NULL,
	.gprs_activate = n7100_gprs_activate,
	.gprs_deactivate = n7100_gprs_deactivate,
	.gprs_data = NULL,
	.data_create = n7100_data_create,
	.data_destroy = n7100_data_destroy,
};

struct ipc_client_gprs_specs n7100_gprs_specs = {
	.gprs_get_iface = xmm626_kernel_smdk4412_gprs_get_iface,
	.gprs_get_capabilities = xmm626_kernel_smdk4412_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs n7100_nv_data_specs = {
	.nv_data_path = XMM626_NV_DATA_PATH,
	.nv_data_md5_path = XMM626_NV_DATA_MD5_PATH,
	.nv_data_backup_path = XMM626_NV_DATA_BACKUP_PATH,
	.nv_data_backup_md5_path = XMM626_NV_DATA_BACKUP_MD5_PATH,
	.nv_data_secret = XMM626_NV_DATA_SECRET,
	.nv_data_size = XMM626_NV_DATA_SIZE,
	.nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};
