/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>

#include <samsung-ipc.h>

#include "ipc.h"

int ipc_seq_valid(unsigned char seq)
{
	if (seq == 0x00 || seq == 0xff)
		return 0;

	return 1;
}

int ipc_data_dump(struct ipc_client *client, const void *data, size_t size)
{
	unsigned int cols = 8;
	unsigned int cols_count = 2;
	int spacer;
	char string[81] = { 0 };
	char final[161] = { 0 };
	size_t length;
	char *print;
	unsigned char *p;
	unsigned int offset;
	unsigned int rollback;
	unsigned int i, j, k;
	int rc;

	if (data == NULL || size == 0)
		return -1;

	/*
	 * spacer = string length - offset print length - data print length -
	 *	    ascii print length
	 */
	spacer = (sizeof(string) - 1) - 6 -
		(3 * cols * cols_count - 1 + (cols_count - 1)) -
		(cols * cols_count + cols_count - 1);

	/* Need 3 spacers */
	spacer /= 3;

	if (spacer <= 0)
		return -1;

	p = (unsigned char *) data;
	offset = 0;

	while (offset < size) {
		rollback = 0;

		print = (char *) &string;
		length = sizeof(string);

		/* Offset print */

		rc = snprintf(print, length, "[%04x]", offset);
		print += rc;
		length -= rc;

		/* Spacer print */

		for (i = 0; i < (unsigned int) spacer; i++) {
			*print++ = ' ';
			length--;
		}

		/* Data print */

		for (i = 0; i < cols_count; i++) {
			for (j = 0; j < cols; j++) {
				if (offset < size) {
					rc = snprintf(print, length, "%02X",
						      *p);
					print += rc;
					length -= rc;

					p++;
					offset++;
					rollback++;
				} else {
					for (k = 0; k < 2; k++) {
						*print++ = ' ';
						length--;
					}
				}

				if (j != (cols - 1)) {
					*print++ = ' ';
					length--;
				}
			}

			if (i != (cols_count - 1)) {
				for (k = 0; k < 2; k++) {
					*print++ = ' ';
					length--;
				}
			}
		}

		/* Spacer print */
		for (i = 0; i < (unsigned int) spacer; i++) {
			*print++ = ' ';
			length--;
		}

		/* ASCII print */
		p -= rollback;
		offset -= rollback;

		for (i = 0; i < cols_count; i++) {
			for (j = 0; j < cols; j++) {
				if (offset < size) {
					if (isascii(*p) && isprint(*p))
						*print = *p;
					else
						*print = '.';

					print++;
					length--;

					p++;
					offset++;
					rollback++;
				}
			}

			if (i != (cols_count - 1) && offset < size) {
				*print++ = ' ';
				length--;
			}
		}

		*print = '\0';

		/* Escape string */
		j = 0;
		for (i = 0; i < sizeof(string); i++) {
			if (string[i] == '%')
				final[j++] = string[i];

			final[j++] = string[i];
		}

		ipc_client_log(client, final);
	}

	return 0;
}

void ipc_client_log_send(struct ipc_client *client, struct ipc_message *message,
			 const char *prefix)
{
	if (client == NULL || message == NULL || prefix == NULL)
		return;

	switch (client->type) {
	case IPC_CLIENT_TYPE_FMT:
		ipc_client_log(client, "\n");
		ipc_client_log(client, "%s: Sent FMT message", prefix);
		ipc_client_log(
			client,
			"%s: Message: mseq=0x%02x, command=%s, type=%s, size=%d",
			prefix, message->mseq,
			ipc_command_string(message->command),
			ipc_request_type_string(message->type),
			message->size);
#ifdef DEBUG
		if (message->size > 0) {
			ipc_client_log(
				client,
				"================================= IPC FMT data =================================");
			ipc_data_dump(client, (void *) message->data,
				      message->size > 0x100 ?
				      0x100 : message->size);
			ipc_client_log(
				client,
				"================================================================================");
		}
#endif
		break;
	case IPC_CLIENT_TYPE_RFS:
		ipc_client_log(client, "\n");
		ipc_client_log(client, "%s: Sent RFS message", prefix);
		ipc_client_log(client,
			       "%s: Message: mseq=0x%02x, command=%s, size=%d",
			       prefix, message->mseq,
			       ipc_command_string(message->command),
			       message->size);
#ifdef DEBUG
		if (message->size > 0) {
			ipc_client_log(
				client,
				"================================= IPC RFS data =================================");
			ipc_data_dump(client, (void *) message->data,
				      message->size > 0x100 ?
				      0x100 : message->size);
			ipc_client_log(
				client,
				"================================================================================");
		}
#endif
		break;
	}
}

void ipc_client_log_recv(struct ipc_client *client, struct ipc_message *message,
			 const char *prefix)
{
	if (client == NULL || message == NULL || prefix == NULL)
		return;

	switch (client->type) {
	case IPC_CLIENT_TYPE_FMT:
		ipc_client_log(client, "\n");
		ipc_client_log(client, "%s: Received FMT message", prefix);
		ipc_client_log(
			client,
			"%s: Message: aseq=0x%02x, command=%s, type=%s, size=%d",
			prefix, message->aseq,
			ipc_command_string(message->command),
			ipc_response_type_string(message->type),
			message->size);
#ifdef DEBUG
		if (message->size > 0) {
			ipc_client_log(
				client,
				"================================= IPC FMT data =================================");
			ipc_data_dump(client, (void *) message->data,
				      message->size > 0x100 ?
				      0x100 : message->size);
			ipc_client_log(
				client,
				"================================================================================");
		}
#endif
		break;
	case IPC_CLIENT_TYPE_RFS:
		ipc_client_log(client, "\n");
		ipc_client_log(client, "%s: Received RFS message", prefix);
		ipc_client_log(client,
			       "%s: Message: aseq=0x%02x, command=%s, size=%d",
			       prefix, message->aseq,
			       ipc_command_string(message->command),
			       message->size);
#ifdef DEBUG
		if (message->size > 0) {
			ipc_client_log(
				client,
				"================================= IPC RFS data =================================");
			ipc_data_dump(client, (void *) message->data,
				      message->size > 0x100 ?
				      0x100 : message->size);
			ipc_client_log(
				client,
				"================================================================================");
		}
#endif
		break;
	}
}

int ipc_fmt_header_setup(struct ipc_fmt_header *header,
			 const struct ipc_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(header, 0, sizeof(struct ipc_fmt_header));
	header->length = message->size + sizeof(struct ipc_fmt_header);
	header->mseq = message->mseq;
	header->aseq = message->aseq;
	header->group = IPC_GROUP(message->command);
	header->index = IPC_INDEX(message->command);
	header->type = message->type;

	return 0;
}

int ipc_fmt_message_setup(const struct ipc_fmt_header *header,
			  struct ipc_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(message, 0, sizeof(struct ipc_message));
	message->mseq = header->mseq;
	message->aseq = header->aseq;
	message->command = IPC_COMMAND(header->group, header->index);
	message->type = header->type;
	message->data = NULL;
	message->size = 0;

	return 0;
}

int ipc_rfs_header_setup(struct ipc_rfs_header *header,
			 const struct ipc_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(header, 0, sizeof(struct ipc_rfs_header));
	header->length = message->size + sizeof(struct ipc_rfs_header);
	header->id = message->mseq;
	header->index = IPC_INDEX(message->command);

	return 0;
}

int ipc_rfs_message_setup(const struct ipc_rfs_header *header,
			  struct ipc_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(message, 0, sizeof(struct ipc_message));
	message->aseq = header->id;
	message->command = IPC_COMMAND(IPC_GROUP_RFS, header->index);
	message->data = NULL;
	message->size = 0;

	return 0;
}
