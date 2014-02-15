/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Igor Almeida <igor.contato@gmail.com>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include "phonet.h"

#ifndef __ARIES_H__
#define __ARIES_H__

#define ARIES_MODEM_IMAGE_SIZE                                  0xA00000
#define ARIES_PSI_SIZE                                          0x5000
#define ARIES_ONEDRAM_NV_DATA_OFFSET                            0xD80000
#define ARIES_ONEDRAM_MEMORY_SIZE                               0xFFF000
#define ARIES_ONEDRAM_INIT                                      0x12341234
#define ARIES_ONEDRAM_MAGIC                                     0x45674567
#define ARIES_ONEDRAM_DEINIT                                    0xABCDABCD
#define ARIES_SOCKET_RFS_MAGIC                                  0x80000
#define ARIES_BUFFER_LENGTH                                     4032

#define SO_IPC_RFS                                              0x21
#define ARIES_MODEM_FMT_SPN                                     0x01
#define ARIES_MODEM_RFS_SPN                                     0x41

#define ARIES_MODEM_IMAGE_DEVICE                "/dev/block/bml12"
#define ARIES_MODEM_SERIAL_DEVICE               "/dev/s3c2410_serial3"
#define ARIES_ONEDRAM_DEVICE                    "/dev/onedram"
#define ARIES_MODEMCTL_STATUS_SYSFS             "/sys/class/modemctl/xmm/status"
#define ARIES_MODEMCTL_CONTROL_SYSFS            "/sys/class/modemctl/xmm/control"
#define ARIES_MODEM_IFACE                       "svnet0"
#define ARIES_MODEM_PDP_ACTIVATE_SYSFS          "/sys/class/net/svnet0/pdp/activate"
#define ARIES_MODEM_PDP_DEACTIVATE_SYSFS        "/sys/class/net/svnet0/pdp/deactivate"

#define ARIES_GPRS_IFACE_PREFIX                 "pdp"
#define ARIES_GPRS_IFACE_COUNT                                  3

struct aries_transport_data {
    struct sockaddr_pn spn;
    int fd;
};

extern struct ipc_client_ops aries_fmt_ops;
extern struct ipc_client_ops aries_rfs_ops;
extern struct ipc_client_handlers aries_handlers;
extern struct ipc_client_gprs_specs aries_gprs_specs;
extern struct ipc_client_nv_data_specs aries_nv_data_specs;

#endif

// vim:ts=4:sw=4:expandtab
