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
#include <stdlib.h> /* system("dmesg") */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "modem.h"

int seq;

static enum modem_state current_state = MODEM_STATE_LPM;

static enum modem_callback_state callback_state = MODEM_CALLBACK_STATE_UTILS;

/* TODO: check authorship from git log in samsung-ipc/ipc.c */
void common_modem_log(struct ipc_client *client, const char *message, ...)
{
        char buffer[4096];
        va_list args;

        if (client == NULL || message == NULL)
                return;

        va_start(args, message);
        vsnprintf((char *) &buffer, sizeof(buffer), message, args);
	/* TODO: Fixme: the log handler is from the app!!! */
	modem_log_handler("mdm", buffer); /* No access to ipc_client) */
        va_end(args);
}

/* Taken from tools/ipc-modem.c */
int seq_get(void)
{
        if (seq == 0xff)
                seq = 0x00;

        seq++;

        return seq;
}

static int modem_response_sec(struct ipc_client *client,
			      struct ipc_message *resp,
			      enum modem_state new_state)
{
	struct ipc_sec_pin_status_response_data *sim_status;
	unsigned char type;

	if (!client)
		return 0;


	switch (resp->command) {
	case IPC_SEC_PIN_STATUS:
		sim_status =
			(struct ipc_sec_pin_status_response_data *)resp->data;

		switch (sim_status->status) {
		case IPC_SEC_PIN_STATUS_CARD_NOT_PRESENT:
			common_modem_log(client, "[I] SIM card not found\n");

			if (new_state == MODEM_STATE_SIM_OK)
				return -ENODEV; /* SIM not found but required */
			else
				return 0; /* Modem init done */
		case IPC_SEC_PIN_STATUS_LOCK_SC:
			/* TODO */
			switch (sim_status->facility_lock) {
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ:
				common_modem_log(client,
					    "[I] "
					    "The card is locked with the PIN1\n"
					    "TODO: implement unlocking the card"
					);
				break;
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ:
				common_modem_log(client,
					    "[I] "
					    "The card is locked with the PUK\n"
					    "TODO: implement unlocking the card"
					);
				break;
			case IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED:
				common_modem_log(client, "[I] "
					    "The SIM Card is blocked:\n"
					    "Unless you have a "
					    "programable card with the "
					    "ADM1 pin you might need to "
					    "contact your operator.\n");
				break;
			}

			/* Only fail if the SIM card is required */
			if (new_state == MODEM_STATE_SIM_OK)
				return -EACCES;

			break;
		case IPC_SEC_PIN_STATUS_INIT_COMPLETE:
			common_modem_log(client, "[3] SIM init complete\n");
			if (current_state == MODEM_STATE_NORMAL) {
				current_state = MODEM_STATE_SIM_OK;
				/* In any case we're done when the SIM is
				 * ready
				 */
				return 0;
			}
			break;
		case IPC_SEC_PIN_STATUS_PB_INIT_COMPLETE:
			common_modem_log(client,
				    "[I] SIM Phone Book init complete\n");
			break;
		}
		break;
	case IPC_SEC_SIM_ICC_TYPE:
		type = *((char *) resp->data);
		switch (type) {
		case IPC_SEC_SIM_CARD_TYPE_UNKNOWN:
			common_modem_log(client,
				    "[I] No SIM card type: unknown (absent?)"
				    "\n");

			/* Only fail if the SIM card is required */
			if (new_state == MODEM_STATE_SIM_OK)
				return -ENODEV;
			break;
		case IPC_SEC_SIM_CARD_TYPE_SIM:
		case IPC_SEC_SIM_CARD_TYPE_USIM:
			common_modem_log(client, "[I] SIM card found\n");
			break;
		}
		break;
	}

	return -EAGAIN;
}

int modem_stop(struct ipc_client *client)
{
	int rc;

	common_modem_log(client, "%s: ipc_client_close", __FUNCTION__);
	rc = ipc_client_close(client);
	if (rc < 0) {
		common_modem_log(client, "Closing failed: error %d\n", rc);
		return rc;
	}

	common_modem_log(client, "%s: ipc_client_power_off", __FUNCTION__);
	rc = ipc_client_power_off(client);
	if (rc < 0) {
		common_modem_log(client, "Powering on failed: error %d\n", rc);
		return rc;
	}

	common_modem_log(client, "%s: Modem stopped", __FUNCTION__);

	return 0;
}

/* TODO: share the code */
static int modem_response_pwr(struct ipc_client *client,
			      struct ipc_message *resp,
			      __attribute__((unused)) enum modem_state new_state)
{
	int state_n;

	if (!client)
		return 0;

	switch (resp->command) {
	case IPC_PWR_PHONE_PWR_UP:
		common_modem_log(client, "[2] Phone is powered up (LPM)!\n");
		current_state = MODEM_STATE_LPM;
		break;

	case IPC_PWR_PHONE_STATE:
		state_n = *((int *)resp->data);
#if 0
		switch (state_n) {
			/* FIXME: Broken */
		case IPC_PWR_PHONE_STATE_NORMAL:
			common_modem_log(client, "Power state is now: NORMAL\n");
			break;
		case IPC_PWR_PHONE_STATE_LPM:
			common_modem_log(client, "Power state is now: "
				       "LPM (Low Power Mode)?\n");
			break;
		}
#else
		common_modem_log(client, "Power state is now: 0x%x\n",
			       current_state);
#endif
		current_state = state_n;
		break;

	}

	return 0;
}

/*
 * -EAGAIN is returned when modem_response_handle handled a
 * response and another value < 0 when there were errors.
 * It returns 0 when it's done initializing the modem, so
 * application writers can more easily run the code they
 * whish at this point and handle the responses the way they
 * whish.
 */
static int modem_start_response_handle(struct ipc_client *client,
				       struct ipc_message *resp,
				       enum modem_state new_state)
{
	int rc;

	if (!client)
		return 0;

	switch (IPC_GROUP(resp->command)) {
	case IPC_GROUP_PWR:
		rc = modem_response_pwr(client, resp, new_state);
		if (rc == 0)
			return -EAGAIN;
		return rc;
	case IPC_GROUP_SEC:
		return modem_response_sec(client, resp, new_state);
	default:
		common_modem_log(client, "Unhandled %s command",
			       ipc_group_string(IPC_GROUP(resp->command)));
		return -EAGAIN;
	}
}

int register_app_modem_response_handler(
	struct ipc_client *client,
	struct app_modem_response_handler *handler,
	int (*handler_func)(struct ipc_client *client, struct ipc_message *resp, void *handler_data),
	void *handler_func_data)
{
	if (!client)
		return 0;

	if (!handler)
		return 0;

	handler->handler = handler_func;
	handler->data = handler_func_data;

	return 0;
}

static int modem_response_handle(struct ipc_client *client,
				 struct ipc_message *resp,
				 enum modem_state new_state,
				 struct app_modem_response_handler *handler)
{
	int rc;

	if (!client)
		return 0;


	if (callback_state == MODEM_CALLBACK_STATE_UTILS)
		rc = modem_start_response_handle(client, resp, new_state);
	else
		rc = handler->handler(client, resp, handler->data);

	return rc;
}

/* TODO: share modem_read_loop and fix it elsewhere too */
/* TODO: new_state is not needed for the application */
int modem_read_loop(struct ipc_client *client,
		    enum modem_state new_state,
		    struct app_modem_response_handler *handler)
{
	struct ipc_message resp;
	int rc;

	common_modem_log(client, "ENTER %s", __func__);

	if (!client) {
		common_modem_log(client, "%s: ipc_client is NULL", __func__);
		return 0;
	}

	ipc_imei_request_imei(client);


	memset(&resp, 0, sizeof(resp));

	while (1) {
		usleep(3000);

		common_modem_log(client, "%s: Starting ipc_client_poll", __func__);
		rc = ipc_client_poll(client, NULL, NULL);

		common_modem_log(client, "%s: ipc_client_poll: %d", __func__, rc);
		if (rc < 0)
			continue;

		common_modem_log(client, "%s: ipc_client_poll done", __func__);

		rc = ipc_client_recv(client, &resp);
		if (rc < 0) {
			if (resp.data != NULL)
				free(resp.data);
			return rc;
		}

		common_modem_log(client, "%s: Received response", __func__);

		rc = modem_response_handle(client, &resp, new_state, handler);

		common_modem_log(client, "modem_response_handle: rc=%d", rc);

		if (resp.data != NULL)
			free(resp.data);

		if (rc == -EAGAIN) {
			continue;
		} else if (rc < 0) {
			return rc;
		} else if (rc == 0) {
			/* The callback exited normally because it reached
			 * new_state.
			 * It's now to the app callback to take over
			 */
			if (callback_state == MODEM_CALLBACK_STATE_UTILS)
				callback_state = MODEM_CALLBACK_STATE_APP;
			return 0;
		}
	}
}

static int _modem_start(struct ipc_client *client)
{
	int rc;

	if (!client)
		return 0;

	rc = ipc_client_data_create(client);
	if (rc < 0) {
		common_modem_log(client, "Creating data failed: error %d\n", rc);
		return rc;
	}

	rc = ipc_client_boot(client);
	if (rc < 0) {
		common_modem_log(client, "Booting failed: error %d\n", rc);
		return rc;
	}

	rc = ipc_client_power_on(client);
	if (rc < 0) {
		common_modem_log(client, "Powering on failed: error %d\n", rc);
		return rc;
	}

	rc = ipc_client_open(client);
	if (rc < 0) {
		common_modem_log(client, "ipc_client_open failed: error %d\n",
			       rc);
		return rc;
	}

	rc = ipc_client_power_on(client);
	if (rc < 0) {
		common_modem_log(client,
			    "ipc_client_power_on failed: error %d\n", rc);
		return rc;
	}

	return rc;
}

const char *modem_state_to_string(enum modem_state state)
{
	static char group_string[5] = { 0 };

	switch (state) {
	case MODEM_STATE_LPM:
		return "MODEM_STATE_LPM";
	case MODEM_STATE_NORMAL:
		return "MODEM_STATE_NORMAL";
	case MODEM_STATE_SIM_OK:
		return "MODEM_STATE_SIM_OK";
	default:
		snprintf((char *) &group_string, sizeof(group_string), "0x%02x",
			 (unsigned int)group_string);
		return group_string;
	}
};

int modem_start(struct ipc_client *client, enum modem_state new_state,
		struct app_modem_response_handler *handler)
{
	int rc;

	if (!client)
		return 0;

	common_modem_log(client, "%s: requested state %s: %d",
		       __FUNCTION__,
		       modem_state_to_string(new_state),
		       new_state);

	rc = _modem_start(client);
	if (rc < 0) {
		ipc_client_destroy(client);
		return 1;
	}

	common_modem_log(client, "%s: modem_start done", __func__);

	return 0;
}
