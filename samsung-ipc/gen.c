/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011-2014 Paul Kocialkowsk <contact@paulk.fr>
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

#include <samsung-ipc.h>

int ipc_gen_phone_res_check(const struct ipc_gen_phone_res_data *data)
{
	if (data == NULL)
		return -1;

	if (data->code & IPC_GEN_PHONE_RES_CODE_SUCCESS)
		return 0;

	return -1;
}
