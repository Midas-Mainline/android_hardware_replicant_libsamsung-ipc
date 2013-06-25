/*
 * This file is part of libsamsung-ipc.
 *
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

void ipc_gprs_port_list_setup(struct ipc_gprs_port_list *message)
{
    // FIXME: These are only known-to-work values used on most devices
    unsigned char bytes[] = {
        0x02, 0x04, 0x16, 0x00, 0x17, 0x00, 0x87, 0x00, 0xBD, 0x01
    };

    if (message == NULL)
        return;

    memset(message, 0, sizeof(struct ipc_gprs_port_list));

    memcpy(message->unk, bytes, sizeof(bytes));
}

void ipc_gprs_define_pdp_context_setup(struct ipc_gprs_define_pdp_context *message,
    unsigned char cid, int enable, char *apn)
{
    if (message == NULL)
        return;

    memset(message, 0, sizeof(struct ipc_gprs_define_pdp_context));

    message->enable = enable ? 1 : 0;
    message->cid = cid;
    message->unk = 0x2;

    strncpy((char *) message->apn, apn, 124);
}

void ipc_gprs_pdp_context_setup(struct ipc_gprs_pdp_context_set *message,
    unsigned char cid, int enable, char *username, char *password)
{
    if (message == NULL)
        return;

    memset(message, 0, sizeof(struct ipc_gprs_pdp_context_set));

    message->enable = enable ? 1 : 0;
    message->cid = cid;

    if (enable && username != NULL && password != NULL)
    {
        message->unk0[2] = 0x13;
        message->unk2 = 0x1;
        strncpy((char *) message->username, username, 32);
        strncpy((char *) message->password, password, 32);
    }
}

// vim:ts=4:sw=4:expandtab
