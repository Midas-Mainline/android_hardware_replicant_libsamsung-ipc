/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2017 Wolfgang Wiedmeyer <wolfgit@wiedmeyer.de>
 * Copyright (C) 2020 Tony Garnock-Jones <tonyg@leastfixedpoint.com>
 * Copyright (C) 2021 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <samsung-ipc.h>

static char const * const partitions_dirnames[] = {
	"/dev/disk/by-partlabel/", /* GNU/Linux */
	"/dev/block/by-name/",     /* Android */
	NULL
};

int open_android_modem_partition(struct ipc_client *client,
				 char const * const *path_names)
{
	int i;

	for (i = 0; path_names[i] != NULL; i++) {
		char const * const path = path_names[i];
		int fd;

		ipc_client_log(client, "%s: Trying to open %s",
			       __func__, path);

		fd = open(path, O_RDONLY);
		if (fd == -1) {
			if (errno == ENOENT)
				continue;
			/* Normally errno should be passed to the caller here */
			return -1;
		}
		return fd;
	}

	errno = ENOENT;
	return -1;
}

int open_android_modem_partition_by_name(struct ipc_client *client,
					 const char *name, char **out_path)
{
	int i;
	int rc;

	for (i = 0; partitions_dirnames[i] != NULL; i++) {
		char *path = NULL;
		int fd;
		size_t len;

		len = strlen(partitions_dirnames[i]) + strlen(name) + 1;
		path = calloc(1, len);
		if (path == NULL) {
			rc = errno;
			ipc_client_log(client,
				       "%s: calloc failed with error %d: %s",
				       __func__, rc, strerror(rc));
			return -errno;
		}

		strncpy(path, partitions_dirnames[i],
			strlen(partitions_dirnames[i]));
		strcat(path, name);

		ipc_client_log(client, "%s: Trying to open %s",
			       __func__, path);

		fd = open(path, O_RDONLY);
		if (fd == -1) {
			rc = -errno;
			if (out_path)
				*out_path = path;
			else
				free(path);

			if (rc == -ENOENT)
				continue;

			errno = -rc;
			return -1;
		}

		if (out_path)
			*out_path = path;
		else
			free(path);

		return fd;
	}

	errno = ENOENT;
	return -1;
}
