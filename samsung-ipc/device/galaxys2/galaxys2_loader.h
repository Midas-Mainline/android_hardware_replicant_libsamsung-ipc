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

#ifndef __GALAXYS2_LOADER_H__
#define __GALAXYS2_LOADER_H__

#define RADIO_IMAGE "/dev/block/mmcblk0p8"
#define I9100_EHCI_PATH "/sys/devices/platform/s5p-ehci/ehci_power"

#define LINK_POLL_DELAY_US (50 * 1000)
#define LINK_TIMEOUT_MS 2000

#define XMM_PSI_MAGIC 0x30
#define PSI_ACK_MAGIC "\x00\xaa"

#define EBL_HDR_ACK_MAGIC "\xcc\xcc"
#define EBL_IMG_ACK_MAGIC "\x51\xa5"

#define BL_END_MAGIC "\x00\x00"
#define BL_END_MAGIC_LEN 2

#define BL_RESET_MAGIC "\x01\x10\x11\x00"
#define BL_RESET_MAGIC_LEN 4

#define SEC_DOWNLOAD_CHUNK 16384
#define SEC_DOWNLOAD_DELAY_US (500 * 1000)

#define POST_BOOT_TIMEOUT_US (600 * 1000)

#define FW_LOAD_ADDR 0x60300000
#define NVDATA_LOAD_ADDR 0x60e80000

struct galaxys2_boot_cmd_desc {
    unsigned code;
    size_t data_size;
    bool need_ack;
};

struct galaxys2_psi_header {
    uint8_t magic;
    uint16_t length;
    uint8_t padding;
} __attribute__((packed));

struct galaxys2_boot_info {
    uint8_t data[76];
} __attribute__((packed));

struct galaxys2_boot_cmd {
    uint16_t check;
    uint16_t cmd;
    uint32_t data_size;
} __attribute__((packed));

int galaxys2_ipc_bootstrap(struct ipc_client *client);

#endif

// vim:ts=4:sw=4:expandtab
