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
#include <unistd.h>
#include <getopt.h>

#include <samsung-ipc.h>

#include <utils.h>

#define IMEI_LENGTH 15

int help(void)
{
    printf("Usage: rfs-imei [OPTION]...\n");
    printf("Options:\n");
    printf("\t--help                    display this help message\n");
    printf("\t--show-imei               show the current imei from nv_data\n");
    printf("\t--store-imei=[IMEI]       store imei to nv_data\n");
    printf("\t--nv_data-path=[PATH]     select nv_data path\n");

    return 0;
}

int show_imei(char *nv_data_path)
{
    struct ipc_client *client = NULL;
    size_t nv_data_size;
    size_t nv_data_chunk_size;
    void *buffer = NULL;
    char imei[IMEI_LENGTH + 1] = { 0 };
    unsigned char *p;
    unsigned int i;
    int rc;

    client = ipc_client_create(IPC_CLIENT_TYPE_FMT);
    if (client == NULL) {
        printf("Creating client failed\n");
        goto error;
    }

    if (nv_data_path == NULL)
        nv_data_path = "nv_data.bin";

    nv_data_size = ipc_client_nv_data_size(client);
    nv_data_chunk_size = ipc_client_nv_data_chunk_size(client);

    buffer = file_data_read(nv_data_path, nv_data_size, nv_data_chunk_size, 0);
    if (buffer == NULL) {
        printf("Reading nv_data failed\n");
        goto error;
    }

    p = (unsigned char *) buffer + 0xE880;
    i = 0;

    i += snprintf(&imei[i], IMEI_LENGTH + 1 - i, "%01x", (*p & 0xf0) >> 4);
    p += sizeof(unsigned char);

    while (i < IMEI_LENGTH) {
        i += snprintf(&imei[i], IMEI_LENGTH + 1 - i, "%02x", (*p >> 4) | ((*p & 0x0f) << 4));
        p += sizeof(unsigned char);
    }

    printf("%s\n", imei);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    if (client != NULL)
        ipc_client_destroy(client);

    return rc;
}

int store_imei(char *nv_data_path, char *imei)
{
    struct ipc_client *client = NULL;
    char *nv_data_md5_path = NULL;
    char *nv_data_secret;
    size_t nv_data_chunk_size;
    size_t nv_data_size;
    char *nv_data_md5_string = NULL;
    unsigned char buffer[(IMEI_LENGTH + 1) / 2] = { 0 };
    size_t length;
    unsigned char *p;
    unsigned int v;
    unsigned int i;
    int rc;

    if (imei == NULL || strlen(imei) < IMEI_LENGTH) {
        printf("Provided IMEI is invalid\n");
        goto error;
    }

    client = ipc_client_create(IPC_CLIENT_TYPE_FMT);
    if (client == NULL) {
        printf("Creating client failed\n");
        goto error;
    }

    if (nv_data_path == NULL)
        nv_data_path = "nv_data.bin";

    asprintf(&nv_data_md5_path, "%s.md5", nv_data_path);

    nv_data_secret = ipc_client_nv_data_secret(client);
    nv_data_size = ipc_client_nv_data_size(client);
    nv_data_chunk_size = ipc_client_nv_data_chunk_size(client);

    p = (unsigned char *) &buffer;
    i = 0;

    sscanf(&imei[i], "%01x", &v);
    *p++ = (v << 4) | 0xA;
    i++;

    while (i < IMEI_LENGTH) {
        sscanf(&imei[i], "%02x", &v);
        *p++ = v << 4 | ((v & 0xf0) >> 4);
        i += 2;
    }

    rc = file_data_write(nv_data_path, buffer, sizeof(buffer), sizeof(buffer), 0xE880);
    if (rc < 0) {
        printf("Writing nv_data failed\n");
        goto error;
    }

    nv_data_md5_string = ipc_nv_data_md5_calculate(nv_data_path, nv_data_secret, nv_data_size, nv_data_chunk_size);
    if (nv_data_md5_string == NULL) {
        printf("Calculating nv_data md5 failed\n");
        goto error;
    }

    length = strlen(nv_data_md5_string);

    unlink(nv_data_md5_path);

    rc = file_data_write(nv_data_md5_path, nv_data_md5_string, length, length, 0);
    if (rc < 0) {
        printf("Writing nv_data md5 failed\n");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (nv_data_md5_path != NULL)
        free(nv_data_md5_path);

    if (client != NULL)
        ipc_client_destroy(client);

    return rc;
}

int main(int argc, const char *argv[])
{
    int (*show_imei_callback)(char *nv_data_path) = NULL;
    int (*store_imei_callback)(char *nv_data_path, char *imei) = NULL;
    char *nv_data_path = NULL;
    char *imei = NULL;
    int rc;
    int i;

    struct option options[] = {
        {"help",            no_argument,        NULL,  0 },
        {"nv_data-path",    required_argument,  NULL,  0 },
        {"show-imei",       no_argument,        NULL,  0 },
        {"store-imei",      required_argument,  NULL,  0 },
        {0,                 0,                  NULL,  0 }
    };

    if (argc < 2)
        goto error_help;

    do {
        rc = getopt_long(argc, (char *const *) argv, "", (const struct option *) &options, &i);
        if (rc < 0)
            break;

        if (strcmp(options[i].name, "help") == 0) {
            rc = help();
            goto complete;
        } else if (strcmp(options[i].name, "nv_data-path") == 0) {
            if (optarg == '\0')
                goto error_help;

            nv_data_path = strdup(optarg);
        } else if (strcmp(options[i].name, "show-imei") == 0) {
            show_imei_callback = show_imei;
        }  else if (strcmp(options[i].name, "store-imei") == 0) {
            if (optarg == '\0')
                goto error_help;

            imei = strdup(optarg);
            store_imei_callback = store_imei;
        }
    } while (rc == '\0');

    rc = 0;

    if (show_imei_callback != NULL)
        rc |= show_imei_callback(nv_data_path);

    if (store_imei_callback != NULL)
        rc |= store_imei_callback(nv_data_path, imei);

    goto complete;

error_help:
    help();

error:
    rc = -1;

complete:
    if (nv_data_path != NULL)
        free(nv_data_path);

    if (imei != NULL)
        free(imei);

    return rc;
}

// vim:ts=4:sw=4:expandtab