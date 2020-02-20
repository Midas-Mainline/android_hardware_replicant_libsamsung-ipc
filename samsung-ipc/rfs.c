/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <openssl/md5.h>

#include <samsung-ipc.h>

#include <ipc.h>

char *ipc_nv_data_md5_calculate(struct ipc_client *client,
				const char *path, const char *secret,
				size_t size, size_t chunk_size)
{
    void *data = NULL;
    char *md5_string = NULL;
    unsigned char md5_hash[MD5_DIGEST_LENGTH] = { 0 };
    MD5_CTX ctx;

    if (secret == NULL) {
	    ipc_client_log(client, "%s: Failed: secret is NULL", __FUNCTION__);
	    return NULL;
    }

    data = file_data_read(client, path, size, chunk_size, 0);
    if (data == NULL) {
	    ipc_client_log(client, "%s failed: data is NULL",  __FUNCTION__);
	    return NULL;
    }

    MD5_Init(&ctx);
    MD5_Update(&ctx, data, size);
    MD5_Update(&ctx, secret, strlen(secret));
    MD5_Final((unsigned char *) &md5_hash, &ctx);

    md5_string = data2string(&md5_hash, sizeof(md5_hash));

    return md5_string;
}

int ipc_nv_data_path_check(struct ipc_client *client)
{
    struct stat st;
    char *path;
    off_t size;
    int rc;

    if (client == NULL)
        return -1;

    path = ipc_client_nv_data_path(client);
    size = ipc_client_nv_data_size(client);
    if (path == NULL || size == 0)
        return -1;

    rc = stat(path, &st);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data path failed");
        return -1;
    }

    if (st.st_size != size) {
        ipc_client_log(client, "Checking nv_data size failed");
        return -1;
    }

    ipc_client_log(client, "Checked nv_data path");

    return 0;
}

int ipc_nv_data_md5_path_check(struct ipc_client *client)
{
    struct stat st;
    char *md5_path;
    int rc;

    if (client == NULL)
        return -1;

    md5_path = ipc_client_nv_data_md5_path(client);
    if (md5_path == NULL)
        return -1;

    rc = stat(md5_path, &st);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data md5 path failed");
        return -1;
    }

    if (st.st_size < 2 * sizeof(char) * MD5_DIGEST_LENGTH) {
        ipc_client_log(client, "Checking nv_data md5 size failed");
        return -1;
    }

    ipc_client_log(client, "Checked nv_data md5 path");

    return 0;
}

int ipc_nv_data_backup_path_check(struct ipc_client *client)
{
    struct stat st;
    char *backup_path;
    off_t size;
    int rc;

    if (client == NULL)
        return -1;

    backup_path = ipc_client_nv_data_backup_path(client);
    size = ipc_client_nv_data_size(client);
    if (backup_path == NULL || size == 0)
        return -1;

    rc = stat(backup_path, &st);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup path failed");
        return -1;
    }

    if (st.st_size != size) {
        ipc_client_log(client, "Checking nv_data backup size failed");
        return -1;
    }

    ipc_client_log(client, "Checked nv_data backup path");

    return 0;
}

int ipc_nv_data_backup_md5_path_check(struct ipc_client *client)
{
    struct stat st;
    char *backup_md5_path;
    int rc;

    if (client == NULL)
        return -1;

    backup_md5_path = ipc_client_nv_data_backup_md5_path(client);
    if (backup_md5_path == NULL)
        return -1;

    rc = stat(backup_md5_path, &st);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup md5 path failed");
        return -1;
    }

    if (st.st_size < 2 * sizeof(char) * MD5_DIGEST_LENGTH) {
        ipc_client_log(client, "Checking nv_data backup md5 size failed");
        return -1;
    }

    ipc_client_log(client, "Checked nv_data backup md5 path");

    return 0;
}

int ipc_nv_data_check(struct ipc_client *client)
{
    char *path;
    char *md5_path;
    char *secret;
    size_t size;
    size_t chunk_size;
    char *md5_string = NULL;
    void *buffer = NULL;
    char *string = NULL;
    size_t length;
    int rc;

    if (client == NULL)
        return -1;

    path = ipc_client_nv_data_path(client);
    md5_path = ipc_client_nv_data_md5_path(client);
    secret = ipc_client_nv_data_secret(client);
    size = ipc_client_nv_data_size(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || md5_path == NULL || secret == NULL || size == 0 || chunk_size == 0)
        return -1;

    rc = ipc_nv_data_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data path failed");
        goto error;
    }

    rc = ipc_nv_data_md5_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data md5 path failed");
        goto error;
    }

    md5_string = ipc_nv_data_md5_calculate(client, path, secret, size, chunk_size);
    if (md5_string == NULL) {
        ipc_client_log(client, "Calculating nv_data md5 failed");
        goto error;
    }
    ipc_client_log(client, "Calculated nv_data md5: %s", md5_string);

    length = strlen(md5_string);

    buffer = file_data_read(client, md5_path, length, length, 0);
    if (buffer == NULL) {
        ipc_client_log(client, "Reading nv_data md5 failed");
        goto error;
    }

    string = strndup(buffer, length);
    ipc_client_log(client, "Read nv_data md5: %s", string);

    rc = strncmp(md5_string, string, length);
    if (rc != 0) {
        ipc_client_log(client, "Matching nv_data md5 failed");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (string != NULL)
        free(string);

    if (buffer != NULL)
        free(buffer);

    if (md5_string != NULL)
        free(md5_string);

    return rc;
}

int ipc_nv_data_backup_check(struct ipc_client *client)
{
    char *backup_path;
    char *backup_md5_path;
    char *secret;
    size_t size;
    size_t chunk_size;
    char *backup_md5_string = NULL;
    void *buffer = NULL;
    char *string = NULL;
    size_t length;
    int rc;

    if (client == NULL)
        return -1;

    backup_path = ipc_client_nv_data_backup_path(client);
    backup_md5_path = ipc_client_nv_data_backup_md5_path(client);
    secret = ipc_client_nv_data_secret(client);
    size = ipc_client_nv_data_size(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (backup_path == NULL || backup_md5_path == NULL || secret == NULL || size == 0 || chunk_size == 0)
        return -1;

    rc = ipc_nv_data_backup_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup path failed");
        goto error;
    }

    rc = ipc_nv_data_backup_md5_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup md5 path failed");
        goto error;
    }

    backup_md5_string = ipc_nv_data_md5_calculate(client, backup_path, secret, size, chunk_size);
    if (backup_md5_string == NULL) {
        ipc_client_log(client, "Calculating nv_data backup md5 failed");
        goto error;
    }
    ipc_client_log(client, "Calculated nv_data backup md5: %s", backup_md5_string);

    length = strlen(backup_md5_string);

    buffer = file_data_read(client, backup_md5_path, length, length, 0);
    if (buffer == NULL) {
        ipc_client_log(client, "Reading nv_data backup md5 failed");
        goto error;
    }

    string = strndup(buffer, length);
    ipc_client_log(client, "Read nv_data backup md5: %s", string);

    rc = strncmp(backup_md5_string, string, length);
    if (rc != 0) {
        ipc_client_log(client, "Matching nv_data backup md5 failed");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (string != NULL)
        free(string);

    if (buffer != NULL)
        free(buffer);

    if (backup_md5_string != NULL)
        free(backup_md5_string);

    return rc;
}

int ipc_nv_data_backup(struct ipc_client *client)
{
    void *data = NULL;
    char *path;
    char *backup_path;
    char *backup_md5_path;
    char *secret;
    size_t size;
    size_t chunk_size;
    char *md5_string = NULL;
    size_t length;
    int rc;

    if (client == NULL)
        return -1;

    path = ipc_client_nv_data_path(client);
    backup_path = ipc_client_nv_data_backup_path(client);
    backup_md5_path = ipc_client_nv_data_backup_md5_path(client);
    secret = ipc_client_nv_data_secret(client);
    size = ipc_client_nv_data_size(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || backup_path == NULL || backup_md5_path == NULL || secret == NULL || size == 0 || chunk_size == 0)
        return -1;

    rc = ipc_nv_data_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data path failed");
        goto error;
    }

    data = file_data_read(client, path, size, chunk_size, 0);
    if (data == NULL) {
        ipc_client_log(client, "Reading nv_data failed");
        goto error;
    }

    md5_string = ipc_nv_data_md5_calculate(client, path, secret, size, chunk_size);
    if (md5_string == NULL) {
        ipc_client_log(client, "Calculating nv_data md5 failed");
        goto error;
    }

    length = strlen(md5_string);

    rc = unlink(backup_path);
    if (rc < 0)
        ipc_client_log(client, "Removing nv_data backup path failed");

    rc = file_data_write(client, backup_path, data, size, chunk_size, 0);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data backup failed");
        goto error;
    }

    rc = unlink(backup_md5_path);
    if (rc < 0)
        ipc_client_log(client, "Removing nv_data backup md5 path failed");

    rc = file_data_write(client, backup_md5_path, md5_string, length, length, 0);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data backup md5 failed");
        goto error;
    }

    ipc_client_log(client, "Backed up nv_data");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (md5_string != NULL)
        free(md5_string);

    if (data != NULL)
        free(data);

    return rc;
}

int ipc_nv_data_restore(struct ipc_client *client)
{
    void *data = NULL;
    char *path;
    char *md5_path;
    char *backup_path;
    char *backup_md5_path;
    char *secret;
    size_t size;
    size_t chunk_size;
    size_t length;
    int rc;

    if (client == NULL)
        return -1;

    path = ipc_client_nv_data_path(client);
    md5_path = ipc_client_nv_data_md5_path(client);
    backup_path = ipc_client_nv_data_backup_path(client);
    backup_md5_path = ipc_client_nv_data_backup_md5_path(client);
    secret = ipc_client_nv_data_secret(client);
    size = ipc_client_nv_data_size(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || md5_path == NULL || backup_path == NULL || backup_md5_path == NULL || secret == NULL || size == 0 || chunk_size == 0)
        return -1;

    rc = ipc_nv_data_backup_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup failed");
        goto error;
    }

    data = file_data_read(client, backup_path, size, chunk_size, 0);
    if (data == NULL) {
        ipc_client_log(client, "Reading nv_data backup failed");
        goto error;
    }

    rc = unlink(path);
    if (rc < 0)
        ipc_client_log(client, "Removing nv_data path failed");

    rc = file_data_write(client, path, data, size, chunk_size, 0);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data failed");
        goto error;
    }

    free(data);
    data = NULL;

    length = 2 * sizeof(char) * MD5_DIGEST_LENGTH;

    data = file_data_read(client, backup_md5_path, length, length, 0);
    if (data == NULL) {
        ipc_client_log(client, "Reading nv_data backup md5 failed");
        goto error;
    }

    rc = unlink(md5_path);
    if (rc < 0)
        ipc_client_log(client, "Removing nv_data md5 path failed");

    rc = file_data_write(client, md5_path, data, length, length, 0);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data md5 failed");
        goto error;
    }

    ipc_client_log(client, "Restored nv_data");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (data != NULL)
        free(data);

    return rc;
}

void *ipc_nv_data_load(struct ipc_client *client)
{
    void *data;
    char *path;
    size_t size;
    size_t chunk_size;
    int rc;

    if (client == NULL)
        return NULL;

    path = ipc_client_nv_data_path(client);
    size = ipc_client_nv_data_size(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || size == 0 || chunk_size == 0)
        return NULL;

    rc = ipc_nv_data_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data failed");

        rc = ipc_nv_data_restore(client);
        if (rc < 0) {
            ipc_client_log(client, "Restoring nv_data failed");
            return NULL;
        }

        rc = ipc_nv_data_check(client);
        if (rc < 0) {
            ipc_client_log(client, "Checking nv_data failed");
            return NULL;
        }
    }

    rc = ipc_nv_data_backup_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data backup path failed");

        rc = ipc_nv_data_backup(client);
        if (rc < 0)
            ipc_client_log(client, "Backing up nv_data failed");
    }

    data = file_data_read(client, path, size, chunk_size, 0);
    if (data == NULL) {
        ipc_client_log(client, "Reading nv_data failed");
        return NULL;
    }

    return data;
}

void *ipc_nv_data_read(struct ipc_client *client, size_t size,
    unsigned int offset)
{
    void *data;
    char *path;
    size_t chunk_size;
    int rc;

    if (client == NULL)
        return NULL;

    path = ipc_client_nv_data_path(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || chunk_size == 0)
        return NULL;

    rc = ipc_nv_data_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data path failed");
        return NULL;
    }

    data = file_data_read(client, path, size, chunk_size > size ? size : chunk_size, offset);
    if (data == NULL) {
        ipc_client_log(client, "Reading nv_data failed");
        return NULL;
    }

    return data;
}

int ipc_nv_data_write(struct ipc_client *client, const void *data, size_t size,
    unsigned int offset)
{
    char *path;
    char *md5_path;
    char *secret;
    size_t chunk_size;
    char *md5_string = NULL;
    size_t length;
    int rc;

    if (client == NULL)
        return -1;

    path = ipc_client_nv_data_path(client);
    md5_path = ipc_client_nv_data_md5_path(client);
    secret = ipc_client_nv_data_secret(client);
    chunk_size = ipc_client_nv_data_chunk_size(client);
    if (path == NULL || md5_path == NULL || secret == NULL || chunk_size == 0)
        return -1;

    rc = ipc_nv_data_path_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data path failed");
        goto error;
    }

    rc = file_data_write(client, path, data, size, chunk_size > size ? size : chunk_size, offset);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data failed");
        goto error;
    }

    size = ipc_client_nv_data_size(client);
    if (size == 0)
        goto error;

    md5_string = ipc_nv_data_md5_calculate(client, path, secret, size, chunk_size);
    if (md5_string == NULL) {
        ipc_client_log(client, "Calculating nv_data md5 failed");
        goto error;
    }

    length = strlen(md5_string);

    rc = unlink(md5_path);
    if (rc < 0) {
        ipc_client_log(client, "Removing nv_data md5 path failed");
        goto error;
    }

    rc = file_data_write(client, md5_path, md5_string, length, length, 0);
    if (rc < 0) {
        ipc_client_log(client, "Writing nv_data md5 failed");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (md5_string != NULL)
        free(md5_string);

    return rc;
}

size_t ipc_rfs_nv_data_item_size_setup(struct ipc_rfs_nv_read_item_response_header *header,
    const void *nv_data, size_t nv_size)
{
    size_t size = 0;

    if (header == NULL || nv_data == NULL || nv_size == 0)
        return 0;

    size = sizeof(struct ipc_rfs_nv_read_item_response_header) + size;

    return size;
}

void *ipc_rfs_nv_read_item_setup(struct ipc_rfs_nv_read_item_response_header *header,
    const void *nv_data, size_t nv_size)
{
    void *data;
    size_t size;
    unsigned char *p;

    if (header == NULL || nv_data == NULL || nv_size == 0)
        return NULL;

    size = ipc_rfs_nv_data_item_size_setup(header, nv_data, nv_size);
    if (size == 0)
        return NULL;

    data = calloc(1, size);

    p = (unsigned char *) data;

    memcpy(p, header, sizeof(struct ipc_rfs_nv_read_item_response_header));
    p += sizeof(struct ipc_rfs_nv_read_item_response_header);

    memcpy(p, nv_data, nv_size);
    p += nv_size;

    return data;
}

size_t ipc_rfs_nv_write_item_size_extract(const void *data, size_t size)
{
    struct ipc_rfs_nv_write_item_request_header *header;

    if (data == NULL || size < sizeof(struct ipc_rfs_nv_write_item_request_header))
        return 0;

    header = (struct ipc_rfs_nv_write_item_request_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_rfs_nv_write_item_request_header))
        return 0;

    return header->length;
}

void *ipc_rfs_nv_write_item_extract(const void *data, size_t size)
{
    struct ipc_rfs_nv_write_item_request_header *header;
    void *nv_data;

    if (data == NULL || size < sizeof(struct ipc_rfs_nv_write_item_request_header))
        return NULL;

    header = (struct ipc_rfs_nv_write_item_request_header *) data;
    if (header->length == 0 || header->length > size - sizeof(struct ipc_rfs_nv_write_item_request_header))
        return NULL;

    nv_data = (void *) ((unsigned char *) data + sizeof(struct ipc_rfs_nv_write_item_request_header));

    return nv_data;
}

// vim:ts=4:sw=4:expandtab
