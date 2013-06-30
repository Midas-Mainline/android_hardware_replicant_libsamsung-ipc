/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2011 Igor Almeida <igor.contato@gmail.com>
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
 *
 */

#include "phonet.h"

#ifndef __ARIES_IPC_H__
#define __ARIES_IPC_H__

#define MODEM_IMAGE_SIZE                                        0xA00000
#define NV_DATA_OFFSET                                          0xD80000
#define ONEDRAM_MEMORY_SIZE                                     0xFFF000
#define ONEDRAM_INIT                                            0x12341234
#define ONEDRAM_MAGIC                                           0x45674567
#define ONEDRAM_DEINIT                                          0xABCDABCD
#define MODEM_DATA_SIZE                                         0x50000
#define MODEM_SOCKET_RFS_MAGIC                                  0x80000

#define SO_IPC_RFS                                              0x21
#define MODEM_FMT_SPN_RESSOURCE                                 0x01
#define MODEM_RFS_SPN_RESSOURCE                                 0x41

#define MODEM_IMAGE_DEVICE                      "/dev/block/bml12"
#define MODEM_SERIAL_DEVICE                     "/dev/s3c2410_serial3"
#define ONEDRAM_DEVICE                          "/dev/onedram"
#define MODEMCTL_STATUS_SYSFS                   "/sys/class/modemctl/xmm/status"
#define MODEMCTL_CONTROL_SYSFS                  "/sys/class/modemctl/xmm/control"
#define MODEM_NETWORK_IFACE                     "svnet0"
#define MODEM_PDP_ACTIVATE_SYSFS                "/sys/class/net/svnet0/pdp/activate"
#define MODEM_PDP_DEACTIVATE_SYSFS              "/sys/class/net/svnet0/pdp/deactivate"

#define GPRS_IFACE_PREFIX                       "pdp"
#define GPRS_IFACE_COUNT                                        3

struct aries_ipc_transport_data {
    struct sockaddr_pn spn;
    int fd;
};

#endif

// vim:ts=4:sw=4:expandtab
