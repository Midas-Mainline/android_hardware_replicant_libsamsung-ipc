/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com
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

void ipc_net_regist_get(struct ipc_net_regist_get *message, int domain)
{
	message->net = 0xff;
	message->domain = domain;
}

void ipc_net_plmn_sel_setup(struct ipc_net_plmn_sel *message, unsigned char mode, unsigned char *plmn)
{
    if (mode == IPC_NET_PLMN_SEL_MODE_AUTO)
    {
        message->mode = IPC_NET_PLMN_SEL_MODE_AUTO;
        message->unk1 = 0xff;
    }
    else if (mode == IPC_NET_PLMN_SEL_MODE_MANUAL)
    {
        message->mode = IPC_NET_PLMN_SEL_MODE_MANUAL;
        strncpy(message->plmn, plmn, 5);
        message->unk0 = 0x23;
        message->unk1 = 0x4;
    }
}

// vim:ts=4:sw=4:expandtab
