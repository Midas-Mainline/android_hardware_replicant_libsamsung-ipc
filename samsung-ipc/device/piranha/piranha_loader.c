/*
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 *
 * based on the incomplete C++ implementation which is
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

//for timeval
#include <sys/time.h>

//for mmap
#include <sys/mman.h>
#include <sys/stat.h>

#include "ipc.h"

#include "xmm6260_loader.h"
#include "xmm6260_modemctl.h"

#include "piranha_loader.h"

struct piranha_radio_part piranha_radio_parts[] = {
    {
        .id = PSI,
        .offset = 0x1000,
        .length = 0xe000,
    },
    {
        .id = EBL,
        .offset = 0xf000,
        .length = 0x19000,
    },
    {
        .id = SECURE_IMAGE,
        .offset = 0x9ff800,
        .length = 0x800,
    },
    {
        .id = FIRMWARE,
        .offset = 0x28000,
        .length = 0x9d7800,
    },
    {
        .id = NVDATA,
        .offset = 0xa00000,
        .length = 2 << 20,
    },
};

int piranha_radio_parts_count = sizeof(piranha_radio_parts) / sizeof(struct piranha_radio_part);

struct piranha_boot_cmd piranha_boot_cmds[] = {
    {
        .cmd = SetPortConf,
        .code = 0x86,
        .flags = PIRANHA_FLAG_NONE,
    },
    {
        .cmd = ReqSecStart,
        .code = 0x204,
        .flags = PIRANHA_FLAG_NONE,
    },
    {
        .cmd = ReqSecEnd,
        .code = 0x205,
        .flags = PIRANHA_FLAG_SHORT_TAIL,
    },
    {
        .cmd = ReqForceHwReset,
        .code = 0x208,
        .flags = PIRANHA_FLAG_NO_ACK,
    },
    {
        .cmd = ReqFlashSetAddress,
        .code = 0x802,
        .flags = PIRANHA_FLAG_NONE,
    },
    {
        .cmd = ReqFlashWriteBlock,
        .code = 0x804,
        .flags = PIRANHA_FLAG_SHORT_TAIL,
    },
};

int piranha_boot_cmds_count = sizeof(piranha_boot_cmds) / sizeof(struct piranha_boot_cmd);

struct piranha_radio_part *piranha_radio_part_get(int id)
{
    int i;

    for (i=0 ; i < piranha_radio_parts_count ; i++) {
        if (piranha_radio_parts[i].id == id)
            return &piranha_radio_parts[i];
    }

    return NULL;
}

struct piranha_boot_cmd *piranha_boot_cmd_get(int cmd)
{
    int i;

    for (i=0 ; i < piranha_boot_cmds_count ; i++) {
        if (piranha_boot_cmds[i].cmd == cmd)
            return &piranha_boot_cmds[i];
    }

    return NULL;
}

int piranha_ack_read(struct ipc_client *client, int fd, int ack)
{
    int c, v;
    int rc;

    v = 0;
    for (c=0 ; c < 10 ; c++) {
        rc = expect_read(fd, &v, sizeof(v));
        if ((v & 0xffff) == ack)
            break;
    }

    if (rc <= 0 || (v & 0xffff) != ack)
        return -1;

    return 0;
}

void piranha_boot_cmd_header_fill(struct piranha_boot_cmd_header *header,
    uint16_t code, int size)
{
    memset(header, 0, sizeof(struct piranha_boot_cmd_header));
    header->size = size + sizeof(struct piranha_boot_cmd_header);
    header->magic = PIRANHA_MAGIC_BOOT_CMD_HEADER;
    header->code = code;
    header->data_size = size;
}

void piranha_boot_cmd_tail_fill(struct piranha_boot_cmd_tail *tail,
    uint16_t code, void *data, int size)
{
    unsigned char *p = NULL;
    int i;

    memset(tail, 0, sizeof(struct piranha_boot_cmd_tail));
    tail->checksum = (size & 0xffff) + code;
    tail->magic = PIRANHA_MAGIC_BOOT_CMD_TAIL;
    tail->unknown = PIRANHA_MAGIC_BOOT_CMD_UNKNOWN;

    p = data;

    for (i=0 ; i < size ; i++)
        tail->checksum += *p++;
}

int piranha_boot_cmd_send(struct ipc_client *client, struct modemctl_io_data *io_data,
    int cmd, void *data, int size)
{
    struct piranha_boot_cmd *boot_cmd;
    struct piranha_boot_cmd_header *header_p;
    unsigned char *p;

    struct piranha_boot_cmd_header header;
    struct piranha_boot_cmd_tail tail;
    int tail_length;

    void *cmd_data = NULL;
    int cmd_length = 0;

    void *ack_data = NULL;
    int ack_length = 0;

    int count, length;
    int chunk = 4;

    int rc;

    boot_cmd = piranha_boot_cmd_get(cmd);
    if (boot_cmd == NULL)
        goto fail;

    piranha_boot_cmd_header_fill(&header, boot_cmd->code, size);
    piranha_boot_cmd_tail_fill(&tail, boot_cmd->code, data, size);

    tail_length = sizeof(tail);
    if (boot_cmd->flags & PIRANHA_FLAG_SHORT_TAIL)
        tail_length -= 2;

    cmd_length = sizeof(header) + size + tail_length;
    cmd_data = calloc(1, cmd_length);

    p = (unsigned char *) cmd_data;
    memcpy(p, &header, sizeof(header));
    p += sizeof(header);
    memcpy(p, data, size);
    p += size;
    memcpy(p, &tail, tail_length);

    rc = write(io_data->boot_fd, cmd_data, cmd_length);
    if (rc != cmd_length)
        goto fail;

    if (boot_cmd->flags & PIRANHA_FLAG_NO_ACK)
        goto done;

    rc = expect_read(io_data->boot_fd, &ack_length, sizeof(ack_length));
    if (rc != sizeof(ack_length) || ack_length <= 0)
        goto fail;

    // Add short tail
    ack_length += sizeof(tail) - 2;
    if (ack_length % 4 != 0)
        ack_length += ack_length % 4;
    ack_data = calloc(1, ack_length);

    p = (unsigned char *) ack_data;
    memcpy(p, &ack_length, sizeof(ack_length));
    p += sizeof(ack_length);

    length = ack_length;
    count = sizeof(ack_length);

    while (count < length) {
        rc = expect_read(io_data->boot_fd, p, chunk < length - count ? chunk : length - count);
        if (rc < 0)
            goto fail;

        p += rc;
        count += rc;
    }

    header_p = (struct piranha_boot_cmd_header *) ack_data;
    if (header_p->code != boot_cmd->code)
            goto fail;

    rc = 0;
    goto done;

fail:
    rc = -1;

done:
    if (cmd_data != NULL)
        free(cmd_data);
    if (ack_data != NULL)
        free(ack_data);

    return rc;
}

int piranha_boot_data_send(struct ipc_client *client, struct modemctl_io_data *io_data,
    int address, void *data, int length)
{
    unsigned char *p = NULL;
    int count;
    int chunk = 0xdf2;
    int rc;

    rc = piranha_boot_cmd_send(client, io_data, ReqFlashSetAddress, &address, sizeof(address));
    if (rc < 0)
        goto fail;

    p = (unsigned char *) data;
    count = 0;

    while (count < length) {
        rc = piranha_boot_cmd_send(client, io_data, ReqFlashWriteBlock, p, chunk < length - count ? chunk : length - count);
        if (rc < 0)
            goto fail;

        p += chunk;
        count += chunk;
    }

    return 0;

fail:
    return -1;
}

int piranha_sec_send(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    struct piranha_radio_part *radio_part;
    void *nv_data = NULL;

    uint16_t sec_end_data;
    uint32_t hw_reset_data;

    int length, offset;
    void *data;
    int start, end;
    int rc;

    // SEC start
    ipc_client_log(client, "%s: Sending SEC start image", __func__);

    radio_part = piranha_radio_part_get(SECURE_IMAGE);
    if (radio_part == NULL)
        goto fail;

    length = radio_part->length;
    offset = radio_part->offset;

    data = io_data->radio_data + offset;

    rc = piranha_boot_cmd_send(client, io_data, ReqSecStart, data, length);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send start SEC image", __func__);
        goto fail;
    }

    // Firmware
    ipc_client_log(client, "%s: Sending Firmware image", __func__);

    radio_part = piranha_radio_part_get(FIRMWARE);
    if (radio_part == NULL)
        goto fail;

    length = radio_part->length;
    offset = radio_part->offset;

    data = io_data->radio_data + offset;

    rc = piranha_boot_data_send(client, io_data, PIRANHA_ADDRESS_FIRMWARE, data, length);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send Firmware image", __func__);
        goto fail;
    }

    // NV Data
    ipc_client_log(client, "%s: Sending NV Data image", __func__);

    rc = nv_data_check(client);
    if (rc < 0)
        goto fail;

    rc = nv_data_md5_check(client);
    if (rc < 0)
        goto fail;

    nv_data = ipc_client_file_read(client, nv_data_path(client), 2 << 20, 1024);
    if (nv_data == NULL) {
        ipc_client_log(client, "%s: Failed to read NV Data image (%s)", __func__, nv_data_path(client));
        goto fail;
    }

    rc = piranha_boot_data_send(client, io_data, PIRANHA_ADDRESS_NV_DATA, nv_data, 2 << 20);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send NV Data image", __func__);
        goto fail;
    }

    // SEC end
    ipc_client_log(client, "%s: Sending SEC end", __func__);

    sec_end_data = PIRANHA_MAGIC_BOOT_CMD_SEC_END;
    piranha_boot_cmd_send(client, io_data, ReqSecEnd, &sec_end_data, sizeof(sec_end_data));
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send SEC end", __func__);
        goto fail;
    }

    // HW reset
    ipc_client_log(client, "%s: Sending HW reset", __func__);

    hw_reset_data = PIRANHA_MAGIC_BOOT_CMD_HW_RESET;
    piranha_boot_cmd_send(client, io_data, ReqForceHwReset, &hw_reset_data, sizeof(hw_reset_data));
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send HW reset", __func__);
        goto fail;
    }

    rc = 0;
    goto done;

fail:
    rc = -1;

done:
    if (nv_data != NULL)
        free(nv_data);

    return rc;
}

int piranha_ebl_set(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    unsigned char *data = NULL;
    unsigned char *p = NULL;
    int count, length;
    int chunk = 4;
    int rc;

    rc = expect_read(io_data->boot_fd, &length, sizeof(length));
    if (rc != sizeof(length)) {
        ipc_client_log(client, "%s: Failed to read EBL set length", __func__);
        goto fail;
    }

    data = calloc(1, length);
    p = data;
    count = 0;

    while (count < length) {
        rc = expect_read(io_data->boot_fd, p, chunk < length - count ? chunk : length - count);
        if (rc < 0) {
            ipc_client_log(client, "%s: Failed to read EBL set data", __func__);
            goto fail;
        }

        p += rc;
        count += rc;
    }

    rc = piranha_boot_cmd_send(client, io_data, SetPortConf, data, length);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send EBL set data", __func__);
        goto fail;
    }

    return 0;

fail:
    if (data != NULL)
        free(data);

    return -1;
}

int piranha_ebl_send(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    struct piranha_radio_part *radio_part;
    int length, offset;
    int chunk = 0xdfc;

    int start, end;
    unsigned char crc;

    unsigned char *p;
    int v;
    int rc;

    radio_part = piranha_radio_part_get(EBL);
    if (radio_part == NULL)
        goto fail;

    length = radio_part->length;
    offset = radio_part->offset;
    start = offset;
    end = start + length;

    v = sizeof(length);
    rc = write(io_data->boot_fd, &v, sizeof(v));
    if (rc != sizeof(v)) {
        ipc_client_log(client, "%s: Failed to write EBL length length", __func__);
        goto fail;
    }

    rc = write(io_data->boot_fd, &length, sizeof(length));
    if (rc != sizeof(length)) {
        ipc_client_log(client, "%s: Failed to write EBL length (1)", __func__);
        goto fail;
    }

    rc = piranha_ack_read(client, io_data->boot_fd, PIRANHA_ACK_EBL_LENGTH);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to read EBL length ACK", __func__);
        goto fail;
    }

    v = length;
     p = (unsigned char *) &v;
    *p = 1;

    rc = write(io_data->boot_fd, &v, sizeof(v));
    if (rc != sizeof(v)) {
        ipc_client_log(client, "%s: Failed to write EBL length (2)", __func__);
        goto fail;
    }

    while (start < end) {
        rc = write(io_data->boot_fd, io_data->radio_data + start, chunk < end - start ? chunk : end - start);
        if (rc < 0) {
            ipc_client_log(client, "%s: Failed to write EBL data", __func__);
            goto fail;
        }

        start += rc;
    }

    crc = xmm6260_crc_calculate(io_data->radio_data, offset, length);

    rc = write(io_data->boot_fd, &crc, sizeof(crc));
    if (rc != sizeof(crc)) {
        ipc_client_log(client, "%s: Failed to write EBL CRC (0x%x)", __func__, crc);
        goto fail;
    }

    rc = piranha_ack_read(client, io_data->boot_fd, PIRANHA_ACK_EBL);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to read EBL ACK", __func__);
        goto fail;
    }

    return 0;

fail:
    return -1;
}

int piranha_psi_send(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    struct piranha_radio_part *radio_part;
    int length, offset;
    int chunk = 0xdfc;

    int start, end;
    unsigned char crc;
    uint32_t crc32;

    int v;
    int rc;

    radio_part = piranha_radio_part_get(PSI);
    if (radio_part == NULL)
        goto fail;

    length = radio_part->length;
    offset = radio_part->offset;
    start = offset;
    end = start + length;

    struct piranha_psi_header header = {
        .padding = 0xff,
        .length[0] = (length >> 8) & 0xff,
        .length[1] = length & 0xff,
        .magic = PIRANHA_MAGIC_PSI,
    };

    rc = write(io_data->boot_fd, &header, sizeof(header));
    if (rc != sizeof(header)) {
        ipc_client_log(client, "%s: Failed to write PSI header", __func__);
        goto fail;
    }

    while (start < end) {
        rc = write(io_data->boot_fd, io_data->radio_data + start, chunk < end - start ? chunk : end - start);
        if (rc < 0) {
            ipc_client_log(client, "%s: Failed to write PSI data", __func__);
            goto fail;
        }

        start += rc;
    }

    crc = xmm6260_crc_calculate(io_data->radio_data, offset, length);
    crc32 = (crc << 24) | 0xffffff;

    rc = write(io_data->boot_fd, &crc32, sizeof(crc32));
    if (rc != sizeof(crc32)) {
        ipc_client_log(client, "%s: Failed to write PSI CRC (0x%x)", __func__, crc);
        goto fail;
    }

    rc = piranha_ack_read(client, io_data->boot_fd, PIRANHA_ACK_PSI);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to read PSI ACK", __func__);
        goto fail;
    }

    return 0;

fail:
    return -1;
}

int piranha_modem_bootstrap(struct ipc_client *client)
{
    struct modemctl_io_data io_data;
    unsigned short boot_magic[4];
    int length;
    int c;
    int rc;

    memset(&io_data, 0, sizeof(client, io_data));

    ipc_client_log(client, "%s: Opening radio image (%s)", __func__, PIRANHA_RADIO_IMAGE);

    io_data.radio_fd = open(PIRANHA_RADIO_IMAGE, O_RDONLY);
    if (io_data.radio_fd < 0) {
        ipc_client_log(client, "%s: Failed to open radio image (%s): %s", __func__, PIRANHA_RADIO_IMAGE, strerror(errno));
        goto fail;
    }

    rc = fstat(io_data.radio_fd, &io_data.radio_stat);
    if (rc < 0)
        goto fail;

    io_data.radio_data = mmap(0, RADIO_MAP_SIZE, PROT_READ, MAP_SHARED,
        io_data.radio_fd, 0);
    if (io_data.radio_data == MAP_FAILED) {
        ipc_client_log(client, "%s: Failed to mmap radio image: %s", __func__, strerror(errno));
        goto fail;
    }

    ipc_client_log(client, "%s: Opening boot device (%s)", __func__, PIRANHA_DEV_BOOT0);

    io_data.boot_fd = open(PIRANHA_DEV_BOOT0, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (io_data.boot_fd < 0) {
        ipc_client_log(client, "%s: Failed to open boot device (%s): %s", __func__, PIRANHA_DEV_BOOT0, strerror(errno));
        goto fail;
    }

    ipc_client_log(client, "%s: Setting power off", __func__);

    rc = modemctl_modem_power(client, &io_data, false);
    if (rc < 0)
        goto fail;

    ipc_client_log(client, "%s: Setting power on", __func__);

    rc = modemctl_modem_power(client, &io_data, true);
    if (rc < 0)
        goto fail;

    ipc_client_log(client, "%s: Writing ATAT on boot device", __func__);

    // Boot 0
    for (c=0 ; c < 10 ; c++) {
        if (write(io_data.boot_fd, "ATAT", 4) != 4) {
            rc = -1;
            continue;
        }

        rc = piranha_ack_read(client, io_data.boot_fd, PIRANHA_ACK_BOOT0);
        if (rc < 0)
            continue;
        else
            break;
    }

    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to get boot ACK", __func__);
        goto fail;
    }

    // PSI
    ipc_client_log(client, "%s: Sending PSI", __func__);

    rc = piranha_psi_send(client, &io_data);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send PSI", __func__);
        goto fail;
    }

    close(io_data.boot_fd);

    // Boot 1
    ipc_client_log(client, "%s: Opening boot device (%s)", __func__, PIRANHA_DEV_BOOT1);

    io_data.boot_fd = open(PIRANHA_DEV_BOOT1, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (io_data.boot_fd < 0) {
        ipc_client_log(client, "%s: Failed to open boot device (%s): %s", __func__, PIRANHA_DEV_BOOT1, strerror(errno));
        goto fail;
    }

    boot_magic[0] = boot_magic[1] = 0;
    boot_magic[2] = boot_magic[3] = PIRANHA_MAGIC_BOOT1;
    length = sizeof(boot_magic);

    rc = write(io_data.boot_fd, &length, sizeof(length));
    if (rc != sizeof(length)) {
        ipc_client_log(client, "%s: Failed to write boot magic length", __func__);
        goto fail;
    }

    rc = write(io_data.boot_fd, &boot_magic, sizeof(boot_magic));
    if (rc != sizeof(boot_magic)) {
        ipc_client_log(client, "%s: Failed to write boot magic", __func__);
        goto fail;
    }

    rc = piranha_ack_read(client, io_data.boot_fd, PIRANHA_ACK_BOOT1);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to read boot ACK", __func__);
        goto fail;
    }

    // EBL
    ipc_client_log(client, "%s: Sending EBL", __func__);

    rc = piranha_ebl_send(client, &io_data);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send EBL", __func__);
        goto fail;
    }

    rc = piranha_ebl_set(client, &io_data);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to set PSI", __func__);
        goto fail;
    }

    // SEC images
    ipc_client_log(client, "%s: Sending SEC images", __func__);

    rc = piranha_sec_send(client, &io_data);
    if (rc < 0) {
        ipc_client_log(client, "%s: Failed to send SEC images", __func__);
        goto fail;
    }

    rc = 0;
    goto done;

fail:
    rc = -1;

done:
    if (io_data.radio_data != MAP_FAILED) {
        munmap(io_data.radio_data, RADIO_MAP_SIZE);
    }

    if (io_data.link_fd >= 0) {
        close(io_data.link_fd);
    }

    if (io_data.radio_fd >= 0) {
        close(io_data.radio_fd);
    }

    if (io_data.boot_fd >= 0) {
        close(io_data.boot_fd);
    }

    return rc;
}

// vim:ts=4:sw=4:expandtab
