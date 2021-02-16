/*
 * This file is part of libsamsung-ipc.
 *
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

#include <stddef.h>
#include <string.h>

#include "toc.h"

struct firmware_toc_entry const *find_toc_entry(
	char const *name,
	struct firmware_toc_entry const *toc)
{
	unsigned int index;

	/* We don't know all the details of the TOC format yet; for now, we
	 * assume two things:
	 * 1. reading 512 bytes of TOC is enough, and
	 * 2. the first entry with an empty name field ends the list.
	 */
	for (index = 0; index < N_TOC_ENTRIES; index++) {
		if (toc[index].name[0] == '\0')
			break;
		if (strncmp(toc[index].name, name,
			    sizeof(toc[index].name)) == 0)
			return &toc[index];
	}
	return NULL;
}
