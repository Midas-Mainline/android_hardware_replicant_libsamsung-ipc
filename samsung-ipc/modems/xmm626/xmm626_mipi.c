/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on the incomplete C++ implementation which is:
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include <samsung-ipc.h>

#include "modems/xmm626/xmm626.h"
#include "modems/xmm626/xmm626_mipi.h"

int xmm626_mipi_crc_calculate(const void *data, size_t size)
{
	unsigned char crc;
	int mipi_crc;

	crc = xmm626_crc_calculate(data, size);
	mipi_crc = (crc << 24) | 0xffffff;

	return mipi_crc;
}

int xmm626_mipi_ack_read(__attribute__((unused)) struct ipc_client *client,
			 int device_fd, unsigned short ack)
{
	struct timeval timeout;
	fd_set fds;
	unsigned int value;
	int rc;
	int i;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(device_fd, &fds);

	for (i = 0; i < 50; i++) {
		rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
		if (rc <= 0)
			return -1;

		value = 0;
		rc = read(device_fd, &value, sizeof(value));
		if (rc < (int) sizeof(value))
			continue;

		if ((value & 0xffff) == ack)
			return 0;
	}

	return -1;
}

int xmm626_mipi_psi_send(struct ipc_client *client, int device_fd,
			 const void *psi_data, unsigned short psi_size)
{
	struct xmm626_mipi_psi_header psi_header;
	char at[] = XMM626_AT;
	int psi_crc;
	struct timeval timeout;
	fd_set fds;
	size_t wc;
	size_t length;
	unsigned char *p;
	int rc;
	int i;

	if (client == NULL || device_fd < 0 || psi_data == NULL ||
	    psi_size == 0) {
		return -1;
	}

	FD_ZERO(&fds);

	i = 0;
	length = strlen(at);

	do {
		FD_SET(device_fd, &fds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		rc = write(device_fd, at, length);
		if (rc < (int) length) {
			ipc_client_log(client, "Writing ATAT in ASCII failed");
			goto error;
		}
		ipc_client_log(client, "Wrote ATAT in ASCII");

		rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
		if (rc < 0) {
			ipc_client_log(client, "Waiting for bootup failed");
			goto error;
		}

		if (i++ > 50) {
			ipc_client_log(client, "Waiting for bootup failed");
			goto error;
		}
	} while (rc == 0);

	rc = xmm626_mipi_ack_read(client, device_fd, XMM626_MIPI_BOOT0_ACK);
	if (rc < 0) {
		ipc_client_log(client, "Reading boot ACK failed");
		goto error;
	}

	psi_header.padding = XMM626_PSI_PADDING;
	psi_header.length = ((psi_size >> 8) & 0xff) | ((psi_size & 0xff) << 8);
	psi_header.magic = XMM626_PSI_MAGIC;

	rc = write(device_fd, &psi_header, sizeof(psi_header));
	if (rc < (int) sizeof(psi_header)) {
		ipc_client_log(client, "Writing PSI header failed");
		goto error;
	}
	ipc_client_log(client, "Wrote PSI header");

	p = (unsigned char *) psi_data;

	wc = 0;
	while (wc < psi_size) {
		rc = write(device_fd, (void *) p, psi_size - wc);
		if (rc <= 0) {
			ipc_client_log(client, "Writing PSI failed");
			goto error;
		}

		p += rc;
		wc += rc;
	}

	psi_crc = xmm626_mipi_crc_calculate(psi_data, psi_size);

	ipc_client_log(client, "Wrote PSI, CRC is 0x%x", psi_crc);

	rc = write(device_fd, &psi_crc, sizeof(psi_crc));
	if (rc < (int) sizeof(psi_crc)) {
		ipc_client_log(client, "Writing PSI CRC failed");
		goto error;
	}
	ipc_client_log(client, "Wrote PSI CRC (0x%x)", psi_crc);

	rc = xmm626_mipi_ack_read(client, device_fd, XMM626_MIPI_PSI_ACK);
	if (rc < 0) {
		ipc_client_log(client, "Reading PSI ACK failed");
		goto error;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	return rc;
}

int xmm626_mipi_ebl_send(struct ipc_client *client, int device_fd,
			 const void *ebl_data, size_t ebl_size)
{
	unsigned short boot_magic[4];
	unsigned char ebl_crc;
	size_t chunk;
	size_t count;
	size_t wc;
	size_t size;
	size_t length;
	unsigned char *p;
	int rc;

	if (client == NULL || device_fd < 0 || ebl_data == NULL ||
	    ebl_size == 0) {
		return -1;
	}

	boot_magic[0] = 0;
	boot_magic[1] = 0;
	boot_magic[2] = XMM626_MIPI_BOOT1_MAGIC;
	boot_magic[3] = XMM626_MIPI_BOOT1_MAGIC;

	length = sizeof(boot_magic);

	rc = write(device_fd, &length, sizeof(length));
	if (rc < (int) sizeof(length)) {
		ipc_client_log(client, "Writing boot magic length failed");
		goto error;
	}

	rc = write(device_fd, &boot_magic, length);
	if (rc < (int) length) {
		ipc_client_log(client, "Writing boot magic failed");
		goto error;
	}
	ipc_client_log(client, "Wrote boot magic");

	rc = xmm626_mipi_ack_read(client, device_fd, XMM626_MIPI_BOOT1_ACK);
	if (rc < 0) {
		ipc_client_log(client, "Reading boot magic ACK failed");
		goto error;
	}

	size = sizeof(ebl_size);

	rc = write(device_fd, &size, sizeof(size));
	if (rc < (int) sizeof(size)) {
		ipc_client_log(client, "Writing EBL size length failed");
		goto error;
	}

	rc = write(device_fd, &ebl_size, size);
	if (rc < (int) size) {
		ipc_client_log(client, "Writing EBL size failed");
		goto error;
	}
	ipc_client_log(client, "Wrote EBL size");

	rc = xmm626_mipi_ack_read(client, device_fd, XMM626_MIPI_EBL_SIZE_ACK);
	if (rc < 0) {
		ipc_client_log(client, "Reading EBL size ACK failed");
		goto error;
	}

	ebl_size++;

	rc = write(device_fd, &ebl_size, size);
	if (rc < (int) size) {
		ipc_client_log(client, "Writing EBL size failed");
		goto error;
	}

	ebl_size--;

	p = (unsigned char *) ebl_data;

	chunk = XMM626_MIPI_EBL_CHUNK;
	wc = 0;
	while (wc < ebl_size) {
		count = chunk < ebl_size - wc ? chunk : ebl_size - wc;

		rc = write(device_fd, (void *) p, count);
		if (rc <= 0) {
			ipc_client_log(client, "Writing EBL failed");
			goto error;
		}

		p += rc;
		wc += rc;
	}

	ebl_crc = xmm626_crc_calculate(ebl_data, ebl_size);

	ipc_client_log(client, "Wrote EBL, CRC is 0x%x", ebl_crc);

	rc = write(device_fd, &ebl_crc, sizeof(ebl_crc));
	if (rc < (int) sizeof(ebl_crc)) {
		ipc_client_log(client, "Writing EBL CRC failed");
		goto error;
	}
	ipc_client_log(client, "Wrote EBL CRC (0x%x)", ebl_crc);

	rc = xmm626_mipi_ack_read(client, device_fd, XMM626_MIPI_EBL_ACK);
	if (rc < 0) {
		ipc_client_log(client, "Reading EBL ACK failed");
		goto error;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	return rc;
}

int xmm626_mipi_command_send(__attribute__((unused)) struct ipc_client *client,
			     int device_fd, unsigned short code,
			     const void *data, size_t size, int ack,
			     int short_footer)
{
	struct xmm626_mipi_command_header header;
	struct xmm626_mipi_command_footer footer;
	void *buffer = NULL;
	size_t length;
	size_t footer_length;
	struct timeval timeout;
	fd_set fds;
	size_t chunk;
	size_t c;
	unsigned char *p;
	int rc;
	int i;

	if (device_fd < 0 || data == NULL || size <= 0)
		return -1;

	header.size = size + sizeof(header);
	header.magic = XMM626_MIPI_COMMAND_HEADER_MAGIC;
	header.code = code;
	header.data_size = size;

	footer.checksum = (size & 0xffff) + code;
	footer.magic = XMM626_MIPI_COMMAND_FOOTER_MAGIC;
	footer.unknown = XMM626_MIPI_COMMAND_FOOTER_UNKNOWN;

	p = (unsigned char *) data;

	for (i = 0; i < (int) size; i++)
		footer.checksum += *p++;

	footer_length = sizeof(footer);
	if (short_footer)
		footer_length -= sizeof(short);

	length = sizeof(header) + size + footer_length;
	buffer = calloc(1, length);

	p = (unsigned char *) buffer;
	memcpy(p, &header, sizeof(header));
	p += sizeof(header);
	memcpy(p, data, size);
	p += size;
	memcpy(p, &footer, footer_length);

	rc = write(device_fd, buffer, length);
	if (rc < (int) length)
		goto error;

	free(buffer);
	buffer = NULL;

	if (!ack) {
		rc = 0;
		goto complete;
	}

	FD_ZERO(&fds);
	FD_SET(device_fd, &fds);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
	if (rc <= 0)
		goto error;

	rc = read(device_fd, &length, sizeof(length));
	if (rc < (int) sizeof(length) || length <= 0)
		goto error;

	length += sizeof(unsigned int);
	if (length % 4 != 0)
		length += length % 4;

	if (length < (int) sizeof(buffer))
		goto error;

	buffer = calloc(1, length);

	p = (unsigned char *) buffer;
	memcpy(p, &length, sizeof(length));
	p += sizeof(length);

	chunk = 4;
	c = sizeof(length);
	while (c < length) {
		rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
		if (rc <= 0)
			goto error;

		rc = read(device_fd, (void *) p, chunk);
		if (rc < (int) chunk)
			goto error;

		p += rc;
		c += rc;
	}

	memcpy(&header, buffer, sizeof(header));
	if (header.code != code)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	return rc;
}

int xmm626_mipi_modem_data_send(struct ipc_client *client,
	int device_fd, const void *data, size_t size,
	int address)
{
	size_t chunk;
	size_t count;
	size_t c;
	unsigned char *p;
	int rc;

	if (device_fd < 0 || data == NULL || size == 0)
		return -1;

	rc = xmm626_mipi_command_send(client, device_fd,
				      XMM626_COMMAND_FLASH_SET_ADDRESS,
				      &address, sizeof(address), 1, 0);
	if (rc < 0)
		goto error;

	p = (unsigned char *) data;

	chunk = XMM626_MIPI_MODEM_DATA_CHUNK;
	c = 0;
	while (c < size) {
		count = chunk < size - c ? chunk : size - c;

		rc = xmm626_mipi_command_send(client, device_fd,
					      XMM626_COMMAND_FLASH_WRITE_BLOCK,
					      p, count, 1, 1);
		if (rc < 0)
			goto error;

		p += count;
		c += count;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	return rc;
}

int xmm626_mipi_port_config_send(struct ipc_client *client, int device_fd)
{
	void *buffer = NULL;
	size_t length;
	struct timeval timeout;
	fd_set fds;
	size_t chunk;
	size_t count;
	size_t c;
	unsigned char *p;
	int rc;

	if (client == NULL || device_fd < 0)
		return -1;

	FD_ZERO(&fds);
	FD_SET(device_fd, &fds);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
	if (rc <= 0)
		goto error;

	rc = read(device_fd, &length, sizeof(length));
	if (rc < (int) sizeof(length) || length == 0) {
		ipc_client_log(client, "Reading port config length failed");
		goto error;
	}
	ipc_client_log(client, "Read port config length (0x%x)", length);

	buffer = calloc(1, length);

	p = (unsigned char *) buffer;

	chunk = 4;
	c = 0;
	while (c < length) {
		count = chunk < length - c ? chunk : length - c;

		rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
		if (rc <= 0)
			goto error;

		rc = read(device_fd, p, count);
		if (rc < (int) count) {
			ipc_client_log(client, "Reading port config failed");
			goto error;
		}

		p += count;
		c += count;
	}
	ipc_client_log(client, "Read port config");

	rc = xmm626_mipi_command_send(client, device_fd,
				      XMM626_COMMAND_SET_PORT_CONFIG, buffer,
				      length, 1, 0);
	if (rc < 0) {
		ipc_client_log(client, "Sending port config command failed");
		goto error;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	return rc;
}

int xmm626_mipi_sec_start_send(struct ipc_client *client, int device_fd,
			       const void *sec_data, size_t sec_size)
{
	int rc;

	if (client == NULL || device_fd < 0 || sec_data == NULL ||
	    sec_size == 0) {
		return -1;
	}

	rc = xmm626_mipi_command_send(client, device_fd,
				      XMM626_COMMAND_SEC_START, sec_data,
				      sec_size, 1, 0);
	if (rc < 0)
		return -1;

	return 0;
}

int xmm626_mipi_sec_end_send(struct ipc_client *client, int device_fd)
{
	unsigned short sec_data;
	size_t sec_size;
	int rc;

	if (client == NULL || device_fd < 0)
		return -1;

	sec_data = XMM626_SEC_END_MAGIC;
	sec_size = sizeof(sec_data);

	rc = xmm626_mipi_command_send(client, device_fd, XMM626_COMMAND_SEC_END,
				      &sec_data, sec_size, 1, 1);
	if (rc < 0)
		return -1;

	return 0;
}

int xmm626_mipi_firmware_send(struct ipc_client *client, int device_fd,
			      const void *firmware_data, size_t firmware_size)
{
	int rc;

	if (client == NULL || device_fd < 0 || firmware_data == NULL ||
	    firmware_size == 0) {
		return -1;
	}

	rc = xmm626_mipi_modem_data_send(client, device_fd, firmware_data,
					 firmware_size,
					 XMM626_FIRMWARE_ADDRESS);
	if (rc < 0)
		return -1;

	return 0;
}

int xmm626_mipi_nv_data_send(struct ipc_client *client, int device_fd)
{
	void *nv_data = NULL;
	size_t nv_size;
	int rc;

	if (client == NULL || device_fd < 0)
		return -1;

	nv_size = ipc_client_nv_data_size(client);
	if (nv_size == 0)
		return -1;

	nv_data = ipc_nv_data_load(client);
	if (nv_data == NULL) {
		ipc_client_log(client, "Loading nv_data failed");
		goto error;
	}
	ipc_client_log(client, "Loaded nv_data");

	rc = xmm626_mipi_modem_data_send(client, device_fd, nv_data, nv_size,
					 XMM626_NV_DATA_ADDRESS);
	if (rc < 0)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (nv_data != NULL)
		free(nv_data);

	return rc;
}

int xmm626_mipi_mps_data_send(struct ipc_client *client, int device_fd,
			      const void *mps_data, size_t mps_size)
{
	int rc;

	if (client == NULL || device_fd < 0 || mps_data == NULL ||
	    mps_size == 0) {
		return -1;
	}

	rc = xmm626_mipi_modem_data_send(client, device_fd, mps_data, mps_size,
					 XMM626_MPS_DATA_ADDRESS);
	if (rc < 0)
		return -1;

	return 0;
}

int xmm626_mipi_hw_reset_send(struct ipc_client *client, int device_fd)
{
	unsigned int hw_reset_data;
	size_t hw_reset_size;
	int rc;

	if (client == NULL || device_fd < 0)
		return -1;

	hw_reset_data = XMM626_HW_RESET_MAGIC;
	hw_reset_size = sizeof(hw_reset_data);

	rc = xmm626_mipi_command_send(client, device_fd,
				      XMM626_COMMAND_HW_RESET, &hw_reset_data,
				      hw_reset_size, 0, 1);
	if (rc < 0)
		return -1;

	return 0;
}
