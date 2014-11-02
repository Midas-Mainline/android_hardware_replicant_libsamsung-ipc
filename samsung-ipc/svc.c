/*
 * This file is part of libsamsung-ipc.
 *
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

int ipc_svc_enter_setup(struct ipc_svc_enter_data *data,
    unsigned char mode, unsigned char type)
{
   if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_svc_enter_data));
    data->mode = mode;
    data->type = type;

    if (mode == IPC_SVC_MODE_MONITOR)
        data->magic = 0x00;
    else
        data->magic = 0x10;

    return 0;
}

unsigned char ipc_svc_display_screen_count_extract(const void *data, size_t size)
{
    struct ipc_svc_display_screen_header *header;

    if (data == NULL || size < sizeof(struct ipc_svc_display_screen_header))
        return 0;

    header = (struct ipc_svc_display_screen_header *) data;

    return header->count;
}

struct ipc_svc_display_screen_entry *ipc_svc_display_screen_extract(const void *data,
    size_t size, unsigned int index)
{
    struct ipc_svc_display_screen_entry *entry = NULL;
    unsigned char count;
    unsigned char i;
    unsigned int offset;

    if (data == NULL)
        return NULL;

    count = ipc_svc_display_screen_count_extract(data, size);
    if (count == 0 || index >= count)
        return NULL;

    offset = sizeof(struct ipc_svc_display_screen_header);

    for (i = 0; i < (index + 1); i++) {
        entry = (struct ipc_svc_display_screen_entry *) ((unsigned char *) data + offset);
        offset += sizeof(struct ipc_svc_display_screen_entry);
    }

    if (offset > size)
        return NULL;

    return entry;
}

// vim:ts=4:sw=4:expandtab
