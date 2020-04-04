/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2014 Paul Kocialkowski <contact@paulk.fr>
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

int ipc_misc_me_version_setup(struct ipc_misc_me_version_request_data *data)
{
	if (data == NULL)
		return -1;

	memset(data, 0, sizeof(struct ipc_misc_me_version_request_data));
	data->magic = 0xff;

	return 0;
}

char *ipc_misc_me_imsi_imsi_extract(const void *data, size_t size)
{
	struct ipc_misc_me_imsi_header *header;
	char *imsi;
	size_t imsi_length;

	if (data == NULL || size < sizeof(struct ipc_misc_me_imsi_header))
		return NULL;

	header = (struct ipc_misc_me_imsi_header *) data;

	/* header->length doesn't count the final null character */
	imsi_length = header->length + sizeof(char);

	imsi = (char *) calloc(1, imsi_length);

	strncpy(imsi, (char *) data + sizeof(struct ipc_misc_me_imsi_header),
		header->length);
	imsi[header->length] = '\0';

	return imsi;
}

char *ipc_misc_me_sn_extract(const struct ipc_misc_me_sn_response_data *data)
{
	char *string;
	size_t length;

	if (data == NULL || data->length > sizeof(data->data))
		return NULL;

	length = data->length + sizeof(char);

	string = (char *) calloc(1, length);

	strncpy(string, (char *) &data->data, data->length);
	string[data->length] = '\0';

	return string;
}
