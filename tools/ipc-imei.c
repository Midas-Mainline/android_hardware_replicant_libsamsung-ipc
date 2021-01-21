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

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "common/modem.h"

void modem_log_handler(void *user_data, const char *msg)
{
	int i, l;
	char *message;

	message = strdup(msg);
	l = strlen(message);

	if (l > 1) {
		for (i = l ; i > 0 ; i--) {
			if (message[i] == '\n')
				message[i] = 0;
			else if (message[i] != 0)
				break;
		}
		if (user_data == NULL) {
			printf("[ ipc ] %s\n", message);
		}
		if (user_data != NULL) {
			printf("[ %s ] %s\n", user_data, message);
		}
	}

	free(message);
}

static void usage(char* progname)
{
	printf("Usage: %s\n", progname);
}

/* TODO: check authorship from git log in samsung-ipc/ipc.c */
void ipc_imei_log(struct ipc_client *client, const char *message, ...)
{
        char buffer[4096];
        va_list args;

        if (client == NULL || message == NULL)
                return;

        va_start(args, message);
        vsnprintf((char *) &buffer, sizeof(buffer), message, args);
	modem_log_handler("ipc-imei", buffer); /* No access to ipc_client) */
        va_end(args);
}

int ipc_imei_request_imei(struct ipc_client *client)
{
	struct ipc_misc_me_sn_request_data request_data;
	int rc;

	if (!client)
		return 0;

	ipc_imei_log(client, "ENTER %s\n", __func__);

	request_data.type = IPC_MISC_ME_SN_SERIAL_NUM;

	rc = ipc_client_send(client, seq_get(), IPC_MISC_ME_SN, IPC_TYPE_GET,
			     (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		ipc_imei_log(client, "ipc_client_send failed with error %d\n",
			       rc);

	ipc_imei_log(client, "EXIT %s\n", __func__);
	return rc;
}

int ipc_imei_parse_imei_response(struct ipc_client *client,
				 struct ipc_message *message,
				 __attribute__((unused)) void* app_data)
{
	struct ipc_misc_me_sn_response_data *data;
	char *imei;

	if (!client)
		return 0;

	if (message == NULL) {
		ipc_imei_log(client,
			       "%s: ipc_message is null\n", __FUNCTION__);
		return -EAGAIN;
	}

	data = (struct ipc_misc_me_sn_response_data *) message->data;

	if (data->type != IPC_MISC_ME_SN_SERIAL_NUM)
		return -EAGAIN;

	imei = ipc_misc_me_sn_extract(data);

	if (strlen (imei) > 15 * sizeof(char))
		imei[15] = '\0';

	ipc_imei_log(client, "%s: found IMEI '%s'\n", __FUNCTION__, imei);

	free(imei);

	/* Exit */
	return 0;
}

static int ipc_imei_response_handle(struct ipc_client *client,
				    struct ipc_message *resp,
				    void *data)
{
	int rc;

	if (!client)
		return 0;

	switch (IPC_GROUP(resp->command)) {
	case IPC_GROUP_MISC:
		rc = ipc_imei_parse_imei_response(client, resp, data);
		return rc;
	default:
		ipc_imei_log(client, "Unhandled %s command",
			       ipc_group_string(IPC_GROUP(resp->command)));
		return -EAGAIN;
	}
}


int main(int argc, char** argv)
{
	struct ipc_client *client = NULL;
	struct app_modem_response_handler handler;

	int rc;

	if (argc == 0) {
		exit(EX_USAGE);
	} else if (argc != 1) {
		usage(argv[0]);
		exit(EX_USAGE);
	}

	client = ipc_client_create(IPC_CLIENT_TYPE_FMT);
	if (client == NULL) {
		printf("Creating client failed\n");
		return 1;
	}

	rc = ipc_client_log_callback_register(client, modem_log_handler, NULL);
	if (rc < 0) {
		printf("ipc_imei_log_callback_registerma failed:"
		       " error %d\n", rc);
		return 1;
	}
	ipc_imei_log(client, "ipc_client_log_callback_register done");

	/* register new callback that would take over after modem_start is
	 *  done
	 */
	rc = register_app_modem_response_handler(client, &handler,
						 ipc_imei_response_handle,
						 NULL);
	if (rc < 0) {
		ipc_imei_log(client,
			       "register_app_modem_response_handler failed: "
			       "error %d\n",
			       rc);
		return 1;
	}

	ipc_imei_log(client, "Starting modem");

	rc = modem_start(client, MODEM_STATE_LPM, &handler);
	if (rc < 0) {
		ipc_imei_log(client, "modem_start failed: error %d\n", rc);
		return 1;
	}

	ipc_imei_log(client, "modem_start done\n");

	ipc_imei_request_imei(client);

	//ipc_imei_log(client, "ipc_imei_request_imei done\n");

	/* TODO: MODEM_STATE_LPM is not needed here */
	rc = modem_read_loop(client, MODEM_STATE_LPM, &handler);
	if (rc < 0)
		ipc_imei_log(client,
			       "ipc-imei: modem_read_loop failed: error %d\n",
			       rc);

	ipc_imei_log(client, "ipc-imei: modem_read_loop done");

	rc = modem_stop(client);
	if (rc < 0) {
		ipc_imei_log(client, "modem_stop failed: error %d\n", rc);
		return 1;
	}

	return 0;
}
