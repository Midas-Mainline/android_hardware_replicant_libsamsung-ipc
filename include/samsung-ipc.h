/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2012 Simon Busch <morphis@gravedo.de>
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

#include <time.h>

#ifndef __SAMSUNG_IPC_H__
#define __SAMSUNG_IPC_H__

/*
 * Values
 */

#define IPC_CLIENT_TYPE_FMT                                     0x00
#define IPC_CLIENT_TYPE_RFS                                     0x01

/*
 * Structures
 */

struct ipc_client;
struct ipc_handlers;

struct ipc_fmt_header;
struct ipc_rfs_header;

struct ipc_message_info {
    unsigned char mseq;
    unsigned char aseq;
    unsigned char group;
    unsigned char index;
    unsigned short cmd;
    unsigned char type;
    unsigned int length;
    unsigned char *data;
};

struct ipc_client_gprs_capabilities {
    int port_list;
    int cid_max;
};

/*
 * Helpers
 */

struct ipc_client *ipc_client_create(int client_type);
int ipc_client_destroy(struct ipc_client *client);

void ipc_client_log(struct ipc_client *client, const char *message, ...);
int ipc_client_set_log_callback(struct ipc_client *client,
    void (*log_callback)(void *log_data, const char *message), void *log_data);

int ipc_client_set_transport_handlers(struct ipc_client *client,
    int (*open)(void *transport_data, int type),
    int (*close)(void *transport_data),
    int (*read)(void *transport_data, void *buffer, unsigned int length),
    int (*write)(void *transport_data, void *buffer, unsigned int length),
    int (*poll)(void *transport_data, struct timeval *timeout),
    void *transport_data);
int ipc_client_set_power_handlers(struct ipc_client *client,
    int (*power_on)(void *power_data),
    int (*power_off)(void *power_data),
    void *power_data);
int ipc_client_set_gprs_handlers(struct ipc_client *client,
    int (*gprs_activate)(void *gprs_data, int cid),
    int (*gprs_deactivate)(void *gprs_data, int cid),
    void *gprs_data);

int ipc_client_bootstrap(struct ipc_client *client);
int ipc_client_send(struct ipc_client *client, const unsigned short command,
    const char type, unsigned char *data, const int length, unsigned char mseq);
int ipc_client_recv(struct ipc_client *client,
    struct ipc_message_info *response);
void ipc_client_response_free(struct ipc_client *client,
    struct ipc_message_info *response);

int ipc_client_open(struct ipc_client *client);
int ipc_client_close(struct ipc_client *client);
int ipc_client_poll(struct ipc_client *client, struct timeval *timeout);

int ipc_client_power_on(struct ipc_client *client);
int ipc_client_power_off(struct ipc_client *client);

int ipc_client_gprs_activate(struct ipc_client *client, int cid);
int ipc_client_gprs_deactivate(struct ipc_client *client, int cid);

int ipc_client_data_create(struct ipc_client *client);
int ipc_client_data_destroy(struct ipc_client *client);

char *ipc_client_gprs_get_iface(struct ipc_client *client, int cid);
int ipc_client_gprs_get_capabilities(struct ipc_client *client,
    struct ipc_client_gprs_capabilities *capabilities);

char *ipc_client_nv_data_path(struct ipc_client *client);
char *ipc_client_nv_data_md5_path(struct ipc_client *client);
char *ipc_client_nv_data_backup_path(struct ipc_client *client);
char *ipc_client_nv_data_backup_md5_path(struct ipc_client *client);
char *ipc_client_nv_data_secret(struct ipc_client *client);
int ipc_client_nv_data_size(struct ipc_client *client);
int ipc_client_nv_data_chunk_size(struct ipc_client *client);

const char *ipc_response_type_to_str(int type);
const char *ipc_request_type_to_str(int type);
const char *ipc_command_to_str(int command);
void ipc_client_hex_dump(struct ipc_client *client, void *data, int size);
void ipc_client_log_recv(struct ipc_client *client,
    struct ipc_message_info *response, const char *prefix);
void ipc_client_log_send(struct ipc_client *client,
    struct ipc_message_info *request, const char *prefix);
void ipc_fmt_header_fill(struct ipc_fmt_header *header, struct ipc_message_info *message);
void ipc_fmt_message_fill(struct ipc_fmt_header *header, struct ipc_message_info *message);
void ipc_rfs_header_fill(struct ipc_rfs_header *header, struct ipc_message_info *message);
void ipc_rfs_message_fill(struct ipc_rfs_header *header, struct ipc_message_info *message);

/*
 * Samsung-IPC protocol
 */

#include "protocol.h"
#include "pwr.h"
#include "call.h"
#include "sms.h"
#include "sec.h"
#include "pb.h"
#include "disp.h"
#include "net.h"
#include "snd.h"
#include "misc.h"
#include "svc.h"
#include "ss.h"
#include "gprs.h"
#include "sat.h"
#include "imei.h"
#include "rfs.h"
#include "gen.h"

#endif

// vim:ts=4:sw=4:expandtab
