/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
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

#include <radio.h>

#include "ipc_devices.h"

struct ipc_device_desc ipc_devices[] = {
    { "galaxys2", "i9100",  &xmm6260_default_handlers,   &xmm6260_i9100_fmt_ops, &xmm6260_rfs_ops },
    { "galaxys2", "smdk4210",  &xmm6260_default_handlers,    &xmm6260_i9100_fmt_ops,  &xmm6260_rfs_ops },
    { "maguro", "tuna",  &xmm6260_default_handlers,    &xmm6260_i9250_fmt_ops,    &xmm6260_rfs_ops },
    { "crespo", "herring",  &crespo_default_handlers,   &crespo_fmt_ops,    &crespo_rfs_ops },
    { "aries",  "aries",    &aries_default_handlers,    &aries_fmt_ops,     &aries_rfs_ops },
    { "aries",  "gt-p1000", &aries_default_handlers,    &aries_fmt_ops,     &aries_rfs_ops }
};

int ipc_devices_count = sizeof(ipc_devices) / sizeof(struct ipc_device_desc);

// vim:ts=4:sw=4:expandtab
