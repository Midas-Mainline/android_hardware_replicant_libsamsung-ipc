/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

void *file_data_read(char *file_name, int size, int block_size);
int network_iface_up(char *iface, int domain, int type);
int network_iface_down(char *iface, int domain, int type);
int sysfs_value_read(char *path);
int sysfs_value_write(char *path, int value);
int sysfs_string_read(char *path, char *buffer, int length);
int sysfs_string_write(char *path, char *buffer, int length);

#endif

// vim:ts=4:sw=4:expandtab
