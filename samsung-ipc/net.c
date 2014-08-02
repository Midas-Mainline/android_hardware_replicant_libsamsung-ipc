/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2014 Paul Kocialkowsk <contact@paulk.fr>
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

int ipc_net_plmn_sel_setup(struct ipc_net_plmn_sel_request_data *data,
    unsigned char mode_sel, const char *plmn, unsigned char act)
{
    size_t plmn_length;

    if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_net_plmn_sel_request_data));
    data->mode_sel = mode_sel;

    if (mode_sel == IPC_NET_PLMN_SEL_AUTO) {
        data->act = IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN;
    } else {
        plmn_length = strlen(plmn);
        if (plmn_length > sizeof(data->plmn))
            plmn_length = sizeof(data->plmn);

        memcpy((void *) data->plmn, (void *) plmn, plmn_length);

        // If there are less (5 is the usual case) PLMN bytes, fill the rest with '#'
        if (plmn_length < sizeof(data->plmn))
            memset((void *) ((unsigned char *) data->plmn + plmn_length), '#', sizeof(data->plmn) - plmn_length);

        data->act = act;
    }

    return 0;
}

int ipc_net_regist_setup(struct ipc_net_regist_request_data *data,
    unsigned char domain)
{
    if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_net_regist_request_data));
    data->act = IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN;
    data->domain = domain;

    return 0;
}

unsigned char ipc_net_plmn_list_count_extract(const void *data, size_t size)
{
    struct ipc_net_plmn_list_header *header;

    if (data == NULL || size < sizeof(struct ipc_net_plmn_list_header))
        return 0;

    header = (struct ipc_net_plmn_list_header *) data;

    return header->count;
}

struct ipc_net_plmn_list_entry *ipc_net_plmn_list_entry_extract(const void *data,
    size_t size, unsigned int index)
{
    struct ipc_net_plmn_list_entry *entry = NULL;
    unsigned char count;
    unsigned char i;
    unsigned int offset;

    if (data == NULL)
        return NULL;

    count = ipc_net_plmn_list_count_extract(data, size);
    if (count == 0 || index >= count)
        return NULL;

    offset = sizeof(struct ipc_net_plmn_list_header);

    for (i = 0; i < (index + 1); i++) {
        entry = (struct ipc_net_plmn_list_entry *) ((unsigned char *) data + offset);
        offset += sizeof(struct ipc_net_plmn_list_entry);
    }

    if (offset > size)
        return NULL;

    return entry;
}

// vim:ts=4:sw=4:expandtab
