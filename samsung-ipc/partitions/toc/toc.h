/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2020 Tony Garnock-Jones <tonyg@leastfixedpoint.com>
 * Copyright (C) 2021 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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

#ifndef __TOC_PARTITION_TABLE_H__
#define __TOC_PARTITION_TABLE_H__

#include <stdint.h>

struct __attribute__((__packed__)) firmware_toc_entry {
	char name[12];
	uint32_t offset;   /* offset within firmware file/partition */
	uint32_t loadaddr; /* target memory address for this blob */
	uint32_t size;     /* size of this blob in bytes */
	uint32_t crc;
	uint32_t entryid;
};

#define N_TOC_ENTRIES (512 / sizeof(struct firmware_toc_entry))

struct firmware_toc_entry const *find_toc_entry(
	char const *name,
	struct firmware_toc_entry const *toc);

#endif /* __TOC_PARTITION_TABLE_H__ */
