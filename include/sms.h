/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_SMS_H__
#define __SAMSUNG_IPC_SMS_H__

/*
 * Commands
 */

#define IPC_SMS_SEND_MSG                                        0x0401
#define IPC_SMS_INCOMING_MSG                                    0x0402
#define IPC_SMS_READ_MSG                                        0x0403
#define IPC_SMS_SAVE_MSG                                        0x0404
#define IPC_SMS_DEL_MSG                                         0x0405
#define IPC_SMS_DELIVER_REPORT                                  0x0406
#define IPC_SMS_DEVICE_READY                                    0x0407
#define IPC_SMS_SEL_MEM                                         0x0408
#define IPC_SMS_STORED_MSG_COUNT                                0x0409
#define IPC_SMS_SVC_CENTER_ADDR                                 0x040A
#define IPC_SMS_SVC_OPTION                                      0x040B
#define IPC_SMS_MEM_STATUS                                      0x040C
#define IPC_SMS_CBS_MSG                                         0x040D
#define IPC_SMS_CBS_CFG                                         0x040E
#define IPC_SMS_STORED_MSG_STATUS                               0x040F
#define IPC_SMS_PARAM_COUNT                                     0x0410
#define IPC_SMS_PARAM                                           0x0411

/*
 * Values
 */

#define IPC_SMS_TYPE_POINT_TO_POINT                             0x01
#define IPC_SMS_TYPE_STATUS_REPORT                              0x02
#define IPC_SMS_TYPE_OUTGOING                                   0x02

#define IPC_SMS_MSG_TYPE_MULTIPLE                               0x01
#define IPC_SMS_MSG_TYPE_SINGLE                                 0x02

#define IPC_SMS_ACK_NO_ERROR                                    0x0000
#define IPC_SMS_ACK_PDA_FULL_ERROR                              0x8080
#define IPC_SMS_ACK_MALFORMED_REQ_ERROR                         0x8061
#define IPC_SMS_ACK_UNSPEC_ERROR                                0x806F

#define IPC_SMS_STATUS_REC_UNREAD                               0x01
#define IPC_SMS_STATUS_REC_READ                                 0x02
#define IPC_SMS_STATUS_STO_UNSENT                               0x03
#define IPC_SMS_STATUS_STO_SENT                                 0x04

/*
 * Structures
 */

struct ipc_sms_send_msg_request_header {
    unsigned char type; // IPC_SMS_TYPE
    unsigned char msg_type; // IPC_SMS_MSG_TYPE
    unsigned char unknown;
    unsigned char length;
} __attribute__((__packed__));

struct ipc_sms_send_msg_response_data {
    unsigned char type; // IPC_SMS_TYPE
    unsigned short ack; // IPC_SMS_ACK
    unsigned char id;
    unsigned char unknown;
} __attribute__((__packed__));

struct ipc_sms_incoming_msg_header {
    unsigned char msg_type; // IPC_SMS_MSG_TYPE
    unsigned char type; // IPC_SMS_TYPE
    unsigned short sim_index;
    unsigned char id;
    unsigned char length;
} __attribute__((__packed__));

struct ipc_sms_save_msg_request_header {
    unsigned char magic;
    unsigned short index;
    unsigned char status; // IPC_SMS_STATUS
    unsigned char length;
} __attribute__((__packed__));

struct ipc_sms_save_msg_response_data {
    unsigned char magic;
    unsigned short error;
    unsigned short index;
} __attribute__((__packed__));

struct ipc_sms_del_msg_request_data {
    unsigned char magic;
    unsigned short index;
} __attribute__((__packed__));

struct ipc_sms_del_msg_response_data {
    unsigned char magic;
    unsigned short error;
    unsigned short index;
} __attribute__((__packed__));

struct ipc_sms_deliver_report_request_data {
    unsigned char type; // IPC_SMS_TYPE
    unsigned short ack; // IPC_SMS_ACK
    unsigned char id;
    unsigned char unknown;
} __attribute__((__packed__));

struct ipc_sms_deliver_report_response_data {
    unsigned short ack; // IPC_SMS_ACK
} __attribute__((__packed__));

struct ipc_sms_svc_center_addr_header {
    unsigned char length;
} __attribute__((__packed__));

/*
 * Helpers
 */

void *ipc_sms_send_msg_setup(struct ipc_sms_send_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size);
char *ipc_sms_incoming_msg_pdu_extract(const void *data, size_t size);
void *ipc_sms_save_msg_setup(struct ipc_sms_save_msg_request_header *header,
    const void *smsc, size_t smsc_size, const void *pdu, size_t pdu_size);
int ipc_sms_del_msg_setup(struct ipc_sms_del_msg_request_data *data,
    unsigned short index);

#endif

// vim:ts=4:sw=4:expandtab
