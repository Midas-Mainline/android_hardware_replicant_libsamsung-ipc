/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Simon Busch <morphis@gravedo.de>
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

typedef void (*ipc_client_log_handler_cb)(const char *message, void *user_data);

typedef int (*ipc_io_handler_cb)(void *data, unsigned int size, void *io_data);
typedef int (*ipc_handler_cb)(void *io_data);

struct ipc_client* ipc_client_new(int client_type);
struct ipc_client *ipc_client_new_for_device(int device_type, int client_type);
int ipc_client_free(struct ipc_client *client);

int ipc_client_set_log_handler(struct ipc_client *client, ipc_client_log_handler_cb log_handler_cb, void *user_data);

int ipc_client_set_handlers(struct ipc_client *client, struct ipc_handlers *handlers);
int ipc_client_set_io_handlers(struct ipc_client *client, ipc_io_handler_cb read, void *read_data, ipc_io_handler_cb write, void *write_data);

int ipc_client_set_handlers_common_data(struct ipc_client *client, void *data);
void *ipc_client_get_handlers_common_data(struct ipc_client *client);
int ipc_client_create_handlers_common_data(struct ipc_client *client);
int ipc_client_destroy_handlers_common_data(struct ipc_client *client);
int ipc_client_set_handlers_common_data_fd(struct ipc_client *client, int fd);
int ipc_client_get_handlers_common_data_fd(struct ipc_client *client);

int ipc_client_bootstrap_modem(struct ipc_client *client);
int ipc_client_open(struct ipc_client *client);
int ipc_client_close(struct ipc_client *client);
int ipc_client_power_on(struct ipc_client *client);
int ipc_client_power_off(struct ipc_client *client);
int ipc_client_gprs_handlers_available(struct ipc_client *client);
int ipc_client_gprs_activate(struct ipc_client *client, int cid);
int ipc_client_gprs_deactivate(struct ipc_client *client, int cid);
char *ipc_client_gprs_get_iface(struct ipc_client *client, int cid);
int ipc_client_gprs_get_capabilities(struct ipc_client *client, struct ipc_client_gprs_capabilities *cap);

int ipc_client_recv(struct ipc_client *client, struct ipc_message_info *response);
void ipc_client_response_free(struct ipc_client *client, struct ipc_message_info *response);

/* Convenience functions for ipc_send */
void ipc_client_send(struct ipc_client *client, const unsigned short command, const char type, unsigned char *data, const int length, unsigned char mseq);
void ipc_client_send_get(struct ipc_client *client, const unsigned short command, unsigned char mseq);
void ipc_client_send_exec(struct ipc_client *client, const unsigned short command, unsigned char mseq);

/* Utility functions */
void ipc_client_log_recv(struct ipc_client *client, struct ipc_message_info *response, const char *prefix);
void ipc_client_log_send(struct ipc_client *client, struct ipc_message_info *request, const char *prefix);
const char *ipc_response_type_to_str(int type);
const char *ipc_request_type_to_str(int type);
const char *ipc_command_to_str(int command);

void ipc_client_hex_dump(struct ipc_client *client, void *data, int size);
void *ipc_client_mtd_read(struct ipc_client *client, char *mtd_name, int size, int block_size);
void *ipc_client_file_read(struct ipc_client *client, char *file_name, int size, int block_size);

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
#include "ss.h"
#include "gprs.h"
#include "sat.h"
#include "imei.h"
#include "rfs.h"
#include "gen.h"

#endif

// vim:ts=4:sw=4:expandtab
