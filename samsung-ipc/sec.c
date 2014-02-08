/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
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

void ipc_sec_pin_status_setup(struct ipc_sec_pin_status_request_data *message,
    unsigned char pin_type, char *pin1, char *pin2)
{
    if (message == NULL)
        return;

    memset(message, 0, sizeof(struct ipc_sec_pin_status_request_data));

    message->type = pin_type;

    if (pin1 != NULL)
    {
        strncpy((char *) message->pin1, pin1, 8);
        message->pin1_length = strlen(pin1);
    }

    if (pin2 != NULL)
    {
        strncpy((char *) message->pin2, pin2, 8);
        message->pin2_length = strlen(pin2);
    }
}

void ipc_sec_lock_infomation_setup(struct ipc_sec_lock_infomation_request_data *message,
    unsigned char pin_type)
{
    if (message == NULL)
        return;

    message->magic = 1;
    message->type = pin_type;
}

char *ipc_sec_rsim_access_response_get_file_data(struct ipc_message_info *response)
{
    int n = 0;
    int offset = (int) sizeof(struct ipc_sec_rsim_access_response_header);
    int size = 0;

    if (response == NULL)
        return NULL;

    struct ipc_sec_rsim_access_response_header *rsimresp = (struct ipc_sec_rsim_access_response_header*) response->data;
    char *file_data = (char *) malloc(sizeof(char) * rsimresp->length);

    for (n = 0; n < rsimresp->length; n++)
    {
        if (response->data[offset + n] == 0x0)
            continue;
        else if (response->data[offset + n] == 0xff)
            break;
        else {
            file_data[size] = response->data[offset + n];
            size++;
        }
    }

    if (size < rsimresp->length)
        file_data = (char *) realloc(file_data, sizeof(char) * size);

    return file_data;
}

void ipc_sec_phone_lock_request_set_setup(struct ipc_sec_phone_lock_request_set_data *message,
    int pin_type, int enable, char *passwd)
{
    message->facility_type = pin_type;
    message->active = enable ? 1 : 0;

    if (passwd != NULL)
    {
        strncpy((char *) message->password, passwd, 39);
        message->password_length = strlen(passwd);
    }
}

void ipc_sec_change_locking_pw_setup(struct ipc_sec_change_locking_pw_data *message,
    int type, char *passwd_old, char *passwd_new)
{
    message->facility_type = type;

    if (passwd_old != NULL)
    {
        strncpy((char *) message->password_old, passwd_old, 39);
        message->password_old_length = strlen(passwd_old);
    }

    if (passwd_new != NULL)
    {
        strncpy((char *) message->password_new, passwd_new, 39);
        message->password_new_length = strlen(passwd_new);
    }
}

// vim:ts=4:sw=4:expandtab
