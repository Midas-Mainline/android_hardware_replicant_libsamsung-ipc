/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
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
 *
 */

#include <assert.h>
#include <fcntl.h>
#include <wakelock.h>
#include <stdlib.h>
#include <string.h>

static int wake_lock_fd = -1;
static int wake_unlock_fd = -1;

int wake_lock(char *lock_name)
{
    int rc;
    assert(lock_name != NULL);

    if (wake_lock_fd < 0)
        wake_lock_fd = open("/sys/power/wake_lock", O_RDWR);

    if (wake_lock_fd < 0)
        return wake_lock_fd;

    rc = write(wake_lock_fd, lock_name, strlen(lock_name));

    return rc;
}

int wake_unlock(char *lock_name)
{
    int rc;
    assert(lock_name != NULL);

    if (wake_unlock_fd < 0)
        wake_unlock_fd = open("/sys/power/wake_unlock", O_RDWR);

    if (wake_unlock_fd < 0)
        return wake_lock_fd;

    rc = write(wake_unlock_fd, lock_name, strlen(lock_name));

    return rc;
}
