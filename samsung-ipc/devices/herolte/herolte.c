/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2017 Wolfgang Wiedmeyer <wolfgit@wiedmeyer.de>
 * Copyright (C) 2020 Tony Garnock-Jones <tonyg@leastfixedpoint.com>
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

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "devices/herolte/herolte.h"
#include "ipc.h"
#include "modems/xmm626/xmm626.h"
#include "modems/xmm626/xmm626_kernel_smdk4412.h"
#include "modems/xmm626/xmm626_modem_prj.h"
#include "partitions/android/android.h"
#include "partitions/toc/toc.h"

struct __attribute__((__packed__)) security_req {
	uint32_t mode;
	uint32_t size_boot;
	uint32_t size_main;
	uint32_t pad_zero;
};

struct __attribute__((__packed__)) modem_firmware_partition_data {
	uint8_t *binary;
	uint32_t size;
	uint32_t m_offset;
	uint32_t b_offset;
	uint32_t mode;
	ssize_t len;
};

#define IOCTL_SECURITY_REQ _IO('o', 0x53)

#define MAX_CHUNK_LEN (62 * 1024)  /* This is just what cbd uses.
				    * Perhaps a larger value would also work.
				    */

static int upload_chunk(struct ipc_client *client,
			int device_fd,
			int firmware_fd,
			struct firmware_toc_entry const *toc,
			char const *name,
			uint32_t *size)
{
	int rc = -1;
	struct modem_firmware_partition_data partition;
	struct firmware_toc_entry const *boot_toc_entry;
	struct firmware_toc_entry const *current_toc_entry;
	uint32_t remaining;

	ipc_client_log(client, "Uploading %s", name);

	boot_toc_entry = find_toc_entry("BOOT", toc);
	if (boot_toc_entry == NULL) {
		ipc_client_log(client,
			       "%s: Failed to find BOOT entry in the TOC",
			       __func__);
		goto exit;
	}

	current_toc_entry = find_toc_entry(name, toc);
	if (current_toc_entry == NULL) {
		ipc_client_log(client, "%s: Failed to find %s entry in the TOC",
			       __func__, name);
		goto exit;
	}

	if (size != NULL)
		*size = current_toc_entry->size;
	ipc_client_log(client, " - blob size for %s is %lu", name,
		       current_toc_entry->size);

	partition.binary = calloc(1, MAX_CHUNK_LEN);
	if (partition.binary == NULL) {
		rc = -errno;
		ipc_client_log(client, "%s: calloc failed with error %d: %s",
			       __func__, rc, strerror(rc));
		goto exit;
	}

	partition.size = current_toc_entry->size;
	partition.m_offset = current_toc_entry->loadaddr -
		boot_toc_entry->loadaddr;
	partition.b_offset = current_toc_entry->offset;
	partition.mode = 0;
	partition.len = 0;

	if (lseek(firmware_fd, partition.b_offset, SEEK_SET) < 0) {
		rc = -errno;
		ipc_client_log(client, "%s: lseek failed with error %d: %s",
			       __func__, rc, strerror(rc));
		goto exit;
	}

	remaining = partition.size;
	while (remaining > 0) {
		partition.len = remaining < MAX_CHUNK_LEN ?
			remaining : MAX_CHUNK_LEN;
		if (data_read(client, firmware_fd, partition.binary,
			      partition.len) != partition.len) {
			rc = errno;
			ipc_client_log(client,
				       "%s: read failed with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}

		if (ioctl(device_fd, IOCTL_DPRAM_SEND_BOOT, &partition) == -1) {
			rc = errno;
			ipc_client_log(client,
				       "%s: IOCTL_DPRAM_SEND_BOOT failed"
				       " with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}
		partition.m_offset += partition.len;
		partition.b_offset += partition.len;
		remaining -= partition.len;
	}

	rc = 0;

exit:
	if (partition.binary != NULL) {
		partition.binary = NULL;
		free(partition.binary);
	}

	return rc;
}

static int select_secure_mode(struct ipc_client *client,
			      int boot0_fd,
			      int secure,
			      uint32_t size_boot,
			      uint32_t size_main)
{
	struct security_req req;
	int rc;

	ipc_client_log(client,
		       "Issuing IOCTL_SECURITY_REQ - setting %s mode",
		       secure ? "secure" : "insecure");

	req.mode = secure ? 0 : 2;
	req.size_boot = size_boot;
	req.size_main = size_main;
	req.pad_zero = 0;

	if (ioctl(boot0_fd, IOCTL_SECURITY_REQ, &req) == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: "
			       "IOCTL_SECURITY_REQ failed with error %d: %s",
			       __func__, rc, strerror(rc));
		return -1;
	}

	return 0;
}

static char const * const modem_image_devices[] = {
	"/dev/disk/by-partlabel/RADIO",                   /* PostmarketOS */
	"/dev/block/platform/155a0000.ufs/by-name/RADIO", /* LineageOS */
	NULL
};

int herolte_boot(struct ipc_client *client)
{
	struct firmware_toc_entry toc[N_TOC_ENTRIES];
	int boot0_fd = -1;
	int imagefd = -1;
	int nvfd = -1;
	int rc = -1;
	uint32_t size_boot;
	uint32_t size_main;

	ipc_client_log(client, "Loading firmware TOC");

	imagefd = open_android_modem_partition(client, modem_image_devices);
	if (imagefd == -1) {
		rc = errno;
		if (rc == ENOENT)
			ipc_client_log(client,
				       "%s: no modem image block device found!",
				       __func__);
		else
			ipc_client_log(client,
				       "%s: "
				       "open_image_device failed with error %d:"
				       " %s",
				       __func__, rc, strerror(rc));
		goto exit;
	}

	if (data_read(client, imagefd, &toc[0], sizeof(toc)) != sizeof(toc)) {
		rc = errno;
		ipc_client_log(client,
			       "%s: read modem image block device failed "
			       " with error %d: %s",
			       __func__, rc, strerror(rc));
		goto exit;
	}

	ipc_client_log(client, "Loaded firmware TOC");

	nvfd = open(herolte_nv_data_specs.nv_data_path, O_RDONLY | O_NOCTTY);
	if (nvfd == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: opening %s failed with error %d: %s",
			       __func__, herolte_nv_data_specs.nv_data_path,
			       rc, strerror(rc));
		goto exit;
	}
	ipc_client_log(client, "Opened NV data file");

	boot0_fd = open(XMM626_SEC_MODEM_BOOT0_DEVICE, O_RDWR | O_NOCTTY);
	if (boot0_fd < 0) {
		rc = errno;
		ipc_client_log(client,
			       "%s: opening %s failed with error %d: %s",
			       __func__, XMM626_SEC_MODEM_BOOT0_DEVICE,
			       rc, strerror(rc));
		goto exit;
	}

	ipc_client_log(client, "Resetting modem");
	if (ioctl(boot0_fd, IOCTL_MODEM_RESET, 0) == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: "
			       "IOCTL_MODEM_RESET failed with error %d: %s",
			       __func__, rc, strerror(rc));
		goto exit;
	}

	if (select_secure_mode(client, boot0_fd, 0, 0, 0) < 0)
		goto exit;

	if (upload_chunk(client, boot0_fd, imagefd, toc, "BOOT",
			 &size_boot) < 0)
		goto exit;

	if (upload_chunk(client, boot0_fd, imagefd, toc, "MAIN",
			 &size_main) < 0)
		goto exit;

	if (upload_chunk(client, boot0_fd, nvfd, toc, "NV", NULL) < 0)
		goto exit;

	if (select_secure_mode(client, boot0_fd, 1, size_boot, size_main) < 0)
		goto exit;

	ipc_client_log(client, "Powering on modem");
	if (xmm626_kernel_smdk4412_power(client, boot0_fd, 1) == -1) {
		ipc_client_log(client, "%s: Powering on modem failed",
			       __func__);
		goto exit;
	}

	ipc_client_log(client, "Starting modem boot process");
	if (xmm626_kernel_smdk4412_boot_power(client, boot0_fd, 1) == -1) {
		ipc_client_log(client, "%s: Starting modem boot process failed",
			__func__);
		goto exit;
	}

	ipc_client_log(client, "Kicking off firmware download");
	if (ioctl(boot0_fd, IOCTL_MODEM_DL_START, 0) < 0) {
		rc = errno;
		ipc_client_log(client,
			       "%s: "
			       "IOCTL_MODEM_RESET failed with error %d: %s",
			       __func__, rc, strerror(rc));
		goto exit;
	}

	ipc_client_log(client, "Handshaking with modem");
	/* At this point, cbd engages in a little dance with the
	 * newly-booted modem, apparently to verify that it is running
	 * as expected. I don’t know the sources of these magic
	 * numbers, I just faithfully reproduce them.
	 */
	{
		uint32_t buf;

		buf = 0x900d;
		if (data_write(client, boot0_fd, &buf, sizeof(buf)) !=
		    sizeof(buf)) {
			rc = errno;
			ipc_client_log(client,
				       "%s: write failed with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}
		if (data_read(client, boot0_fd, &buf, sizeof(buf)) !=
		    sizeof(buf)) {
			rc = errno;
			ipc_client_log(client,
				       "%s: read failed with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}
		if (buf != 0xa00d) {
			ipc_client_log(client,
				       "%s: Handshake stage I failed: "
				       "expected 0xa00d, got 0x%x instead",
				       __func__, buf);
			goto exit;
		}
		ipc_client_log(client, "Handshake stage I passed");

		buf = 0x9f00;
		if (data_write(client, boot0_fd, &buf,
			       sizeof(buf)) != sizeof(buf)) {
			rc = errno;
			ipc_client_log(client,
				       "%s: write failed with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}
		if (data_read(client, boot0_fd, &buf,
			      sizeof(buf)) != sizeof(buf)) {
			rc = errno;
			ipc_client_log(client,
				       "%s: read failed with error %d: %s",
				       __func__, rc, strerror(rc));
			goto exit;
		}
		if (buf != 0xaf00) {
			ipc_client_log(client,
				       "%s: Handshake stage II failed: "
				       "expected 0xaf00, got 0x%x instead",
				       __func__, buf);
			goto exit;
		}
		ipc_client_log(client, "Handshake stage II passed");
	}

	ipc_client_log(client, "Finishing modem boot process");
	if (xmm626_kernel_smdk4412_boot_power(client, boot0_fd, 0) == -1) {
		ipc_client_log(client,
			       "%s: xmm626_kernel_smdk4412_boot_power failed",
			__func__);
		goto exit;
	}

	ipc_client_log(client, "Modem boot complete");
	rc = 0;

	/* Samsung's official daemons continue to read from umts_boot0
	 * (XMM626_SEC_MODEM_BOOT0_DEVICE) at this point. It may be
	 * done to restart the modem in case of errors. The fact that
	 * I have never seen anything actually come out of umts_boot0
	 * after booting is complete with libsamsung-ipc seem to be
	 * consistent with that hypothesis. With libsamsung-ipc,
	 * it's up to the daemon using it (like libsamsung-ril) to restart
	 * the boot sequence if .poll (here herolte_poll) fails.
	 * For that to work, the kernel driver needs to return an error
	 * if the modem crash for instance.
	 */

	/* The kernel modem driver for this device[1] checks if both the FMT
	 * and the RFS channels are open (in the rild_ready function) and will
	 * refuse to work if both channels aren't open. Note that even if
	 * libsamsung-ipc opens both channels, at the time of writing, none
	 * of the tools in tools/ opens the RFS channel.
	 *
	 * [1]See the rild_ready function and its usage in
	 * drivers/misc/modem_v1/link_device_shmem.c in the lineage-17.0 branch
	 * https://github.com/ivanmeler/android_kernel_samsung_herolte
	 */

exit:
	if (boot0_fd != -1)
		close(boot0_fd);
	if (imagefd != -1)
		close(imagefd);
	if (nvfd != -1)
		close(nvfd);
	return rc;
}

int herolte_open(__attribute__((unused)) struct ipc_client *client, void *data,
	       int type)
{
	struct herolte_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct herolte_transport_data *) data;

	transport_data->fd = xmm626_kernel_smdk4412_open(client, type);
	if (transport_data->fd < 0)
		return -1;

	return 0;
}

int herolte_close(__attribute__((unused)) struct ipc_client *client, void *data)
{
	struct herolte_transport_data *transport_data;

	if (data == NULL)
		return -1;

	transport_data = (struct herolte_transport_data *) data;

	xmm626_kernel_smdk4412_close(client, transport_data->fd);
	transport_data->fd = -1;

	return 0;
}

int herolte_read(__attribute__((unused)) struct ipc_client *client, void *data,
	       void *buffer, size_t length)
{
	struct herolte_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct herolte_transport_data *) data;

	rc = xmm626_kernel_smdk4412_read(client, transport_data->fd, buffer,
					 length);

	return rc;
}

int herolte_write(__attribute__((unused)) struct ipc_client *client, void *data,
		const void *buffer, size_t length)
{
	struct herolte_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct herolte_transport_data *) data;

	rc = xmm626_kernel_smdk4412_write(client, transport_data->fd, buffer,
					  length);

	return rc;
}

int herolte_poll(__attribute__((unused)) struct ipc_client *client, void *data,
	       struct ipc_poll_fds *fds, struct timeval *timeout)
{
	struct herolte_transport_data *transport_data;
	int rc;

	if (data == NULL)
		return -1;

	transport_data = (struct herolte_transport_data *) data;

	rc = xmm626_kernel_smdk4412_poll(client, transport_data->fd, fds,
					 timeout);

	return rc;
}

int herolte_power_on(__attribute__((unused)) struct ipc_client *client,
		   __attribute__((unused)) void *data)
{
	return 0;
}

int herolte_power_off(__attribute__((unused)) struct ipc_client *client,
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

int herolte_gprs_activate(__attribute__((unused)) struct ipc_client *client,
			__attribute__((unused)) void *data,
			__attribute__((unused)) unsigned int cid)
{
	/* TODO: For now, we don't have enough information to
	 * implement this sensibly, hence this placeholder.
	 */

	return 0;
}

int herolte_gprs_deactivate(__attribute__((unused)) struct ipc_client *client,
			  __attribute__((unused)) void *data,
			  __attribute__((unused)) unsigned int cid)
{
	/* TODO: For now, we don't have enough information to
	 * implement this sensibly, hence this placeholder.
	 */

	return 0;
}

int herolte_data_create(__attribute__((unused)) struct ipc_client *client,
		      void **transport_data,
		      __attribute__((unused)) void **power_data,
		      __attribute__((unused)) void **gprs_data)
{
	if (transport_data == NULL)
		return -1;

	*transport_data = calloc(1, sizeof(struct herolte_transport_data));

	return 0;
}

int herolte_data_destroy(__attribute__((unused)) struct ipc_client *client,
		       void *transport_data,
		       __attribute__((unused)) void *power_data,
		       __attribute__((unused)) void *gprs_data)
{
	if (transport_data == NULL)
		return -1;

	free(transport_data);

	return 0;
}

struct ipc_client_ops herolte_fmt_ops = {
	.boot = herolte_boot,
	.send = xmm626_kernel_smdk4412_fmt_send,
	.recv = xmm626_kernel_smdk4412_fmt_recv,
};

struct ipc_client_ops herolte_rfs_ops = {
	.boot = NULL,
	.send = xmm626_kernel_smdk4412_rfs_send,
	.recv = xmm626_kernel_smdk4412_rfs_recv,
};

struct ipc_client_handlers herolte_handlers = {
	.read = herolte_read,
	.write = herolte_write,
	.open = herolte_open,
	.close = herolte_close,
	.poll = herolte_poll,
	.transport_data = NULL,
	.power_on = herolte_power_on,
	.power_off = herolte_power_off,
	.power_data = NULL,
	.gprs_activate = herolte_gprs_activate,
	.gprs_deactivate = herolte_gprs_deactivate,
	.gprs_data = NULL,
	.data_create = herolte_data_create,
	.data_destroy = herolte_data_destroy,
};

struct ipc_client_gprs_specs herolte_gprs_specs = {
	.gprs_get_iface = xmm626_kernel_smdk4412_gprs_get_iface,
	.gprs_get_capabilities = xmm626_kernel_smdk4412_gprs_get_capabilities,
};

struct ipc_client_nv_data_specs herolte_nv_data_specs = {
	.nv_data_path = XMM626_NV_DATA_PATH,
	.nv_data_md5_path = XMM626_NV_DATA_MD5_PATH,
	.nv_data_backup_path = XMM626_NV_DATA_BACKUP_PATH,
	.nv_data_backup_md5_path = XMM626_NV_DATA_BACKUP_MD5_PATH,
	.nv_data_secret = XMM626_NV_DATA_SECRET,
	.nv_data_size = XMM626_NV_DATA_SIZE,
	.nv_data_chunk_size = XMM626_NV_DATA_CHUNK_SIZE,
};
