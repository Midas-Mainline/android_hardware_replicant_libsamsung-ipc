/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * based on crespo IPC code which is:
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef __XMM6260_IPC_H__
#define __XMM6260_IPC_H__

extern struct ipc_handlers xmm6260_ipc_default_handlers;

struct rfs_hdr {
        uint32_t size;
        uint8_t cmd;
        uint8_t id;
} __attribute__ ((packed));

#endif

// vim:ts=4:sw=4:expandtab
