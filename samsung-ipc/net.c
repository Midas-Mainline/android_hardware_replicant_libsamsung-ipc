/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2014 Paul Kocialkowsk <contact@paulk.fr>
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
#include <string.h>

#include <samsung-ipc.h>

int ipc_net_plmn_sel_setup(struct ipc_net_plmn_sel_request_data *data,
    unsigned char mode_sel, const char *plmn, unsigned char act)
{
    size_t plmn_length;

    if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_net_plmn_sel_request_data));
    data->mode_sel = mode_sel;

    if (mode_sel == IPC_NET_PLMN_SEL_AUTO) {
        data->act = IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN;
    } else {
        plmn_length = strlen(plmn);
        if (plmn_length > sizeof(data->plmn))
            plmn_length = sizeof(data->plmn);

        strncpy((char *) data->plmn, plmn, plmn_length);

        // If there are less (5 is the usual case) PLMN bytes, fill the rest with '#'
        if (plmn_length < sizeof(data->plmn))
            memset(data->plmn + plmn_length, '#', sizeof(data->plmn) - plmn_length);

        data->act = act;
    }

    return 0;
}

int ipc_net_regist_setup(struct ipc_net_regist_request_data *data,
    unsigned char domain)
{
    if (data == NULL)
        return -1;

    memset(data, 0, sizeof(struct ipc_net_regist_request_data));
    data->act = IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN;
    data->domain = domain;

    return 0;
}

// vim:ts=4:sw=4:expandtab
