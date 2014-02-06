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

#ifndef __SAMSUNG_IPC_CALL_H__
#define __SAMSUNG_IPC_CALL_H__

/*
 * Commands
 */

#define IPC_CALL_OUTGOING                                       0x0201
#define IPC_CALL_INCOMING                                       0x0202
#define IPC_CALL_RELEASE                                        0x0203
#define IPC_CALL_ANSWER                                         0x0204
#define IPC_CALL_STATUS                                         0x0205
#define IPC_CALL_LIST                                           0x0206
#define IPC_CALL_BURST_DTMF                                     0x0207
#define IPC_CALL_CONT_DTMF                                      0x0208
#define IPC_CALL_WAITING                                        0x0209
#define IPC_CALL_LINE_ID                                        0x020A

/*
 * Values
 */

#define IPC_CALL_TYPE_VOICE                                     0x01
#define IPC_CALL_TYPE_DATA                                      0x03

#define IPC_CALL_IDENTITY_DEFAULT                               0x00
#define IPC_CALL_IDENTITY_HIDE                                  0x01
#define IPC_CALL_IDENTITY_SHOW                                  0x02

#define IPC_CALL_PREFIX_NONE                                    0x00
#define IPC_CALL_PREFIX_INTL                                    0x11

#define IPC_CALL_STATUS_DIALING                                 0x01
#define IPC_CALL_STATUS_IGNORING_INCOMING_STATUS                0x02
#define IPC_CALL_STATUS_CONNECTED                               0x03
#define IPC_CALL_STATUS_RELEASED                                0x04
#define IPC_CALL_STATUS_CONNECTING                              0x05

#define IPC_CALL_END_CAUSE_NORMAL                               0x05
#define IPC_CALL_END_CAUSE_REJECTED                             0x2F
#define IPC_CALL_END_CAUSE_UNSPECIFIED                          0x10

#define IPC_CALL_TERM_MO                                        0x01
#define IPC_CALL_TERM_MT                                        0x02

#define IPC_CALL_LIST_ENTRY_STATUS_ACTIVE                       0x01
#define IPC_CALL_LIST_ENTRY_STATUS_HOLDING                      0x02
#define IPC_CALL_LIST_ENTRY_STATUS_DIALING                      0x03
#define IPC_CALL_LIST_ENTRY_STATUS_ALERTING                     0x04
#define IPC_CALL_LIST_ENTRY_STATUS_INCOMING                     0x05
#define IPC_CALL_LIST_ENTRY_STATUS_WAITING                      0x06

#define IPC_CALL_DTMF_STATE_START                               0x01
#define IPC_CALL_DTMF_STATE_STOP                                0x02

/*
 * Structures
 */

struct ipc_call_outgoing_data {
    unsigned char unknown;
    unsigned char type; // IPC_CALL_TYPE
    unsigned char identity; // IPC_CALL_IDENTITY
    unsigned char number_length;
    unsigned char prefix; // IPC_CALL_PREFIX
    unsigned char number[86];
} __attribute__((__packed__));

struct ipc_call_incoming_data {
    unsigned char unknown;
    unsigned char type; // IPC_CALL_TYPE_...
    unsigned char id;
    unsigned char line;
} __attribute__((__packed__));

struct ipc_call_status_data {
    unsigned char unknown;
    unsigned char type; // IPC_CALL_TYPE
    unsigned char id;
    unsigned char status; // IPC_CALL_STATUS
    unsigned char reason;
    unsigned char end_cause; // IPC_CALL_END_CAUSE
} __attribute__((__packed__));

struct ipc_call_list_entry {
    unsigned char unknown1;
    unsigned char type; // IPC_CALL_TYPE
    unsigned char id;
    unsigned char term; // IPC_CALL_TERM
    unsigned char status; // IPC_CALL_LIST_ENTRY_STATUS
    unsigned char unknown2;
    unsigned char number_length;
    unsigned char unknown3;
} __attribute__((__packed__));

struct ipc_call_cont_dtmf_data {
    unsigned char state;
    unsigned char tone;
} __attribute__((__packed__));

/*
 * Helpers
 */

void ipc_call_outgoing_setup(struct ipc_call_outgoing_data *message, unsigned char type,
    unsigned char identity, unsigned char prefix, char *number);
unsigned int ipc_call_list_response_get_num_entries(struct ipc_message_info *response);
struct ipc_call_list_entry* ipc_call_list_response_get_entry(struct ipc_message_info *response,
    unsigned int num);
char *ipc_call_list_response_get_entry_number(struct ipc_message_info *response,
    unsigned int num);
unsigned char *ipc_call_cont_dtmf_burst_pack(struct ipc_call_cont_dtmf_data *message,
    unsigned char *burst, int burst_len);

#endif

// vim:ts=4:sw=4:expandtab
