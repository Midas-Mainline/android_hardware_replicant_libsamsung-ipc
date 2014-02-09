/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
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

#ifndef __SAMSUNG_IPC_GPRS_H__
#define __SAMSUNG_IPC_GPRS_H__

/*
 * Commands
 */

#define IPC_GPRS_DEFINE_PDP_CONTEXT                             0x0D01
#define IPC_GPRS_QOS                                            0x0D02
#define IPC_GPRS_PS                                             0x0D03
#define IPC_GPRS_PDP_CONTEXT                                    0x0D04
#define IPC_GPRS_ENTER_DATA                                     0x0D05
#define IPC_GPRS_SHOW_PDP_ADDR                                  0x0D06
#define IPC_GPRS_MS_CLASS                                       0x0D07
#define IPC_GPRS_3G_QUAL_SRVC_PROFILE                           0x0D08
#define IPC_GPRS_IP_CONFIGURATION                               0x0D09
#define IPC_GPRS_DEFINE_SEC_PDP_CONTEXT                         0x0D0A
#define IPC_GPRS_TFT                                            0x0D0B
#define IPC_GPRS_HSDPA_STATUS                                   0x0D0C
#define IPC_GPRS_CURRENT_SESSION_DATA_COUNTER                   0x0D0D
#define IPC_GPRS_DATA_DORMANT                                   0x0D0E
#define IPC_GPRS_PIN_CTRL                                       0x0D0F
#define IPC_GPRS_CALL_STATUS                                    0x0D10
#define IPC_GPRS_PORT_LIST                                      0x0D11

/*
 * Values
 */

#define IPC_GPRS_STATUS_NOT_ENABLED                             0x00
#define IPC_GPRS_STATUS_ENABLED                                 0x01
#define IPC_GPRS_STATUS_DISABLED                                0x03

#define IPC_GPRS_FAIL_CAUSE_INSUFFICIENT_RESOURCES              0x0004
#define IPC_GPRS_FAIL_CAUSE_MISSING_UKNOWN_APN                  0x0005
#define IPC_GPRS_FAIL_CAUSE_UNKNOWN_PDP_ADDRESS_TYPE            0x0006
#define IPC_GPRS_FAIL_CAUSE_USER_AUTHENTICATION                 0x0007
#define IPC_GPRS_FAIL_CAUSE_ACTIVATION_REJECT_GGSN              0x0008
#define IPC_GPRS_FAIL_CAUSE_ACTIVATION_REJECT_UNSPECIFIED       0x0009
#define IPC_GPRS_FAIL_CAUSE_SERVICE_OPTION_NOT_SUPPORTED        0x000A
#define IPC_GPRS_FAIL_CAUSE_SERVICE_OPTION_NOT_SUBSCRIBED       0x000B
#define IPC_GPRS_FAIL_CAUSE_SERVICE_OPTION_OUT_OF_ORDER         0x000C
#define IPC_GPRS_FAIL_CAUSE_NSAPI_IN_USE                        0x000D

/*
 * Structures
 */

struct ipc_gprs_define_pdp_context_data {
    unsigned char enable;
    unsigned char cid;
    unsigned char magic;
    unsigned char apn[124];
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_set_data {
    unsigned char enable;
    unsigned char cid;
    unsigned char magic1[4];
    unsigned char username[32];
    unsigned char password[32];
    unsigned char unknown1[32];
    unsigned char magic2;
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_get_entry {
    unsigned char cid;
    unsigned char status; // IPC_GPRS_STATUS
} __attribute__((__packed__));

struct ipc_gprs_pdp_context_request_get_data {
    unsigned char unknown;
    struct ipc_gprs_pdp_context_request_get_entry entries[3];
} __attribute__((__packed__));

struct ipc_gprs_ip_configuration_data {
    unsigned char cid;
    unsigned char field_flag;
    unsigned char unknown1;
    unsigned char ip[4];
    unsigned char dns1[4];
    unsigned char dns2[4];
    unsigned char gateway[4];
    unsigned char subnet_mask[4];
    unsigned char unknown2[4];
} __attribute__((__packed__));

struct ipc_gprs_hsdpa_status_data {
    unsigned char status;
} __attribute__((__packed__));

struct ipc_gprs_current_session_data_counter_data {
    unsigned char cid;
    unsigned int rx_count;
    unsigned int tx_count;
} __attribute__((__packed__));

struct ipc_gprs_call_status_data {
    unsigned char cid;
    unsigned char status;
    unsigned short fail_cause; // IPC_GPRS_FAIL_CAUSE
} __attribute__((__packed__));

struct ipc_gprs_port_list_data {
    unsigned char magic[804];
} __attribute__((__packed__));

/*
 * Helpers
 */

int ipc_gprs_define_pdp_context_setup(struct ipc_gprs_define_pdp_context_data *data,
    unsigned char enable, unsigned char cid, const char *apn);
int ipc_gprs_pdp_context_request_set_setup(struct ipc_gprs_pdp_context_request_set_data *data,
    unsigned char enable, unsigned char cid, const char *username,
    const char *password);
int ipc_gprs_port_list_setup(struct ipc_gprs_port_list_data *data);

#endif

// vim:ts=4:sw=4:expandtab
