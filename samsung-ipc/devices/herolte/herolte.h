/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2017 Wolfgang Wiedmeyer <wolfgit@wiedmeyer.de>
 * Copyright (C) 2020 Tony Garnock-Jones <tonyg@leastfixedpoint.com>
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

#ifndef __HEROLTE_H__
#define __HEROLTE_H__

struct herolte_transport_data {
	int fd;
};

extern struct ipc_client_ops herolte_fmt_ops;
extern struct ipc_client_ops herolte_rfs_ops;
extern struct ipc_client_handlers herolte_handlers;
extern struct ipc_client_gprs_specs herolte_gprs_specs;
extern struct ipc_client_nv_data_specs herolte_nv_data_specs;

#endif /* __HEROLTE_H__ */
