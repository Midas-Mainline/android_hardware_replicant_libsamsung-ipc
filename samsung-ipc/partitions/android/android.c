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

#include <sys/stat.h>
#include <sys/types.h>

#include <samsung-ipc.h>

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
