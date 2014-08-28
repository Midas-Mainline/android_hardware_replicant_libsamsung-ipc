/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2014 Paul Kocialkowsk <contact@paulk.fr>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <samsung-ipc.h>

void log_callback(void *data, const char *message)
{
    char *buffer;
    size_t length;
    int i;

    if (message == NULL)
        return;

    buffer = strdup(message);
    length = strlen(message);

    for (i = length; i > 0; i--) {
        if (buffer[i] == '\n')
            buffer[i] = '\0';
        else if (buffer[i] != '\0')
            break;
    }

    printf("[ipc] %s\n", buffer);

    free(buffer);
}

int main(int args, char *argv[])
{
    struct ipc_client *client = NULL;
    struct ipc_message message;
    int rc;
    int i;

    client = ipc_client_create(IPC_CLIENT_TYPE_FMT);
    if (client == NULL) {
        printf("Creating client failed\n");
        goto error;
    }

    rc = ipc_client_log_callback_register(client, log_callback, NULL);
    if (rc < 0) {
        printf("Registering log callback failed\n");
        goto error;
    }

    rc = ipc_client_data_create(client);
    if (rc < 0) {
        printf("Creating data failed\n");
        goto error;
    }

    rc = ipc_client_boot(client);
    if (rc < 0) {
        printf("Booting failed\n");
        goto error;
    }

    rc = ipc_client_power_on(client);
    if (rc < 0) {
        printf("Powering on failed\n");
        goto error;
    }

    rc = ipc_client_open(client);
    if (rc < 0) {
        printf("Opening failed\n");
        goto error;
    }

    for (i = 0; i < 5; i++) {
        rc = ipc_client_poll(client, NULL, NULL);
        if (rc < 0) {
            printf("Polling failed\n");
            break;
        }

        rc = ipc_client_recv(client, &message);
        if (rc < 0) {
            printf("Receiving failed\n");
            break;
        }

        if (message.data != NULL && message.size > 0) {
            free(message.data);
            message.data = NULL;
        }
    }

    rc = ipc_client_close(client);
    if (rc < 0) {
        printf("Closing failed\n");
        goto error;
    }

    rc = ipc_client_power_off(client);
    if (rc < 0) {
        printf("Powering on failed\n");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = 1;

complete:
    if (client != NULL)
        ipc_client_destroy(client);

    return rc;
}

// vim:ts=4:sw=4:expandtab
