/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2014 Paul Kocialkowsk <contact@paulk.fr>
 * Copyright (C) 2021 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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

#ifndef TOOLS_UTILS_H
#define TOOLS_UTILS_H

#include <stddef.h>  /*fixme */
#include <samsung-ipc.h>

#define BIT(n) (1<<n)

int seq_get(void);

struct app_modem_response_handler {
	int (*handler)(struct ipc_client *client, struct ipc_message *resp, void *handler_data);
	void *data;
};

enum modem_state {
	MODEM_STATE_LPM    = BIT(0),
	MODEM_STATE_NORMAL = BIT(1),
	MODEM_STATE_SIM_OK = BIT(2),
};

enum modem_callback_state {
	MODEM_CALLBACK_STATE_UTILS = BIT(0),
        MODEM_CALLBACK_STATE_APP   = BIT(1),
};

int modem_start(struct ipc_client *client, enum modem_state state,
		struct app_modem_response_handler *handler);
int modem_stop(struct ipc_client *client);

int modem_read_loop(struct ipc_client *client,
		    enum modem_state new_state,
		    struct app_modem_response_handler *handler);

int register_app_modem_response_handler(
	struct ipc_client *client,
	struct app_modem_response_handler *handler,
	int (*handler_func)(struct ipc_client *client, struct ipc_message *resp, void *handler_data),
	void *handler_func_data);


#endif /* TOOLS_UTILS_H */
