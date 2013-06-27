/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 * based on crespo IPC code which is:
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *                    Joerie de Gram <j.de.gram@gmail.com>
 *                    Simon Busch <morphis@gravedo.de>
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

#include "ipc.h"

#include "xmm6260_ipc.h"
#include "galaxys2_loader.h"

struct ipc_ops galaxys2_ipc_fmt_ops = {
    .bootstrap = galaxys2_ipc_bootstrap,
    .send = xmm6260_ipc_fmt_send,
    .recv = xmm6260_ipc_fmt_recv,
};

struct ipc_ops galaxys2_ipc_rfs_ops = {
    .bootstrap = NULL,
    .send = xmm6260_ipc_rfs_send,
    .recv = xmm6260_ipc_rfs_recv,
};

struct ipc_handlers galaxys2_ipc_handlers = {
    .open = xmm6260_ipc_open,
    .close = xmm6260_ipc_close,
    .read = xmm6260_ipc_read,
    .write = xmm6260_ipc_write,
    .poll = xmm6260_ipc_poll,
    .transport_data = NULL,
    .power_on = xmm6260_ipc_power_on,
    .power_off = xmm6260_ipc_power_off,
    .power_data = NULL,
    .gprs_activate = NULL,
    .gprs_deactivate = NULL,
    .gprs_data = NULL,
    .data_create = xmm6260_ipc_data_create,
    .data_destroy = xmm6260_ipc_data_destroy,
};

struct ipc_gprs_specs galaxys2_ipc_gprs_specs = {
    .gprs_get_iface = xmm6260_ipc_gprs_get_iface,
    .gprs_get_capabilities = xmm6260_ipc_gprs_get_capabilities,
};

// vim:ts=4:sw=4:expandtab
