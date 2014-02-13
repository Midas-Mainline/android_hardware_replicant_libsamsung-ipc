/*
 * This file is part of libsamsung-ipc.
 *
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
 *
 */

#ifndef __UTIL_H__
#define __UTIL_H__

void *file_data_read(const char *path, size_t size, size_t chunk_size,
    size_t offset);
int file_data_write(const char *path, const void *data, size_t size,
    size_t chunk_size, size_t offset);
int network_iface_up(const char *iface, int domain, int type);
int network_iface_down(const char *iface, int domain, int type);
int sysfs_value_read(const char *path);
int sysfs_value_write(const char *path, int value);
char *sysfs_string_read(const char *path, size_t length);
int sysfs_string_write(const char *path, const char *buffer, size_t length);
char *data2string(const void *data, size_t size);
void *string2data(const char *string, size_t *size_p);

#endif

// vim:ts=4:sw=4:expandtab
