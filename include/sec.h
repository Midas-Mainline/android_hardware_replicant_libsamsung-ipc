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

#ifndef __SAMSUNG_IPC_SEC_H__
#define __SAMSUNG_IPC_SEC_H__

/*
 * Commands
 */

#define IPC_SEC_PIN_STATUS                                      0x0501
#define IPC_SEC_PHONE_LOCK                                      0x0502
#define IPC_SEC_CHANGE_LOCKING_PW                               0x0503
#define IPC_SEC_SIM_LANG                                        0x0504
#define IPC_SEC_RSIM_ACCESS                                     0x0505
#define IPC_SEC_GSIM_ACCESS                                     0x0506
#define IPC_SEC_SIM_ICC_TYPE                                    0x0507
#define IPC_SEC_LOCK_INFOMATION                                 0x0508
#define IPC_SEC_IMS_AUTH                                        0x0509

/*
 * Values
 */

#define IPC_SEC_PIN_STATUS_READY                                0x00
#define IPC_SEC_PIN_STATUS_SIM_LOCK_REQUIRED                    0x01
#define IPC_SEC_PIN_STATUS_INSIDE_PF_ERROR                      0x02
#define IPC_SEC_PIN_STATUS_LOCK_SC                              0x03
#define IPC_SEC_PIN_STATUS_LOCK_FD                              0x04
#define IPC_SEC_PIN_STATUS_LOCK_PN                              0x05
#define IPC_SEC_PIN_STATUS_LOCK_PU                              0x06
#define IPC_SEC_PIN_STATUS_LOCK_PP                              0x07
#define IPC_SEC_PIN_STATUS_LOCK_PC                              0x08
#define IPC_SEC_PIN_STATUS_CARD_NOT_PRESENT                     0x80
#define IPC_SEC_PIN_STATUS_CARD_ERROR                           0x81
#define IPC_SEC_PIN_STATUS_INIT_COMPLETE                        0x82
#define IPC_SEC_PIN_STATUS_PB_INIT_COMPLETE                     0x83

#define IPC_SEC_FACILITY_LOCK_TYPE_SC_UNLOCKED                  0x00
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ                  0x01
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ                   0x02
#define IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED              0x05

#define IPC_SEC_PIN_TYPE_PIN1                                   0x03
#define IPC_SEC_PIN_TYPE_PIN2                                   0x09

#define IPC_SEC_FACILITY_TYPE_SC                                0x03
#define IPC_SEC_FACILITY_TYPE_FD                                0x04
#define IPC_SEC_FACILITY_TYPE_PN                                0x05
#define IPC_SEC_FACILITY_TYPE_PU                                0x06
#define IPC_SEC_FACILITY_TYPE_PP                                0x07
#define IPC_SEC_FACILITY_TYPE_PC                                0x08

#define IPC_SEC_RSIM_COMMAND_READ_BINARY                        0xB0
#define IPC_SEC_RSIM_COMMAND_READ_RECORD                        0xB2
#define IPC_SEC_RSIM_COMMAND_GET_RESPONSE                       0xC0
#define IPC_SEC_RSIM_COMMAND_UPDATE_BINARY                      0xD6
#define IPC_SEC_RSIM_COMMAND_UPDATE_RECORD                      0xDC
#define IPC_SEC_RSIM_COMMAND_STATUS                             0xF2
#define IPC_SEC_RSIM_COMMAND_RETRIEVE_DATA                      0xCB
#define IPC_SEC_RSIM_COMMAND_SET_DATA                           0xDB

#define IPC_SEC_RSIM_FILE_STRUCTURE_TRANSPARENT                 0x83
#define IPC_SEC_RSIM_FILE_STRUCTURE_LINEAR_FIXED                0x00

#define IPC_SEC_SIM_CARD_TYPE_UNKNOWN                           0x00
#define IPC_SEC_SIM_CARD_TYPE_SIM                               0x01
#define IPC_SEC_SIM_CARD_TYPE_USIM                              0x02

/*
 * Structures
 */

struct ipc_sec_pin_status_response_data {
    unsigned char status; // IPC_SEC_PIN_STATUS
    unsigned char facility_lock; // IPC_SEC_FACILITY_LOCK_TYPE
} __attribute__((__packed__));

struct ipc_sec_pin_status_request_data {
    unsigned char type; // IPC_SEC_PIN_TYPE
    unsigned char pin1_length;
    unsigned char pin2_length;
    unsigned char pin1[8];
    unsigned char pin2[8];
} __attribute__((__packed__));

struct ipc_sec_phone_lock_request_set_data {
    unsigned char facility_type; // IPC_SEC_FACILITY_TYPE
    unsigned char active;
    unsigned char password_length;
    unsigned char password[39];
}  __attribute__((__packed__));

struct ipc_sec_phone_lock_request_get_data {
    unsigned char facility_type; // IPC_SEC_FACILITY_TYPE
}  __attribute__((__packed__));

struct ipc_sec_phone_lock_response_data {
    unsigned char facility_type; // IPC_SEC_FACILITY_TYPE
    unsigned char active;
} __attribute__((__packed__));

struct ipc_sec_change_locking_pw_data {
    unsigned char facility_type; // IPC_SEC_FACILITY_TYPE
    unsigned char password_old_length;
    unsigned char password_new_length;
    unsigned char password_old[39];
    unsigned char password_new[39];
}  __attribute__((__packed__));

struct ipc_sec_rsim_access_request_header {
    unsigned char command; // IPC_SEC_RSIM_COMMAND
    unsigned short file_id;
    unsigned char p1, p2, p3;
} __attribute__((__packed__));

struct ipc_sec_rsim_access_response_header {
    unsigned char sw1, sw2;
    unsigned char length;
} __attribute__((__packed__));

struct ipc_sec_rsim_access_usim_response_header {
	unsigned char unknown1[3];
    unsigned char offset;
    unsigned char unknown2[2];
	unsigned char file_structure; // IPC_SEC_RSIM_FILE_STRUCTURE
	unsigned char length;
} __attribute__((__packed__));

struct ipc_sec_sim_icc_type_data {
    unsigned char type; // IPC_SEC_SIM_CARD_TYPE
} __attribute__((__packed__));

struct ipc_sec_lock_infomation_request_data {
    unsigned char magic;
    unsigned char type; // IPC_SEC_PIN_TYPE
} __attribute__((__packed__));

struct ipc_sec_lock_infomation_response_data {
    unsigned char unknown;
    unsigned char type; // IPC_SEC_PIN_TYPE
    unsigned char key;
    unsigned char retry_count;
} __attribute__((__packed__));

/*
 * Helpers
 */

int ipc_sec_pin_status_setup(struct ipc_sec_pin_status_request_data *data,
    unsigned char type, const char *pin1, const char *pin2);
int ipc_sec_phone_lock_request_set_setup(struct ipc_sec_phone_lock_request_set_data *data,
    unsigned char facility_type, unsigned char active, const char *password);
int ipc_sec_change_locking_pw_setup(struct ipc_sec_change_locking_pw_data *data,
    unsigned char facility_type, const char *password_old,
    const char *password_new);
size_t ipc_sec_rsim_access_size_setup(struct ipc_sec_rsim_access_request_header *header,
    const void *sim_io_data, size_t sim_io_size);
void *ipc_sec_rsim_access_setup(struct ipc_sec_rsim_access_request_header *header,
    const void *sim_io_data, size_t sim_io_size);
size_t ipc_sec_rsim_access_size_extract(const void *data, size_t size);
void *ipc_sec_rsim_access_extract(const void *data, size_t size);
int ipc_sec_lock_infomation_setup(struct ipc_sec_lock_infomation_request_data *data,
    unsigned char type);

#endif

// vim:ts=4:sw=4:expandtab
