/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
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

#ifndef __IPC_PRIVATE_H__
#define __IPC_PRIVATE_H__

#include <radio.h>

struct ipc_ops {
    int (*bootstrap)(struct ipc_client *client);
    int (*send)(struct ipc_client *client, struct ipc_message_info *);
    int (*recv)(struct ipc_client *client, struct ipc_message_info *);
};

struct ipc_handlers {
    /* Transport handlers/data */
    ipc_io_handler_cb read;
    void *read_data;
    ipc_io_handler_cb write;
    void *write_data;

    int (*open)(int type, void *io_data);
    int (*close)(void *io_data);
    void *open_data;
    void *close_data;

    /* Power handlers */
    ipc_handler_cb power_on;
    void *power_on_data;
    ipc_handler_cb power_off;
    void *power_off_data;

    /* GPRS handlers */
    int (*gprs_activate)(void *io_data, int cid);
    void *gprs_activate_data;
    int (*gprs_deactivate)(void *io_data, int cid);
    void *gprs_deactivate_data;

    /* Handlers common data*/
    void *common_data;

    void *(*common_data_create)(void);
    int (*common_data_destroy)(void *io_data);
    int (*common_data_set_fd)(void *io_data, int fd);
    int (*common_data_get_fd)(void *io_data);
};

struct ipc_gprs_specs {
    char* (*gprs_get_iface)(int cid);
    int (*gprs_get_capabilities)(struct ipc_client_gprs_capabilities *cap);
};

struct ipc_nv_data_specs {
    char *nv_data_path;
    char *nv_data_md5_path;
    char *nv_data_bak_path;
    char *nv_data_md5_bak_path;
    char *nv_state_path;
    char *nv_data_secret;
    int nv_data_size;
    int nv_data_chunk_size;
};

struct ipc_client {
    int type;

    ipc_client_log_handler_cb log_handler;
    void *log_data;

    struct ipc_ops *ops;
    struct ipc_handlers *handlers;
    struct ipc_gprs_specs *gprs_specs;
    struct ipc_nv_data_specs *nv_data_specs;
};

void ipc_client_log(struct ipc_client *client, const char *message, ...);

#endif

// vim:ts=4:sw=4:expandtab
