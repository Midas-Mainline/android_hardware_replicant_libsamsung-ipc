/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <net/if.h>

#include "util.h"

void *file_data_read(char *path, int size, int chunk)
{
    void *data = NULL;
    int fd = -1;

    unsigned char *p;
    int count;
    int rc;

    if (path == NULL || size <= 0 || chunk <= 0)
        return NULL;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        goto error;

    data = malloc(size);
    memset(data, 0, size);

    p = (unsigned char *) data;

    count = 0;
    while (count < size) {
        rc = read(fd, p, size - count > chunk ? chunk : size - count);
        if (rc < 0)
            goto error;

        p += rc;
        count += rc;
    }

    goto complete;

error:
    if (data != NULL)
        free(data);
    data = NULL;

complete:
    if (fd >= 0)
        close(fd);

    return data;
}

int network_iface_up(char *iface, int domain, int type)
{
    struct ifreq ifr;
    int fd = -1;
    int rc;

    if (iface == NULL)
        return -1;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    fd = socket(domain, type, 0);
    if (fd < 0)
        goto error;

    rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (rc < 0)
        goto error;

    ifr.ifr_flags |= IFF_UP;

    rc = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if (rc < 0)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (fd >= 0)
        close(fd);

    return rc;
}

int network_iface_down(char *iface, int domain, int type)
{
    struct ifreq ifr;
    int fd = -1;
    int rc;

    if (iface == NULL)
        return -1;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    fd = socket(domain, type, 0);
    if (fd < 0)
        goto error;

    rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (rc < 0)
        goto error;

    ifr.ifr_flags = (ifr.ifr_flags & (~IFF_UP));

    rc = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if (rc < 0)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (fd >= 0)
        close(fd);

    return rc;
}

int sysfs_value_read(char *path)
{
    char buffer[100];
    int value;
    int fd = -1;
    int rc;

    if (path == NULL)
        return -1;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        goto error;

    rc = read(fd, &buffer, sizeof(buffer));
    if (rc <= 0)
        goto error;

    value = atoi(buffer);
    goto complete;

error:
    value = -1;

complete:
    if (fd >= 0)
        close(fd);

    return value;
}

int sysfs_value_write(char *path, int value)
{
    char buffer[100];
    int fd = -1;
    int rc;

    if (path == NULL)
        return -1;

    fd = open(path, O_WRONLY);
    if (fd < 0)
        goto error;

    snprintf((char *) &buffer, sizeof(buffer), "%d\n", value);

    rc = write(fd, buffer, strlen(buffer));
    if (rc < (int) strlen(buffer))
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (fd >= 0)
        close(fd);

    return rc;
}

int sysfs_string_read(char *path, char *buffer, int length)
{
    int fd = -1;
    int rc;

    if (path == NULL || buffer == NULL || length <= 0)
        return -1;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        goto error;

    rc = read(fd, buffer, length);
    if (rc <= 0)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (fd >= 0)
        close(fd);

    return rc;
}

int sysfs_string_write(char *path, char *buffer, int length)
{
    int fd = -1;
    int rc;

    if (path == NULL || buffer == NULL || length <= 0)
        return -1;

    fd = open(path, O_WRONLY);
    if (fd < 0)
        goto error;

    rc = write(fd, buffer, length);
    if (rc <= 0)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (fd >= 0)
        close(fd);

    return rc;
}

// vim:ts=4:sw=4:expandtab
