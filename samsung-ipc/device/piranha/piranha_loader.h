/*
 * Firmware loader for Samsung I9100 (galaxys2)
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

#ifndef __PIRANHA_LOADER_H__
#define __PIRANHA_LOADER_H__

#define PIRANHA_RADIO_IMAGE             "/dev/block/mmcblk0p8"
#define PIRANHA_DEV_BOOT0               "/dev/umts_boot0"
#define PIRANHA_DEV_BOOT1               "/dev/umts_boot1"

#define PIRANHA_MAGIC_PSI               0x30
#define PIRANHA_MAGIC_BOOT1             0x02
#define PIRANHA_MAGIC_BOOT_CMD_HEADER   0x02
#define PIRANHA_MAGIC_BOOT_CMD_TAIL     0x03
#define PIRANHA_MAGIC_BOOT_CMD_UNKNOWN  0xeaea
#define PIRANHA_MAGIC_BOOT_CMD_SEC_END  0x0000
#define PIRANHA_MAGIC_BOOT_CMD_HW_RESET 0x00111001

#define PIRANHA_ACK_BOOT0               0xffff
#define PIRANHA_ACK_PSI                 0xdd01
#define PIRANHA_ACK_BOOT1               0xaa00
#define PIRANHA_ACK_EBL_LENGTH          0xcccc
#define PIRANHA_ACK_EBL                 0xa551

#define PIRANHA_ADDRESS_FIRMWARE        0x60300000
#define PIRANHA_ADDRESS_NV_DATA         0x60e80000

#define PIRANHA_FLAG_NONE               (0)
#define PIRANHA_FLAG_SHORT_TAIL         (1 << 0)
#define PIRANHA_FLAG_NO_ACK             (1 << 1)

struct piranha_radio_part {
    int id;
    size_t offset;
    size_t length;
};

struct piranha_boot_cmd {
    int cmd;
    uint16_t code;
    int flags;
};

struct piranha_psi_header {
    uint8_t padding;
    uint8_t length[2];
    uint8_t magic;
} __attribute__((packed));

struct piranha_boot_cmd_header {
    uint32_t size;
    uint16_t magic;
    uint16_t code;
    uint16_t data_size;
} __attribute__((packed));

struct piranha_boot_cmd_tail {
    uint16_t checksum;
    uint16_t magic;
    uint16_t unknown;
} __attribute__((packed));

int piranha_modem_bootstrap(struct ipc_client *client);

#endif

// vim:ts=4:sw=4:expandtab
