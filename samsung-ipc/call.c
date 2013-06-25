/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowsk <contact@paulk.fr>
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
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

#include <stdlib.h>
#include <string.h>

#include <samsung-ipc.h>

#define OUTGOING_NUMBER_MAX_LENGTH 86

void ipc_call_outgoing_setup(struct ipc_call_outgoing *message, unsigned char type,
    unsigned char identity, unsigned char prefix, char *number)
{
    int length;

    if (message == NULL || number == NULL)
        return;

    length = strlen(number);
    if (length > OUTGOING_NUMBER_MAX_LENGTH)
        length = OUTGOING_NUMBER_MAX_LENGTH;

    memset(message, 0, sizeof(struct ipc_call_outgoing));

    message->type = type;
    message->identity = identity;
    message->prefix = prefix;
    message->length = length;

    strncpy((char *) message->number, number, length);
}

/* Retrieve number of calls in list of calls */
unsigned int ipc_call_list_response_get_num_entries(struct ipc_message_info *response)
{
    if (response == NULL || response->data == NULL || response->length < sizeof(unsigned int))
        return 0;

    return *((unsigned int *) response->data);
}

/* Retrieve one specific entry from a list of calls */
struct ipc_call_list_entry* ipc_call_list_response_get_entry(struct ipc_message_info *response,
    unsigned int num)
{
    unsigned int count, pos, n;
    struct ipc_call_list_entry *entry = NULL;

    count = ipc_call_list_response_get_num_entries(response);
    if (num > count || count == 0)
        return NULL;

    pos = 1;
    for (n = 0; n < num + 1; n++)
    {
        entry = (struct ipc_call_list_entry *) (response->data + pos);
        pos += (unsigned int) (sizeof(struct ipc_call_list_entry) + entry->number_len);
    }

    return entry;
}

/* Retrieve the number of a call entry in the list of calls */
char *ipc_call_list_response_get_entry_number(struct ipc_message_info *response,
    unsigned int num)
{
    unsigned int count, pos, n;
    struct ipc_call_list_entry *entry = NULL;
    char *number;

    count = ipc_call_list_response_get_num_entries(response);
    if (num > count || count == 0)
        return NULL;

    pos = 1;
    for (n = 0; n < num + 1; n++)
    {
        if (entry != NULL)
            pos += entry->number_len;

        entry = (struct ipc_call_list_entry *) (response->data + pos);
        pos += (unsigned int) sizeof(struct ipc_call_list_entry);
    }

    if (entry == NULL || (unsigned char *) (response->data + pos) == NULL)
        return NULL;

    number = (char *) malloc(sizeof(char) * entry->number_len);
    strncpy(number, (char *) (response->data + pos), entry->number_len);

    return number;
}

unsigned char *ipc_call_cont_dtmf_burst_pack(struct ipc_call_cont_dtmf *message,
    unsigned char *burst, int burst_len)
{
    unsigned char *data = NULL;
    int data_len = sizeof(struct ipc_call_cont_dtmf) + burst_len;

    if (message == NULL || burst == NULL || burst_len <= 0)
        return NULL;

    data = (unsigned char *) malloc(sizeof(unsigned char) * data_len);
    memset(data, 0, data_len);

    memcpy(data, message, sizeof(struct ipc_call_cont_dtmf));
    memcpy(data + sizeof(struct ipc_call_cont_dtmf), burst, burst_len);

    return data;
}

// vim:ts=4:sw=4:expandtab
