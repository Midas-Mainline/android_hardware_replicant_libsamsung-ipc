/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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
#include <string.h>

#include <samsung-ipc.h>

int ipc_sec_pin_status_setup(struct ipc_sec_pin_status_request_data *data,
			     unsigned char type, const char *pin1,
			     const char *pin2)
{
	size_t pin1_length;
	size_t pin2_length;

	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_sec_pin_status_request_data));
	data->type = type;

	if (pin1 != NULL) {
		pin1_length = strlen(pin1);
		if (pin1_length > sizeof(data->pin1))
			pin1_length = sizeof(data->pin1);

		data->pin1_length = (unsigned char) pin1_length;
		strncpy((char *) data->pin1, pin1, pin1_length);
	}

	if (pin2 != NULL) {
		pin2_length = strlen(pin2);
		if (pin2_length > sizeof(data->pin2))
			pin2_length = sizeof(data->pin2);

		data->pin2_length = (unsigned char) pin2_length;
		strncpy((char *) data->pin2, pin2, pin2_length);
	}

	return 0;
}

int ipc_sec_phone_lock_request_set_setup(
	struct ipc_sec_phone_lock_request_set_data *data,
	unsigned char facility_type, unsigned char active, const char *password)
{
	size_t password_length;

	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_sec_phone_lock_request_set_data));
	data->facility_type = facility_type;
	data->active = active;

	if (password != NULL) {
		password_length = strlen(password);
		if (password_length > sizeof(data->password))
			password_length = sizeof(data->password);

		data->password_length = (unsigned char) password_length;
		strncpy((char *) data->password, password, password_length);
	} else {
		data->password_length = 0;
	}

	return 0;
}

int ipc_sec_change_locking_pw_setup(
	struct ipc_sec_change_locking_pw_data *data,
	unsigned char facility_type, const char *password_old,
	const char *password_new)
{
	size_t password_old_length;
	size_t password_new_length;

	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_sec_change_locking_pw_data));
	data->facility_type = facility_type;

	if (password_old != NULL) {
		password_old_length = strlen(password_old);
		if (password_old_length > sizeof(data->password_old))
			password_old_length = sizeof(data->password_old);

		data->password_old_length = (unsigned char) password_old_length;
		strncpy((char *) data->password_old, password_old,
			password_old_length);
	} else {
		data->password_old_length = 0;
	}

	if (password_new != NULL) {
		password_new_length = strlen(password_new);
		if (password_new_length > sizeof(data->password_new))
			password_new_length = sizeof(data->password_new);

		data->password_new_length = (unsigned char) password_new_length;
		strncpy((char *) data->password_new, password_new,
			password_new_length);
	} else {
		data->password_new_length = 0;
	}

	return 0;
}

size_t ipc_sec_rsim_access_size_setup(
	struct ipc_sec_rsim_access_request_header *header,
	const void *sim_io_data, size_t sim_io_size)
{
	size_t size;

	if (header == NULL)
		return 0;

	if (sim_io_data == NULL)
		sim_io_size = 0;

	size = sizeof(struct ipc_sec_rsim_access_request_header) + sim_io_size;

	return size;
}

void *ipc_sec_rsim_access_setup(
	struct ipc_sec_rsim_access_request_header *header,
	const void *sim_io_data, size_t sim_io_size)
{
	void *data;
	size_t size;
	unsigned char *p;

	if (header == NULL)
		return NULL;

	if (sim_io_data == NULL)
		sim_io_size = 0;

	size = ipc_sec_rsim_access_size_setup(header, sim_io_data, sim_io_size);
	if (size == 0)
		return NULL;

	data = calloc(1, size);

	p = (unsigned char *) data;

	memcpy(p, header, sizeof(struct ipc_sec_rsim_access_request_header));
	p += sizeof(struct ipc_sec_rsim_access_request_header);

	if (sim_io_data != NULL && sim_io_size > 0) {
		memcpy(p, sim_io_data, sim_io_size);
		p += sim_io_size;
	}

	return data;
}

size_t ipc_sec_rsim_access_size_extract(const void *data, size_t size)
{
	struct ipc_sec_rsim_access_response_header *header;

	if (data == NULL ||
	    size < sizeof(struct ipc_sec_rsim_access_response_header)) {
		return 0;
	}

	header = (struct ipc_sec_rsim_access_response_header *) data;
	if (header->length == 0 ||
	    header->length > size -
	    sizeof(struct ipc_sec_rsim_access_response_header)) {
		return 0;
	}

	return (size_t) header->length;
}

void *ipc_sec_rsim_access_extract(const void *data, size_t size)
{
	struct ipc_sec_rsim_access_response_header *header;
	void *rsim_data;

	if (data == NULL ||
	    size < sizeof(struct ipc_sec_rsim_access_response_header)) {
		return NULL;
	}

	header = (struct ipc_sec_rsim_access_response_header *) data;
	if (header->length == 0 ||
	    header->length > size -
	    sizeof(struct ipc_sec_rsim_access_response_header)) {
		return NULL;
	}

	rsim_data = (void *) (
		(unsigned char *) data +
		sizeof(struct ipc_sec_rsim_access_response_header));

	return rsim_data;
}

int ipc_sec_lock_information_setup(
	struct ipc_sec_lock_information_request_data *data, unsigned char type)
{
	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_sec_lock_information_request_data));
	data->magic = 0x01;
	data->type = type;

	return 0;
}
