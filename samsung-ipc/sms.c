/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
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
#include <utils.h>

void *ipc_sms_send_msg_setup(struct ipc_sms_send_msg_request_header *header,
    const char *smsc, const char *pdu)
{
    void *data;
    size_t size;
    unsigned char smsc_length;
    unsigned char *p;

    if (header == NULL || smsc == NULL || pdu == NULL)
        return NULL;

    smsc_length = (unsigned char) strlen(smsc);

    size = sizeof(struct ipc_sms_send_msg_request_header) + sizeof(smsc_length) + strlen(smsc) + strlen(pdu);
    header->length = (unsigned char) size;

    data = calloc(1, size);

    p = (unsigned char *) data;

    memcpy(p, header, sizeof(struct ipc_sms_send_msg_request_header));
    p += sizeof(struct ipc_sms_send_msg_request_header);

    memcpy(p, &smsc_length, sizeof(smsc_length));
    p += sizeof(smsc_length);

    memcpy(p, smsc, smsc_length);
    p += smsc_length;

    memcpy(p, pdu, strlen(pdu));
    p += strlen(pdu);

    return data;
}

char *ipc_sms_incoming_msg_pdu_extract(const void *data, size_t size)
{
    struct ipc_sms_incoming_msg_header *header;
    char *string;
    void *pdu;

    if (data == NULL || size < sizeof(struct ipc_sms_incoming_msg_header))
        return NULL;

    header = (struct ipc_sms_incoming_msg_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_sms_incoming_msg_header))
        return NULL;

    pdu = (void *) ((unsigned char *) data + sizeof(struct ipc_sms_incoming_msg_header));

    string = data2string(pdu, header->length);

    return string;
}

// vim:ts=4:sw=4:expandtab
