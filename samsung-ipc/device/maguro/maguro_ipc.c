/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *
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

#include "ipc_private.h"

#include "xmm6260_ipc.h"
#include "maguro_loader.h"

struct ipc_ops maguro_fmt_ops = {
    .send = xmm6260_ipc_fmt_client_send,
    .recv = xmm6260_ipc_fmt_client_recv,
    .bootstrap = maguro_modem_bootstrap,
};

struct ipc_ops maguro_rfs_ops = {
    .send = xmm6260_ipc_rfs_client_send,
    .recv = xmm6260_ipc_rfs_client_recv,
    .bootstrap = NULL,
};

struct ipc_handlers maguro_default_handlers = {
    .read = xmm6260_ipc_read,
    .write = xmm6260_ipc_write,
    .open = xmm6260_ipc_open,
    .close = xmm6260_ipc_close,
    .power_on = xmm6260_ipc_power_on,
    .power_off = maguro_power_off,
    .common_data = NULL,
    .common_data_create = xmm6260_ipc_common_data_create,
    .common_data_destroy = xmm6260_ipc_common_data_destroy,
    .common_data_set_fd = xmm6260_ipc_common_data_set_fd,
    .common_data_get_fd = xmm6260_ipc_common_data_get_fd,
};

struct ipc_gprs_specs maguro_gprs_specs = {
    .gprs_get_iface = xmm6260_ipc_gprs_get_iface,
    .gprs_get_capabilities = xmm6260_ipc_gprs_get_capabilities,
};

struct ipc_nv_data_specs maguro_nv_data_specs = {
    .nv_data_path = "/factory/nv_data.bin",
    .nv_data_md5_path = "/factory/nv_data.bin.md5",
    .nv_state_path = "/factory/.nv_state",
    .nv_data_bak_path = "/factory/.nv_data.bak",
    .nv_data_md5_bak_path = "/factory/.nv_data.bak.md5",
};
