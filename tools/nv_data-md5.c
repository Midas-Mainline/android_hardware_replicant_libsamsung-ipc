/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2016 Paul Kocialkowsk <contact@paulk.fr>
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
#include <sys/types.h>

#include <samsung-ipc.h>
#include <string.h>

void usage_print(void)
{
    printf("Usage: nv_data-md5 [nv_data.bin]\n");
}

void log_callback(__attribute__((unused)) void *data,
		  const char *message)
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

int main(int argc, char *argv[])
{
    struct ipc_client *client = NULL;
    char *secret = NV_DATA_SECRET;
    size_t size = NV_DATA_SIZE;
    size_t chunk_size = NV_DATA_CHUNK_SIZE;
    char *md5_string = NULL;
    char *path;
    int rc = 0;

    if (argc < 2) {
        usage_print();
        return 1;
    }

    path = argv[1];

    client = ipc_client_create(IPC_CLIENT_TYPE_DUMMY);
    if (client == NULL) {
        printf("Creating client failed\n");
        goto error;
    }

    rc = ipc_client_log_callback_register(client, log_callback, NULL);
    if (rc < 0) {
        printf("Registering log callback failed: error %d\n", rc);
        goto error;
    }

    md5_string = ipc_nv_data_md5_calculate(client, path, secret, size, chunk_size);
    if (md5_string == NULL) {
        fprintf(stderr, "Calculating nv_data backup md5 failed\n");
        return 1;
    }

    printf("%s", md5_string);

    free(md5_string);

    return 0;

error:
    if (client != NULL)
        ipc_client_destroy(client);
}

// vim:ts=4:sw=4:expandtab
