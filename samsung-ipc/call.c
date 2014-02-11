/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2013-2014 Paul Kocialkowsk <contact@paulk.fr>
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

int ipc_call_outgoing_setup(struct ipc_call_outgoing_data *data,
    unsigned char type, unsigned char identity, unsigned char prefix,
    const char *number)
{
    size_t number_length;

    if (data == NULL || number == NULL)
        return -1;

    number_length = strlen(number);
    if (number_length > sizeof(data->number))
        number_length = sizeof(data->number);

    memset(data, 0, sizeof(struct ipc_call_outgoing_data));
    data->type = type;
    data->identity = identity;
    data->prefix = prefix;
    data->number_length = (unsigned char) number_length;

    strncpy((char *) data->number, number, number_length);

    return 0;
}

unsigned char ipc_call_list_count_extract(const void *data, size_t size)
{
    struct ipc_call_list_header *header;

    if (data == NULL || size < sizeof(struct ipc_call_list_header))
        return 0;

    header = (struct ipc_call_list_header *) data;

    return header->count;
}

struct ipc_call_list_entry *ipc_call_list_entry_extract(const void *data,
    size_t size, unsigned int index)
{
    struct ipc_call_list_entry *entry = NULL;
    unsigned char count;
    unsigned char i;
    size_t offset;

    if (data == NULL)
        return NULL;

    count = ipc_call_list_count_extract(data, size);
    if (count == 0)
        return NULL;

    offset = sizeof(struct ipc_call_list_header);

    for (i = 0; i < (index + 1); i++) {
        entry = (struct ipc_call_list_entry *) ((unsigned char *) data + offset);
        offset += sizeof(struct ipc_call_list_entry) + entry->number_length;
    }

    if (offset > size)
        return NULL;

    return entry;
}

char *ipc_call_list_entry_number_extract(const struct ipc_call_list_entry *entry)
{
    char *number;
    size_t number_length;

    if (entry == NULL)
        return NULL;

    // entry->number_length doesn't count the final null character
    number_length = entry->number_length + sizeof(char);

    number = (char *) calloc(1, number_length);

    strncpy(number, (char *) entry + sizeof(struct ipc_call_list_entry), entry->number_length);
    number[entry->number_length] = '\0';

    return number;
}

void *ipc_call_burst_dtmf_setup(const struct ipc_call_burst_dtmf_entry *entries,
    unsigned char count)
{
    struct ipc_call_burst_dtmf_header *header;
    void *data;
    size_t size;

    if (entries == NULL)
        return NULL;

    size = sizeof(struct ipc_call_burst_dtmf_header) + count * sizeof(struct ipc_call_burst_dtmf_entry);

    data = calloc(1, size);

    header = (struct ipc_call_burst_dtmf_header *) data;
    header->count = count;

    memcpy((void *) ((unsigned char *) data + sizeof(struct ipc_call_burst_dtmf_header)), entries, count * sizeof(struct ipc_call_burst_dtmf_entry));

    return data;
}

// vim:ts=4:sw=4:expandtab
