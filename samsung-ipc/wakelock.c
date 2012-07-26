/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <assert.h>
#include <fcntl.h>
#include <wakelock.h>

int wake_lock(char *lock_name) {
	int rc;
	assert(lock_name != NULL);
	
	int fd = open("/sys/power/wake_lock", O_RDWR);
	if (fd < 0) {
		return fd;
	}

	rc = write(fd, lock_name, strlen(lock_name));
	close(fd);

	return rc;
}

int wake_unlock(char *lock_name) {
	int rc;
	assert(lock_name != NULL);
	
	int fd = open("/sys/power/wake_unlock", O_RDWR);
	if (fd < 0) {
		return fd;
	}

	rc = write(fd, lock_name, strlen(lock_name));
	close(fd);

	return rc;
}
