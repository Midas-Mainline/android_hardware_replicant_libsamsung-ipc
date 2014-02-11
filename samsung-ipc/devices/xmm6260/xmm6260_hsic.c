/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on the incomplete C++ implementation which is:
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
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
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include <samsung-ipc.h>
#include <util.h>

#include "xmm6260.h"
#include "xmm6260_hsic.h"

int xmm6260_hsic_ack_read(int device_fd, unsigned short ack)
{
    struct timeval timeout;
    fd_set fds;

    unsigned short value;
    int rc;
    int i;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(device_fd, &fds);

    for (i = 0; i < 50; i++) {
        rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
        if (rc <= 0)
            return -1;

        value = 0;
        rc = read(device_fd, &value, sizeof(value));
        if (rc < (int) sizeof(value))
            continue;

        if (value == ack)
            return 0;
    }

    return -1;
}

int xmm6260_hsic_psi_send(struct ipc_client *client, int device_fd,
    void *psi_data, unsigned short psi_size)
{
    struct xmm6260_hsic_psi_header psi_header;
    char at[] = XMM6260_AT;
    unsigned char psi_ack;
    unsigned char chip_id;
    unsigned char psi_crc;

    struct timeval timeout;
    fd_set fds;
    int wc;

    unsigned char *p;
    int length;
    int rc;
    int i;

    if (client == NULL || device_fd < 0 || psi_data == NULL || psi_size == 0)
        return -1;

    FD_ZERO(&fds);

    i = 0;
    length = strlen(at);

    do {
        FD_SET(device_fd, &fds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        rc = write(device_fd, at, length);
        if (rc < length) {
            ipc_client_log(client, "Writing ATAT in ASCII failed");
            goto error;
        }
        ipc_client_log(client, "Wrote ATAT in ASCII");

        rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
        if (rc < 0) {
            ipc_client_log(client, "Waiting for bootup failed");
            goto error;
        }

        if (i++ > 50) {
            ipc_client_log(client, "Waiting for bootup failed");
            goto error;
        }
    } while(rc == 0);

    FD_SET(device_fd, &fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0) {
        ipc_client_log(client, "Reading chip id failed");
        goto error;
    }

    psi_ack = 0;
    rc = read(device_fd, &psi_ack, sizeof(psi_ack));
    if (rc < 0 || psi_ack != XMM6260_HSIC_BOOT0_ACK) {
        ipc_client_log(client, "Reading boot ACK failed");
        goto error;
    }

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0) {
        ipc_client_log(client, "Reading chip id failed");
        goto error;
    }

    chip_id = 0;
    rc = read(device_fd, &chip_id, sizeof(chip_id));
    if (rc < 0) {
        ipc_client_log(client, "Reading chip id failed");
        goto error;
    }
    ipc_client_log(client, "Read chip id (0x%x)", chip_id);

    psi_header.magic = XMM6260_PSI_MAGIC;
    psi_header.length = psi_size;
    psi_header.padding = XMM6260_PSI_PADDING;

    rc = write(device_fd, &psi_header, sizeof(psi_header));
    if (rc < (int) sizeof(psi_header)) {
        ipc_client_log(client, "Writing PSI header failed");
        goto error;
    }
    ipc_client_log(client, "Wrote PSI header");

    p = (unsigned char *) psi_data;

    wc = 0;
    while (wc < psi_size) {
        rc = write(device_fd, (void *) p, psi_size - wc);
        if (rc < 0) {
            ipc_client_log(client, "Writing PSI failed");
            goto error;
        }

        p += rc;
        wc += rc;
    }

    psi_crc = xmm6260_crc_calculate(psi_data, psi_size);

    ipc_client_log(client, "Wrote PSI, CRC is 0x%x", psi_crc);

    rc = write(device_fd, &psi_crc, sizeof(psi_crc));
    if (rc < (int) sizeof(psi_crc)) {
        ipc_client_log(client, "Writing PSI CRC failed");
        goto error;
    }
    ipc_client_log(client, "Wrote PSI CRC (0x%x)", psi_crc);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    for (i = 0; i < XMM6260_HSIC_PSI_UNKNOWN_COUNT; i++) {
        rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
        if (rc <= 0) {
            ipc_client_log(client, "Reading PSI unknown failed");
            goto error;
        }

        rc = read(device_fd, &psi_ack, sizeof(psi_ack));
        if (rc < (int) sizeof(psi_ack)) {
            ipc_client_log(client, "Reading PSI unknown failed");
            goto error;
        }
    }

    for (i = 0; i < XMM6260_HSIC_PSI_CRC_ACK_COUNT ; i++) {
        rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
        if (rc <= 0) {
            ipc_client_log(client, "Reading PSI CRC ACK failed");
            goto error;
        }

        rc = read(device_fd, &psi_ack, sizeof(psi_ack));
        if (rc < (int) sizeof(psi_ack) || psi_ack != XMM6260_HSIC_PSI_CRC_ACK) {
            ipc_client_log(client, "Reading PSI CRC ACK failed");
            goto error;
        }
    }
    ipc_client_log(client, "Read PSI CRC ACK");

    rc = xmm6260_hsic_ack_read(device_fd, XMM6260_HSIC_PSI_ACK);
    if (rc < 0) {
        ipc_client_log(client, "Reading PSI ACK failed");
        goto error;
    }
    ipc_client_log(client, "Read PSI ACK");

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    return rc;
}

int xmm6260_hsic_ebl_send(struct ipc_client *client, int device_fd,
    void *ebl_data, int ebl_size)
{
    unsigned char ebl_crc;

    int chunk;
    int count;
    int wc;

    unsigned char *p;
    int length;
    int rc;

    if (client == NULL || device_fd < 0 || ebl_data == NULL || ebl_size <= 0)
        return -1;

    length = sizeof(ebl_size);

    rc = write(device_fd, &ebl_size, length);
    if (rc < length) {
        ipc_client_log(client, "Writing EBL size failed");
        goto error;
    }
    ipc_client_log(client, "Wrote EBL size");

    rc = xmm6260_hsic_ack_read(device_fd, XMM6260_HSIC_EBL_SIZE_ACK);
    if (rc < 0) {
        ipc_client_log(client, "Reading EBL size ACK failed");
        goto error;
    }

    p = (unsigned char *) ebl_data;

    chunk = XMM6260_HSIC_EBL_CHUNK;
    wc = 0;
    while (wc < ebl_size) {
        count = chunk < ebl_size - wc ? chunk : ebl_size - wc;

        rc = write(device_fd, (void *) p, count);
        if (rc < 0) {
            ipc_client_log(client, "Writing EBL failed");
            goto error;
        }

        p += rc;
        wc += rc;
    }

    ebl_crc = xmm6260_crc_calculate(ebl_data, ebl_size);

    ipc_client_log(client, "Wrote EBL, CRC is 0x%x", ebl_crc);

    rc = write(device_fd, &ebl_crc, sizeof(ebl_crc));
    if (rc < (int) sizeof(ebl_crc)) {
        ipc_client_log(client, "Writing EBL CRC failed");
        goto error;
    }
    ipc_client_log(client, "Wrote EBL CRC (0x%x)", ebl_crc);

    rc = xmm6260_hsic_ack_read(device_fd, XMM6260_HSIC_EBL_ACK);
    if (rc < 0) {
        ipc_client_log(client, "Reading EBL ACK failed");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    return rc;
}

int xmm6260_hsic_command_send(int device_fd, unsigned short code,
    void *data, int size, int command_data_size, int ack)
{
    struct xmm6260_hsic_command_header header;
    void *buffer = NULL;
    int length;

    struct timeval timeout;
    fd_set fds;

    unsigned char *p;
    int rc;
    int i;

    if (device_fd < 0 || data == NULL || size <= 0 || command_data_size < size)
        return -1;

    header.checksum = (size & 0xffff) + code;
    header.code = code;
    header.data_size = size;

    p = (unsigned char *) data;

    for (i = 0; i < size; i++)
        header.checksum += *p++;

    length = command_data_size + sizeof(header);
    buffer = malloc(length);

    memset(buffer, 0, length);
    p = (unsigned char *) buffer;
    memcpy(p, &header, sizeof(header));
    p += sizeof(header);
    memcpy(p, data, size);

    rc = write(device_fd, buffer, length);
    if (rc < length)
        goto error;

    if (!ack) {
        rc = 0;
        goto complete;
    }

    memset(buffer, 0, length);

    FD_ZERO(&fds);
    FD_SET(device_fd, &fds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0)
        goto error;

    rc = read(device_fd, &header, sizeof(header));
    if (rc < (int) sizeof(header))
        goto error;

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0)
        goto error;

    rc = read(device_fd, buffer, command_data_size);
    if (rc < command_data_size)
        goto error;

    if (header.code != code)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_hsic_modem_data_send(int device_fd, void *data, int size, int address)
{
    int chunk;
    int count;
    int c;

    unsigned char *p;
    int rc;

    if (device_fd < 0 || data == NULL || size <= 0)
        return -1;

    rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_FLASH_SET_ADDRESS, &address, sizeof(address), XMM6260_HSIC_FLASH_SET_ADDRESS_SIZE, 1);
    if (rc < 0)
        goto error;

    p = (unsigned char *) data;

    chunk = XMM6260_HSIC_MODEM_DATA_CHUNK;
    c = 0;
    while (c < size) {
        count = chunk < size - c ? chunk : size - c;

        rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_FLASH_WRITE_BLOCK, p, count, XMM6260_HSIC_FLASH_WRITE_BLOCK_SIZE, 0);
        if (rc < 0)
            goto error;

        p += count;
        c += count;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    return rc;
}

int xmm6260_hsic_port_config_send(struct ipc_client *client, int device_fd)
{
    void *buffer = NULL;
    int length;

    struct timeval timeout;
    fd_set fds;

    int rc;

    if (client == NULL || device_fd < 0)
        return -1;

    FD_ZERO(&fds);
    FD_SET(device_fd, &fds);

    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0)
        goto error;

    length = XMM6260_HSIC_PORT_CONFIG_SIZE;
    buffer = malloc(length);

    rc = select(device_fd + 1, &fds, NULL, NULL, &timeout);
    if (rc <= 0)
        goto error;

    rc = read(device_fd, buffer, length);
    if (rc < length) {
        ipc_client_log(client, "Reading port config failed");
        goto error;
    }
    ipc_client_log(client, "Read port config");

    rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_SET_PORT_CONFIG, buffer, length, XMM6260_HSIC_SET_PORT_CONFIG_SIZE, 1);
    if (rc < 0) {
        ipc_client_log(client, "Sending port config command failed");
        goto error;
    }

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (buffer != NULL)
        free(buffer);

    return rc;
}

int xmm6260_hsic_sec_start_send(struct ipc_client *client, int device_fd,
    void *sec_data, int sec_size)
{
    int rc;

    if (client == NULL || device_fd < 0 || sec_data == NULL || sec_size <= 0)
        return -1;

    rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_SEC_START, sec_data, sec_size, XMM6260_HSIC_SEC_START_SIZE, 1);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_hsic_sec_end_send(struct ipc_client *client, int device_fd)
{
    unsigned short sec_data;
    int sec_size;
    int rc;

    if (client == NULL || device_fd < 0)
        return -1;

    sec_data = XMM6260_SEC_END_MAGIC;
    sec_size = sizeof(sec_data);

    rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_SEC_END, &sec_data, sec_size, XMM6260_HSIC_SEC_END_SIZE, 1);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_hsic_firmware_send(struct ipc_client *client, int device_fd,
    void *firmware_data, int firmware_size)
{
    int rc;

    if (client == NULL || device_fd < 0 || firmware_data == NULL || firmware_size <= 0)
        return -1;

    rc = xmm6260_hsic_modem_data_send(device_fd, firmware_data, firmware_size, XMM6260_FIRMWARE_ADDRESS);
    if (rc < 0)
        return -1;

    return 0;
}

int xmm6260_hsic_nv_data_send(struct ipc_client *client, int device_fd)
{
    void *nv_data = NULL;
    int nv_size;
    int rc;

    if (client == NULL || device_fd < 0)
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

    nv_data = file_data_read(ipc_client_nv_data_path(client), ipc_client_nv_data_size(client), ipc_client_nv_data_chunk_size(client));
    if (nv_data == NULL) {
        ipc_client_log(client, "Reading nv_data failed");
        goto error;
    }
    ipc_client_log(client, "Read nv_data");

    nv_size = ipc_client_nv_data_size(client);

    rc = xmm6260_hsic_modem_data_send(device_fd, nv_data, nv_size, XMM6260_NV_DATA_ADDRESS);
    if (rc < 0)
        goto error;

    rc = 0;
    goto complete;

error:
    rc = -1;

complete:
    if (nv_data != NULL)
        free(nv_data);

    return rc;
}

int xmm6260_hsic_hw_reset_send(struct ipc_client *client, int device_fd)
{
    unsigned int hw_reset_data;
    int hw_reset_size;
    int rc;

    if (client == NULL || device_fd < 0)
        return -1;

    hw_reset_data = XMM6260_HW_RESET_MAGIC;
    hw_reset_size = sizeof(hw_reset_data);

    rc = xmm6260_hsic_command_send(device_fd, XMM6260_COMMAND_HW_RESET, &hw_reset_data, hw_reset_size, XMM6260_HSIC_HW_RESET_SIZE, 0);
    if (rc < 0)
        return -1;

    return 0;
}

// vim:ts=4:sw=4:expandtab
