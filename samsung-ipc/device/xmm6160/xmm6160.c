/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>

#include <samsung-ipc.h>
#include <util.h>

#include "xmm6160.h"

int xmm6160_psi_send(struct ipc_client *client, int serial_fd,
    void *modem_image_data, int modem_image_size)
{
    char at[] = XMM6160_AT;
    unsigned char version;
    unsigned char info;
    unsigned char psi_magic;
    unsigned short psi_size;
    unsigned char psi_crc;
    unsigned char psi_ack;

    struct termios termios;
    struct timeval timeout;
    fd_set fds;

    unsigned char *p;
    int length;
    int rc;
    int i;

    if (client == NULL || serial_fd < 0 || modem_image_data == NULL || modem_image_size < XMM6160_PSI_SIZE)
        return -1;

    tcgetattr(serial_fd, &termios);

    cfmakeraw(&termios);
    cfsetispeed(&termios, B115200);
    cfsetospeed(&termios, B115200);

    tcsetattr(serial_fd, TCSANOW, &termios);

    length = strlen(at);
    for (i=0; i < XMM6160_AT_COUNT; i++) {
        rc = write(serial_fd, at, length);
        if (rc < length) {
            ipc_client_log(client, "Writing AT in ASCII failed");
            goto error;
        }

        usleep(50000);
    }
    ipc_client_log(client, "Wrote AT in ASCII");

    usleep(50000);

    version = 0;

    rc = read(serial_fd, &version, sizeof(version));
    if (rc < (int) sizeof(version)) {
        ipc_client_log(client, "Reading bootcore version failed");
        goto error;
    }

    if (version != XMM6160_BOOTCORE_VERSION) {
        ipc_client_log(client, "Read wrong bootcore version (0x%x)", version);
        goto error;
    }

    ipc_client_log(client, "Read bootcore version (0x%x)", version);

    rc = read(serial_fd, &info, sizeof(info));
    if (rc < (int) sizeof(info)) {
        ipc_client_log(client, "Reading info size failed");
        goto error;
    }
    ipc_client_log(client, "Read info size (0x%x)", info);

    psi_magic = XMM6160_PSI_MAGIC;

    rc = write(serial_fd, &psi_magic, sizeof(psi_magic));
    if (rc < (int) sizeof(psi_magic)) {
        ipc_client_log(client, "Writing PSI magic failed");
        goto error;
    }
    ipc_client_log(client, "Wrote PSI magic (0x%x)", psi_magic);

    psi_size = XMM6160_PSI_SIZE;

    rc = write(serial_fd, &psi_size, sizeof(psi_size));
    if (rc < (int) sizeof(psi_size)) {
        ipc_client_log(client, "Writing PSI size failed");
        goto error;
    }
    ipc_client_log(client, "Wrote PSI size (0x%x)", psi_size);

    FD_ZERO(&fds);
    FD_SET(serial_fd, &fds);

    timeout.tv_sec = 4;
    timeout.tv_usec = 0;

    p = (unsigned char *) modem_image_data;
    psi_crc = 0;

    for (i=0; i < XMM6160_PSI_SIZE; i++) {
        rc = select(serial_fd + 1, NULL, &fds, NULL, &timeout);
        if (rc <= 0) {
            ipc_client_log(client, "Writing PSI failed");
            goto error;
        }

        rc = write(serial_fd, p, 1);
        if (rc < 1) {
            ipc_client_log(client, "Writing PSI failed");
            goto error;
        }

        psi_crc ^= *p++;
    }
    ipc_client_log(client, "Wrote PSI, CRC is 0x%x", psi_crc);

    rc = select(serial_fd + 1, NULL, &fds, NULL, &timeout);
    if (rc <= 0) {
        ipc_client_log(client, "Writing PSI crc failed");
        goto error;
    }

    rc = write(serial_fd, &psi_crc, sizeof(psi_crc));
    if (rc < (int) sizeof(psi_crc)) {
        ipc_client_log(client, "Writing PSI crc failed");
        goto error;
    }
    ipc_client_log(client, "Wrote PSI CRC (0x%x)", psi_crc);

    timeout.tv_sec = 4;
    timeout.tv_usec = 0;

    i = 0;
    do {
        rc = select(serial_fd + 1, &fds, NULL, NULL, &timeout);
        if (rc <= 0) {
            ipc_client_log(client, "Reading PSI ACK failed");
            goto error;
        }

        rc = read(serial_fd, &psi_ack, sizeof(psi_ack));
        if (rc < (int) sizeof(psi_ack)) {
            ipc_client_log(client, "Reading PSI ACK failed");
            goto error;
        }

        if (i++ > 50) {
            ipc_client_log(client, "Reading PSI ACK failed");
            goto error;
        }
    } while (psi_ack != XMM6160_PSI_ACK);
    ipc_client_log(client, "Read PSI ACK (0x%x)", psi_ack);

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    return rc;
}

int xmm6160_modem_image_send(struct ipc_client *client, int device_fd,
    void *device_address, void *modem_image_data, int modem_image_size, int modem_image_offset)
{
    int wc;

    unsigned char *p;
    int length;
    int rc;
    int i;

    if (client == NULL || (device_fd < 0 && device_address == NULL) || modem_image_data == NULL || modem_image_size <= XMM6160_PSI_SIZE)
        return -1;

    p = (unsigned char *) modem_image_data + XMM6160_PSI_SIZE;
    length = modem_image_size - XMM6160_PSI_SIZE;

    if (device_address != NULL) {
        memcpy((void *) ((unsigned char *) device_address + modem_image_offset), p, length);
    } else {
        lseek(device_fd, modem_image_offset, SEEK_SET);

        wc = 0;
        while (wc < length) {
            rc = write(device_fd, p, length - wc);
            if (rc < 0) {
                ipc_client_log(client, "Writing modem image failed");
                goto error;
            }

            p += rc;
            wc += rc;
        }
    }
    ipc_client_log(client, "Wrote modem image");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    return rc;
}

int xmm6160_nv_data_send(struct ipc_client *client, int device_fd,
    void *device_address, int modem_image_offset)
{
    void *nv_data = NULL;
    int wc;

    unsigned char *p;
    int length;
    int rc;

    if (client == NULL || (device_fd < 0 && device_address == NULL))
        return -1;

    rc = nv_data_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Checked nv_data");

    rc = nv_data_md5_check(client);
    if (rc < 0) {
        ipc_client_log(client, "Checking nv_data md5 failed");
        goto error;
    }
    ipc_client_log(client, "Checked nv_data md5");

    nv_data = file_data_read(nv_data_path(client), nv_data_size(client), nv_data_chunk_size(client));
    if (nv_data == NULL) {
        ipc_client_log(client, "Reading nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Read nv_data");

    p = (unsigned char *) nv_data;
    length = nv_data_size(client);

    if (device_address != NULL) {
        memcpy((void *) ((unsigned char *) device_address + modem_image_offset), p, length);
    } else {
        lseek(device_fd, modem_image_offset, SEEK_SET);

        wc = 0;
        while (wc < length) {
            rc = write(device_fd, p, length - wc);
            if (rc < 0) {
                ipc_client_log(client, "Writing modem image failed");
                goto error;
            }

            p += rc;
            wc += rc;
        }
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (nv_data != NULL)
        free(nv_data);

    return rc;
}

// vim:ts=4:sw=4:expandtab
