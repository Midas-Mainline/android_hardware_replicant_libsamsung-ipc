/*
 * This file is part of libsamsung-ipc.
 *
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
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <samsung-ipc.h>

#include <partitions/android/android.h>
#include "android.h"

static char const * const dummy_modem_image_paths[] = {
	/* We can't use mktemp here since everything is const
	 * echo libsamsung-ipc | sha1sum | cut -c-20
	 * gives 55f4731d2e11e85bd889
	 */
	"/tmp/libsamsung-ipc.55f4731d2e11e85bd889/modem.img",
	NULL
};


int create_dummy_modem_image(__attribute__((unused)) struct ipc_client *client,
			     __attribute__((unused)) const char * const path)
{
	/* TODO: replace it by C code but make sure that the replacement code
	 * is as robust as the shell commands
	 */
	system("mkdir -p /tmp/libsamsung-ipc.55f4731d2e11e85bd889/");
	system("touch /tmp/libsamsung-ipc.55f4731d2e11e85bd889/modem.img");

	return 0;
}

int test_open_android_modem_partition(struct ipc_client *client)
{
	int i;
	int rc;

	for (i = 0; dummy_modem_image_paths[i] != NULL; i++) {
		rc = create_dummy_modem_image(client,
					      dummy_modem_image_paths[i]);
		if (rc == -1) {
			ipc_client_log(
				client,
				"%s: create_dummy_modem_image(client, %s)"
				" failed\n",
				__func__, dummy_modem_image_paths[i]);
			return -1;
		}
	}

	rc = open_android_modem_partition(client, dummy_modem_image_paths);
	if (rc == -1) {
		rc = errno;
		ipc_client_log(client, "%s: open_android_modem_partition"
			       " failed with errror %d: %s\n",
			       __func__, rc, strerror(rc));
		errno = rc;
		return -1;
	}

	rc = close(rc);
	if (rc == -1) {
		rc = errno;
		ipc_client_log(client,
			       "%s: close() failed with errror %d: %s\n",
			       __func__, rc, strerror(rc));
		return -1;
	}

	return 0;
}
