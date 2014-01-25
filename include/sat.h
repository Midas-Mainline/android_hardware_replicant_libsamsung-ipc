/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

#ifndef __SAMSUNG_IPC_SAT_H__
#define __SAMSUNG_IPC_SAT_H__

/*
 * Types
 */

#define IPC_SAT_PROFILE_DOWNLOAD                                0x0E01
#define IPC_SAT_ENVELOPE_CMD                                    0x0E02
#define IPC_SAT_PROACTIVE_CMD                                   0x0E03
#define IPC_SAT_TERMINATE_USAT_SESSION                          0x0E04
#define IPC_SAT_EVENT_DOWNLOAD                                  0x0E05
#define IPC_SAT_PROVIDE_LOCAL_INFO                              0x0E06
#define IPC_SAT_POLLING                                         0x0E07
#define IPC_SAT_REFRESH                                         0x0E08
#define IPC_SAT_SETUP_EVENT_LIST                                0x0E09
#define IPC_SAT_CALL_CONTROL_RESULT                             0x0E0A
#define IPC_SAT_IMAGE_CLUT                                      0x0E0B
#define IPC_SAT_CALL_PROCESSING                                 0x0E0C

#endif

// vim:ts=4:sw=4:expandtab
