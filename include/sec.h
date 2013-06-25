/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_SEC_H__
#define __SAMSUNG_IPC_SEC_H__

/*
 * Types
 */

/* Possible SEC related IPC message types */
#define IPC_SEC_SIM_STATUS                                      0x0501
#define IPC_SEC_PHONE_LOCK                                      0x0502
#define IPC_SEC_CHANGE_LOCKING_PW                               0x0503
#define IPC_SEC_SIM_LANG                                        0x0504
#define IPC_SEC_RSIM_ACCESS                                     0x0505
#define IPC_SEC_GSIM_ACCESS                                     0x0506
#define IPC_SEC_SIM_ICC_TYPE                                    0x0507
#define IPC_SEC_LOCK_INFO                                       0x0508
#define IPC_SEC_ISIM_AUTH                                       0x0509

/*
 * Values
 */

/* SIM status */
#define IPC_SEC_SIM_STATUS_READY                                0x00
#define IPC_SEC_SIM_STATUS_SIM_LOCK_REQUIRED                    0x01
#define IPC_SEC_SIM_STATUS_INSIDE_PF_ERROR                      0x02
#define IPC_SEC_SIM_STATUS_LOCK_SC                              0x03
#define IPC_SEC_SIM_STATUS_LOCK_FD                              0x04
#define IPC_SEC_SIM_STATUS_LOCK_PN                              0x05
#define IPC_SEC_SIM_STATUS_LOCK_PU                              0x06
#define IPC_SEC_SIM_STATUS_LOCK_PP                              0x07
#define IPC_SEC_SIM_STATUS_LOCK_PC                              0x08
#define IPC_SEC_SIM_STATUS_CARD_NOT_PRESENT                     0x80
#define IPC_SEC_SIM_STATUS_CARD_ERROR                           0x81
#define IPC_SEC_SIM_STATUS_INIT_COMPLETE                        0x82
#define IPC_SEC_SIM_STATUS_PB_INIT_COMPLETE                     0x83

/* SIM/Network facility types */
#define IPC_SEC_FACILITY_TYPE_SC                                0x03
#define IPC_SEC_FACILITY_TYPE_FD                                0x04
#define IPC_SEC_FACILITY_TYPE_PN                                0x05
#define IPC_SEC_FACILITY_TYPE_PU                                0x06
#define IPC_SEC_FACILITY_TYPE_PP                                0x07
#define IPC_SEC_FACILITY_TYPE_PC                                0x08

/* SIM card (SC) facility lock types */
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_UNLOCKED                  0x00
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ                  0x01
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ                   0x02
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED              0x05

/* PIN type */
#define IPC_SEC_PIN_TYPE_PIN1                                   0x03
#define IPC_SEC_PIN_TYPE_PIN2                                   0x09

/* Type of the used SIM card */
#define IPC_SEC_SIM_CARD_TYPE_UNKNOWN                           0x00
#define IPC_SEC_SIM_CARD_TYPE_SIM                               0x01
#define IPC_SEC_SIM_CARD_TYPE_USIM                              0x02

/* Possible RSIM commads (see TS 27.00.1 8.18) */
#define IPC_SEC_RSIM_COMMAND_READ_BINARY                        0xb0
#define IPC_SEC_RSIM_COMMAND_READ_RECORD                        0xb2
#define IPC_SEC_RSIM_COMMAND_GET_RESPONSE                       0xc0
#define IPC_SEC_RSIM_COMMAND_UPDATE_BINARY                      0xd6
#define IPC_SEC_RSIM_COMMAND_UPDATE_RECORD                      0xdc
#define IPC_SEC_RSIM_COMMAND_STATUS                             0xf2
#define IPC_SEC_RSIM_COMMAND_RETRIEVE_DATA                      0xcb
#define IPC_SEC_RSIM_COMMAND_SET_DATA                           0xdb

#define IPC_SEC_RSIM_FILE_STRUCTURE_TRANSPARENT                 0x83
#define IPC_SEC_RSIM_FILE_STRUCTURE_LINEAR_FIXED                0x00

/*
 * Structures
 */

struct ipc_sec_sim_status_response {
    /* IPC_SEC_SIM_STATUS_... */
    unsigned char status;
    /* IPC_SEC_FACILITY_LOCK_TYPE_... */
    unsigned char facility_lock;
} __attribute__((__packed__));

struct ipc_sec_pin_status_set {
    /* IPC_SEC_SIM_STATUS_... */
    unsigned char type;
    unsigned char length1;
    unsigned char length2;
    unsigned char pin1[8];
    unsigned char pin2[8];
} __attribute__((__packed__));

struct ipc_sec_phone_lock_set {
    /* IPC_SEC_SIM_STATUS_... */
    unsigned char type;
    unsigned char lock; /* 1: lock, 0: unlock */
    unsigned char length;
    unsigned char password[39];
}  __attribute__((__packed__));

struct ipc_sec_phone_lock_get {
    /* IPC_SEC_FACILITY_TYPE_... */
    unsigned char facility;
}  __attribute__((__packed__));

struct ipc_sec_phone_lock_response {
    /* IPC_SEC_FACILITY_TYPE_... */
    unsigned char facility;
    unsigned char status; /* 1: active, 0: not active */
} __attribute__((__packed__));

struct ipc_sec_change_locking_pw_set {
    /* IPC_SEC_FACILITY_TYPE_... */
    unsigned char facility;
    unsigned char length_old;
    unsigned char length_new;
    unsigned char password_old[39];
    unsigned char password_new[39];
}  __attribute__((__packed__));

struct ipc_sec_rsim_access_get {
    /* IPC_SEC_RSIM_COMMAND_... */
    unsigned char command;
    unsigned short fileid;
    unsigned char p1, p2, p3;
} __attribute__((__packed__));

struct ipc_sec_rsim_access_response {
    unsigned char sw1, sw2;
    unsigned char len;
} __attribute__((__packed__));

// This is the data structure for SIM ICC type != 1
struct ipc_sec_rsim_access_response_data {
	unsigned char unk1[3];
    unsigned char offset;
    unsigned char unk2[2];
	unsigned char file_structure;
	unsigned char record_length;
} __attribute__((__packed__));

struct ipc_sec_lock_info_get {
    unsigned char unk0; /* needs to be "1" */
    /* IPC_SEC_PIN_TYPE_... */
    unsigned char pin_type;
} __attribute__((__packed__));

struct ipc_sec_lock_info_response {
    unsigned char num;
    /* IPC_SEC_PIN_TYPE_... */
    unsigned char type;
    unsigned char key;
    unsigned char attempts;
} __attribute__((__packed__));

struct ipc_sec_sim_icc_type {
    unsigned char type;
} __attribute__((__packed__));

/*
 * Helpers
 */

void ipc_sec_pin_status_set_setup(struct ipc_sec_pin_status_set *message,
    unsigned char pin_type, char *pin1, char *pin2);
void ipc_sec_lock_info_get_setup(struct ipc_sec_lock_info_get *message,
    unsigned char pin_type);
void ipc_sec_phone_lock_set_setup(struct ipc_sec_phone_lock_set *message,
    int pin_type, int enable, char *passwd);
char *ipc_sec_rsim_access_response_get_file_data(struct ipc_message_info *response);
void ipc_sec_change_locking_pw_set_setup(struct ipc_sec_change_locking_pw_set *message,
    int type, char *passwd_old, char *passwd_new);

#endif

// vim:ts=4:sw=4:expandtab
