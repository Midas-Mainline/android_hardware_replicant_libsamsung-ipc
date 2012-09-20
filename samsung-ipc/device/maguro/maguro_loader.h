/*
 * Firmware loader for Samsung I9250 (maguro)
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __MAGURO_LOADER_H__
#define __MAGURO_LOADER_H__

#define I9250_SECOND_BOOT_DEV "/dev/umts_boot1"

#define I9250_BOOT_LAST_MASK 0x0000ffff
#define I9250_BOOT_REPLY_MAX 20

#define I9250_GENERAL_ACK "\x02\x00\x00\x00"

#define I9250_PSI_START_MAGIC "\xff\xf0\x00\x30"
#define I9250_PSI_CMD_EXEC "\x08\x00\x00\x00"
#define I9250_PSI_EXEC_DATA "\x00\x00\x00\x00\x02\x00\x02\x00"
#define I9250_PSI_READY_ACK "\x00\xaa\x00\x00"

#define I9250_EBL_IMG_ACK_MAGIC "\x51\xa5\x00\x00"
#define I9250_EBL_HDR_ACK_MAGIC "\xcc\xcc\x00\x00"

#define I9250_MPS_IMAGE_PATH "/factory/imei/mps_code.dat"
#define I9250_MPS_LOAD_ADDR 0x61080000
#define I9250_MPS_LENGTH 3

#define SEC_DOWNLOAD_CHUNK 0xdfc2
#define SEC_DOWNLOAD_DELAY_US (500 * 1000)

	#define FW_LOAD_ADDR 0x60300000
#define NVDATA_LOAD_ADDR 0x60e80000

#define BL_END_MAGIC "\x00\x00"
#define BL_END_MAGIC_LEN 2

#define BL_RESET_MAGIC "\x01\x10\x11\x00"
#define BL_RESET_MAGIC_LEN 4

/*
 * on I9250, all commands need ACK and we do not need to
 * allocate a fixed size buffer
 */
struct maguro_boot_cmd_desc {
    unsigned code;
    bool long_tail;
    bool no_ack;
};

struct maguro_boot_cmd_header {
    uint32_t total_size;
    uint16_t hdr_magic;
    uint16_t cmd;
    uint16_t data_size;
} __attribute__((packed));

#define DECLARE_BOOT_CMD_HEADER(name, code, size) \
struct maguro_boot_cmd_header name = {\
    .total_size = size + 10,\
    .hdr_magic = 2,\
    .cmd = code,\
    .data_size = size,\
}

struct maguro_boot_tail_header {
    uint16_t checksum;
    uint16_t tail_magic;
    uint8_t unknown[2];
} __attribute__((packed));

#define DECLARE_BOOT_TAIL_HEADER(name, checksum) \
struct maguro_boot_tail_header name = {\
    .checksum = checksum,\
    .tail_magic = 3,\
    .unknown = "\xea\xea",\
}

int maguro_modem_bootstrap(struct ipc_client *client);
int maguro_power_off(void *io_data);

#endif

// vim:ts=4:sw=4:expandtab
