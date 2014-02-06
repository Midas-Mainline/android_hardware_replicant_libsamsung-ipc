/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_PWR_H__
#define __SAMSUNG_IPC_PWR_H__

/*
 * Commands
 */

#define IPC_PWR_PHONE_PWR_UP                                    0x0101
#define IPC_PWR_PHONE_PWR_OFF                                   0x0102
#define IPC_PWR_PHONE_RESET                                     0x0103
#define IPC_PWR_BATT_STATUS                                     0x0104
#define IPC_PWR_BATT_TYPE                                       0x0105
#define IPC_PWR_BATT_COMP                                       0x0106
#define IPC_PWR_PHONE_STATE                                     0x0107

/*
 * Values
 */

#define IPC_PWR_PHONE_STATE_LPM                                 0x0001
#define IPC_PWR_PHONE_STATE_NORMAL                              0x0202

#endif

// vim:ts=4:sw=4:expandtab
