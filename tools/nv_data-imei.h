/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2014 Paul Kocialkowsk <contact@paulk.fr>
 * Copyright (C) 2020 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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
#ifndef NV_DATA_IMEI_H
#define NV_DATA_IMEI_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BIT(n) (1<<n)

#define IMEI_LENGTH 15

extern int opterr;

/* This enables to enforce the size limit */
struct imei {
	char imei[IMEI_LENGTH + 1];
	bool option_set;
	char *optarg;
};

#define OFFSET_NEGATIVE BIT(0)
#define OFFSET_OVERFLOW BIT(1)

struct offset {
	size_t offset;
	bool option_set;
	int error;
	char *optarg;
};

/* So far we support only one device and IMEI offset, but more offsets do exist
 * as the original code from Paul Kocialkowski used the 0xE880 offset instead.
 * We don't know yet if multiple offsets can work for one device.
 */
#define DEFAULT_IMEI_OFFSET 0xEC80

struct command {
	const char *name;
	const char *help;
	uint8_t options;
	uint8_t required_options;
	int (*func)(); /* TODO: enfroce argument types */
};

struct command_option {
	uint8_t option;
	const char *option_string;
	const char *help;
	const char *example;
	int (*get_data)(struct command *command, void *arg);
};

#define NO_OPTIONS	0
#define OPTION_FILE	BIT(0)
#define OPTION_HELP	BIT(1)
#define OPTION_IMEI	BIT(2)
#define OPTION_OFFSET	BIT(3)

int bruteforce_imei_offset(char *nv_data_path, struct imei *given_imei);
int read_imei(char *nv_data_path, struct offset *offset);
int write_imei(char *nv_data_path, struct offset *offset, struct imei *imei);

#endif /* NV_DATA_IMEI_H */
