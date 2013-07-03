/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
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

#include <stdlib.h>
#include <sys/ioctl.h>

#include "xmm6260.h"

unsigned char xmm6260_crc_calculate(void *buffer, int length)
{
    unsigned char crc;
    unsigned char *p;

    if (buffer == NULL || length <= 0)
        return 0;

    p = (unsigned char *) buffer;

    crc = 0;
    while (length--)
        crc ^= *p++;

    return crc;
}

// vim:ts=4:sw=4:expandtab
