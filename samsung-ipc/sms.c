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

size_t ipc_sms_send_msg_size_setup(struct ipc_sms_send_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size)
{
    size_t size;

	if (header == NULL || smsc == NULL || smsc_size == 0 || pdu == NULL || pdu_size == 0)
		return 0;

    size = sizeof(struct ipc_sms_send_msg_request_header) + sizeof(unsigned char) + smsc_size + pdu_size;

    return size;
}

void *ipc_sms_send_msg_setup(struct ipc_sms_send_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size)
{
    void *data;
    size_t size;
    unsigned char smsc_length;
    unsigned char *p;

	if (header == NULL || smsc == NULL || smsc_size == 0 || pdu == NULL || pdu_size == 0)
		return NULL;

    smsc_length = (unsigned char) smsc_size;

    header->length = (unsigned char) (sizeof(unsigned char) + smsc_size + pdu_size);

    size = ipc_sms_send_msg_size_setup(header, smsc, smsc_size, pdu, pdu_size);
    if (size == 0)
        return NULL;

    data = calloc(1, size);

    p = (unsigned char *) data;

    memcpy(p, header, sizeof(struct ipc_sms_send_msg_request_header));
    p += sizeof(struct ipc_sms_send_msg_request_header);

    memcpy(p, &smsc_length, sizeof(smsc_length));
    p += sizeof(smsc_length);

    memcpy(p, smsc, smsc_size);
    p += smsc_size;

    memcpy(p, pdu, pdu_size);
    p += pdu_size;

    return data;
}

size_t ipc_sms_incoming_msg_pdu_size_extract(const void *data, size_t size)
{
    struct ipc_sms_incoming_msg_header *header;

    header = (struct ipc_sms_incoming_msg_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_sms_incoming_msg_header))
        return 0;

    return (size_t) header->length;
}

void *ipc_sms_incoming_msg_pdu_extract(const void *data, size_t size)
{
    struct ipc_sms_incoming_msg_header *header;
    void *pdu;

    if (data == NULL || size < sizeof(struct ipc_sms_incoming_msg_header))
        return NULL;

    header = (struct ipc_sms_incoming_msg_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_sms_incoming_msg_header))
        return NULL;

    pdu = (void *) ((unsigned char *) data + sizeof(struct ipc_sms_incoming_msg_header));

    return pdu;
}

size_t ipc_sms_save_msg_size_setup(struct ipc_sms_save_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size)
{
    size_t size;

    if (header == NULL || pdu == NULL || pdu_size == 0)
        return 0;

    if (smsc == NULL)
        smsc_size = 0;

    size = sizeof(struct ipc_sms_save_msg_request_header) + sizeof(unsigned char) + smsc_size + pdu_size;

    return size;
}

void *ipc_sms_save_msg_setup(struct ipc_sms_save_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size)
{
    void *data;
    size_t size;
    unsigned char smsc_length;
    unsigned char *p;

    if (header == NULL || pdu == NULL || pdu_size == 0)
        return NULL;

    if (smsc == NULL)
        smsc_size = 0;

    smsc_length = (unsigned char) smsc_size;

    header->magic = 2;
    header->index = 12 - 1,
    header->length = (unsigned char) (sizeof(unsigned char) + smsc_size + pdu_size);

    size = ipc_sms_save_msg_size_setup(header, smsc, smsc_size, pdu, pdu_size);
    if (size == 0)
        return NULL;

    data = calloc(1, size);

    p = (unsigned char *) data;

    memcpy(p, header, sizeof(struct ipc_sms_save_msg_request_header));
    p += sizeof(struct ipc_sms_save_msg_request_header);

    memcpy(p, &smsc_length, sizeof(smsc_length));
    p += sizeof(smsc_length);

    if (smsc != NULL && smsc_size > 0) {
        memcpy(p, smsc, smsc_size);
        p += smsc_size;
    }

    memcpy(p, pdu, pdu_size);
    p += pdu_size;

    return data;
}

int ipc_sms_del_msg_setup(struct ipc_sms_del_msg_request_data *data,
    unsigned short index)
{
   if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_sms_del_msg_request_data));
    data->magic = 2;
    data->index = index;

    return 0;
}

size_t ipc_sms_svc_center_addr_smsc_size_extract(const void *data, size_t size)
{
    struct ipc_sms_svc_center_addr_header *header;

    if (data == NULL || size < sizeof(struct ipc_sms_svc_center_addr_header))
        return 0;

    header = (struct ipc_sms_svc_center_addr_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_sms_svc_center_addr_header))
        return 0;

    return (size_t) header->length;
}

void *ipc_sms_svc_center_addr_smsc_extract(const void *data, size_t size)
{
    struct ipc_sms_svc_center_addr_header *header;
    void *smsc;

    if (data == NULL || size < sizeof(struct ipc_sms_svc_center_addr_header))
        return NULL;

    header = (struct ipc_sms_svc_center_addr_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_sms_svc_center_addr_header))
        return NULL;

    smsc = (void *) ((unsigned char *) data + sizeof(struct ipc_sms_svc_center_addr_header));

    return smsc;
}

// vim:ts=4:sw=4:expandtab
