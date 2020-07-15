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

#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <samsung-ipc.h>

#include "ipc.h"
#include "devices/maguro/maguro.h"
#include "modems/xmm626/xmm626.h"
#include "modems/xmm626/xmm626_kernel_smdk4412.h"
#include "modems/xmm626/xmm626_mipi.h"

int maguro_boot(struct ipc_client *client)
{
	void *modem_image_data = NULL;
	int modem_image_fd = -1;
	int modem_boot_fd = -1;
	void *mps_data = NULL;
	unsigned char *p;
	int rc;

	if (client == NULL)
		return -1;

	ipc_client_log(client, "Starting maguro modem boot");

	modem_image_fd = open(MAGURO_MODEM_IMAGE_DEVICE, O_RDONLY);
	if (modem_image_fd < 0) {
		ipc_client_log(client, "Opening modem image device failed");
		goto error;
	}
	ipc_client_log(client, "Opened modem image device");

	modem_image_data = mmap(0, MAGURO_MODEM_IMAGE_SIZE, PROT_READ,
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

	rc = xmm626_kernel_smdk4412_power(client, modem_boot_fd, 0);
	if (rc < 0) {
		ipc_client_log(client, "Turning the modem off failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem off");

	rc = xmm626_kernel_smdk4412_power(client, modem_boot_fd, 1);
	if (rc < 0) {
		ipc_client_log(client, "Turning the modem on failed");
		goto error;
	}
	ipc_client_log(client, "Turned the modem on");

	p = (unsigned char *) modem_image_data + MAGURO_PSI_OFFSET;

	rc = xmm626_mipi_psi_send(client, modem_boot_fd, (void *) p,
				  MAGURO_PSI_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI PSI failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI PSI");

	close(modem_boot_fd);

	modem_boot_fd = open(XMM626_SEC_MODEM_BOOT1_DEVICE,
			     O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (modem_boot_fd < 0) {
		ipc_client_log(client, "Opening modem boot device failed");
		goto error;
	}
	ipc_client_log(client, "Opened modem boot device");

	p = (unsigned char *) modem_image_data + MAGURO_EBL_OFFSET;

	rc = xmm626_mipi_ebl_send(client, modem_boot_fd, (void *) p,
				  MAGURO_EBL_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI EBL failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI EBL");

	rc = xmm626_mipi_port_config_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client,
			       "Sending XMM626 MIPI port config failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI port config");

	p = (unsigned char *) modem_image_data + MAGURO_SEC_START_OFFSET;

	rc = xmm626_mipi_sec_start_send(client, modem_boot_fd, (void *) p,
					MAGURO_SEC_START_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI SEC start failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI SEC start");

	p = (unsigned char *) modem_image_data + MAGURO_FIRMWARE_OFFSET;

	rc = xmm626_mipi_firmware_send(client, modem_boot_fd, (void *) p,
				       MAGURO_FIRMWARE_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI firmware failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI firmware");

	rc = xmm626_mipi_nv_data_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI nv_data failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI nv_data");

	mps_data = file_data_read(client, MAGURO_MPS_DATA_DEVICE,
				  MAGURO_MPS_DATA_SIZE, MAGURO_MPS_DATA_SIZE,
				  0);
	if (mps_data == NULL) {
		ipc_client_log(client, "Reading MPS data failed");
		goto error;
	}
	ipc_client_log(client, "Read MPS data");

	rc = xmm626_mipi_mps_data_send(client, modem_boot_fd, mps_data,
				       MAGURO_MPS_DATA_SIZE);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI MPS data failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI MPS data");

	rc = xmm626_mipi_sec_end_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI SEC end failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI SEC end");

	rc = xmm626_mipi_hw_reset_send(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Sending XMM626 MIPI HW reset failed");
		goto error;
	}
	ipc_client_log(client, "Sent XMM626 MIPI HW reset");

	rc = xmm626_kernel_smdk4412_status_online_wait(client, modem_boot_fd);
	if (rc < 0) {
		ipc_client_log(client, "Waiting for online status failed");
		goto error;
	}
	ipc_client_log(client, "Waited for online status");

	rc = xmm626_kernel_smdk4412_boot_power(client, modem_boot_fd, 0);
	if (rc < 0) {
		ipc_client_log(client, "Turning modem boot off failed");
		goto error;
	}
	ipc_client_log(client, "Turned modem boot off");

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (mps_data != NULL)
		free(mps_data);

	if (modem_image_data != NULL)
		munmap(modem_image_data, MAGURO_MODEM_IMAGE_SIZE);

	if (modem_image_fd >= 0)
		close(modem_image_fd);

	if (modem_boot_fd >= 0)
		close(modem_boot_fd);

	return rc;
}

int maguro_open(__attribute__((unused)) struct ipc_client *client, void *data,
		int type)
{
	struct maguro_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct maguro_transport_data *) data;

	transport_data->fd = xmm626_kernel_smdk4412_open(client, type);
	if (transport_data->fd < 0)
		return -1;

	return 0;
}

int maguro_close(__attribute__((unused)) struct ipc_client *client, void *data)
{
	struct maguro_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct maguro_transport_data *) data;

	xmm626_kernel_smdk4412_close(client, transport_data->fd);
	transport_data->fd = -1;

	return 0;
}

int maguro_read(__attribute__((unused)) struct ipc_client *client, void *data,
		void *buffer, size_t length)
{
	struct maguro_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct maguro_transport_data *) data;

	rc = xmm626_kernel_smdk4412_read(client, transport_data->fd, buffer,
					 length);

	return rc;
}

int maguro_write(__attribute__((unused)) struct ipc_client *client, void *data,
		 const void *buffer, size_t length)
{
	struct maguro_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct maguro_transport_data *) data;

	rc = xmm626_kernel_smdk4412_write(client, transport_data->fd, buffer,
					  length);

	return rc;
}

int maguro_poll(__attribute__((unused)) struct ipc_client *client, void *data,
		struct ipc_poll_fds *fds, struct timeval *timeout)
{
	struct maguro_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct maguro_transport_data *) data;

	rc = xmm626_kernel_smdk4412_poll(client, transport_data->fd, fds,
					 timeout);

	return rc;
}

int maguro_power_on(__attribute__((unused)) struct ipc_client *client,
		    __attribute__((unused)) void *data)
{
	return 0;
}

int maguro_power_off(__attribute__((unused)) struct ipc_client *client,
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

int maguro_gprs_activate(__attribute__((unused)) struct ipc_client *client,
			 __attribute__((unused)) void *data,
			 __attribute__((unused)) unsigned int cid)
{
	return 0;
}

int maguro_gprs_deactivate(__attribute__((unused)) struct ipc_client *client,
			   __attribute__((unused)) void *data,
			   __attribute__((unused)) unsigned int cid)
{
	return 0;
}

int maguro_data_create(__attribute__((unused)) struct ipc_client *client,
		       void **transport_data,
		       __attribute__((unused)) void **power_data,
		       __attribute__((unused)) void **gprs_data)
{
	if (transport_data == NULL)
		return -1;

	*transport_data = calloc(1, sizeof(struct maguro_transport_data));

	return 0;
}

int maguro_data_destroy(__attribute__((unused)) struct ipc_client *client,
			void *transport_data,
			__attribute__((unused)) void *power_data,
			__attribute__((unused)) void *gprs_data)
{
	if (transport_data == NULL)
		return -1;

	free(transport_data);

	return 0;
}

struct ipc_client_ops maguro_fmt_ops = {
	.boot = maguro_boot,
	.send = xmm626_kernel_smdk4412_fmt_send,
	.recv = xmm626_kernel_smdk4412_fmt_recv,
};

struct ipc_client_ops maguro_rfs_ops = {
	.boot = NULL,
	.send = xmm626_kernel_smdk4412_rfs_send,
	.recv = xmm626_kernel_smdk4412_rfs_recv,
};

struct ipc_client_handlers maguro_handlers = {
	.read = maguro_read,
	.write = maguro_write,
	.open = maguro_open,
	.close = maguro_close,
	.poll = maguro_poll,
	.transport_data = NULL,
	.power_on = maguro_power_on,
	.power_off = maguro_power_off,
	.power_data = NULL,
	.gprs_activate = maguro_gprs_activate,
	.gprs_deactivate = maguro_gprs_deactivate,
	.gprs_data = NULL,
	.data_create = maguro_data_create,
	.data_destroy = maguro_data_destroy,
};

struct ipc_client_gprs_specs maguro_gprs_specs = {
	.gprs_get_iface = xmm626_kernel_smdk4412_gprs_get_iface,
	.gprs_get_capabilities = xmm626_kernel_smdk4412_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs maguro_nv_data_specs = {
	.nv_data_path = MAGURO_NV_DATA_PATH,
	.nv_data_md5_path = MAGURO_NV_DATA_MD5_PATH,
	.nv_data_backup_path = MAGURO_NV_DATA_BACKUP_PATH,
	.nv_data_backup_md5_path = MAGURO_NV_DATA_BACKUP_MD5_PATH,
	.nv_data_secret = XMM626_NV_DATA_SECRET,
	.nv_data_size = XMM626_NV_DATA_SIZE,
	.nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};
