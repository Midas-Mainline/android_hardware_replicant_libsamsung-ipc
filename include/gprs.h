/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef __GPRS_H__
#define __GPRS_H__

#define IPC_GPRS_DEFINE_PDP_CONTEXT         0x0D01
#define IPC_GPRS_QOS                        0x0D02
#define IPC_GPRS_PS                         0x0D03
#define IPC_GPRS_PDP_CONTEXT                0x0D04
#define IPC_GPRS_ENTER_DATA                 0x0D05
#define IPC_GPRS_SHOW_PDP_ADDR              0x0D06
#define IPC_GPRS_MS_CLASS                   0x0D07
#define IPC_GPRS_3G_QUAL_SERVICE_PROFILE    0x0D08
#define IPC_GPRS_IP_CONFIGURATION           0x0D09
#define IPC_GPRS_DEFINE_SEC_PDP_CONTEXT     0x0D0A
#define IPC_GPRS_TFT                        0x0D0B
#define IPC_GPRS_HSDPA_STATUS               0x0D0C
#define IPC_GPRS_CURRENT_SESSION_DATA_COUNT 0x0D0D
#define IPC_GPRS_DATA_DORMANT               0x0D0E
#define IPC_GPRS_DUN_PIN_CTRL               0x0D0F
#define IPC_GPRS_CALL_STATUS                0x0D10
#define IPC_GPRS_PORT_LIST                  0x0D11

#define IPC_GPRS_STATE_NOT_ENABLED          0x00
#define IPC_GPRS_STATE_ENABLED              0x01
#define IPC_GPRS_STATE_DISABLED             0x03

#define IPC_GPRS_FAIL_INSUFFICIENT_RESOURCES        0x0004
#define IPC_GPRS_FAIL_MISSING_UKNOWN_APN            0x0005
#define IPC_GPRS_FAIL_UNKNOWN_PDP_ADDRESS_TYPE      0x0006
#define IPC_GPRS_FAIL_USER_AUTHENTICATION           0x0007
#define IPC_GPRS_FAIL_ACTIVATION_REJECT_GGSN        0x0008
#define IPC_GPRS_FAIL_ACTIVATION_REJECT_UNSPECIFIED 0x0009
#define IPC_GPRS_FAIL_SERVICE_OPTION_NOT_SUPPORTED  0x000A
#define IPC_GPRS_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED 0x000B
#define IPC_GPRS_FAIL_SERVICE_OPTION_OUT_OF_ORDER   0x000C
#define IPC_GPRS_FAIL_NSAPI_IN_USE                  0x000D

#define IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT         3

struct ipc_gprs_define_pdp_context {
    unsigned char enable;
    unsigned char cid;
    unsigned char unk;
    unsigned char apn[124];
};

struct ipc_gprs_pdp_context_set {
    unsigned char enable;
    unsigned char cid;
    unsigned char unk0[4];
    unsigned char username[32];
    unsigned char password[32];
    unsigned char unk1[32];
    unsigned char unk2;
};

struct ipc_gprs_pdp_context_get_desc {
    unsigned char cid;
    unsigned char state;
};

struct ipc_gprs_pdp_context_get {
    unsigned char unk;
    struct ipc_gprs_pdp_context_get_desc desc[IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT];
};

struct ipc_gprs_ip_configuration {
    unsigned char cid;
    unsigned char field_flag;
    unsigned char unk1;
    unsigned char ip[4];
    unsigned char dns1[4];
    unsigned char dns2[4];
    unsigned char gateway[4];
    unsigned char subnet_mask[4];
    unsigned char unk2[4];
};

struct ipc_gprs_call_status {
    unsigned char cid;
    unsigned char state;
    unsigned short fail_cause;
};

struct ipc_gprs_hsdpa_status {
    unsigned char unk;
};

struct ipc_gprs_ps {
    unsigned char unk[2];
};

struct ipc_gprs_current_session_data_counter {
    unsigned char cid;
    unsigned int rx_count;
    unsigned int tx_count;
};

struct ipc_gprs_port_list {
    unsigned char unk[804];
};

void ipc_gprs_port_list_setup(struct ipc_gprs_port_list *message);
void ipc_gprs_pdp_context_setup(struct ipc_gprs_pdp_context_set *message,
    unsigned char cid, int enable, char *username, char *password);
void ipc_gprs_define_pdp_context_setup(struct ipc_gprs_define_pdp_context *message,
    unsigned char cid, int enable, char *apn);

#endif

// vim:ts=4:sw=4:expandtab
