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

#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <samsung-ipc.h>

#include "../samsung-ipc/modems/xmm616/xmm616.h"

#include "nv_data-imei.h"

#define DEBUG 0

#if DEBUG
static int print_offset(struct offset *offset)
{
	printf("offset @ %p: {\n", offset);
	printf("\tsize_t offset: 0x%x\n", offset->offset);
	printf("\tbool option_set: %s\n",
	       offset->option_set ? "True" : "False");
	printf("\tint error: %d\n", offset->error);
	printf("\tchar *optarg: %s\n", offset->optarg);
	printf("}\n");

	return 0;
}

static int print_imei(struct imei *imei)
{
	printf("imei @ %p: {\n", imei);
	printf("\tchar imei[%d + 1]: %s\n", IMEI_LENGTH, imei->imei);
	printf("\tbool option_set: %s\n", imei->option_set ? "True" : "False");
	printf("\tchar *optarg: %s\n", imei->optarg);
	printf("}\n");

	return 0;
}
#endif /* DEBUG */

static int get_offset(struct command *command, void *arg)
{
	struct offset *offset = arg;
	size_t i;
	int rc;
	bool cmd_has_offset = !!(command->options & OPTION_OFFSET);
	bool cmd_requires_offset = !!(command->required_options &
				      OPTION_OFFSET);

	if (!cmd_has_offset && !offset->option_set)
		return 0;

	if (!cmd_has_offset && offset->option_set) {
		printf("The %s command doesn't have an -o or --offset option\n",
		       command->name);
		printf("See 'nv_data-imei %s -h' for more details\n",
		       command->name);
		return -EINVAL;
	}

	if (cmd_requires_offset  && !offset->option_set) {
		printf("OFFSET option required\n");
		printf("See nv_data-imei %s -h for more details.\n",
		       command->name);
		return -EINVAL;
	} else if (offset->option_set) {
		for (i = 0; i < strlen(offset->optarg); i++) {
			if (isspace(offset->optarg[i])) {
				continue;
			} else {
				if (offset->optarg[i] == '-')
					offset->error |= OFFSET_NEGATIVE;
				break;
			}
		}

		offset->offset = strtoul(offset->optarg, NULL, 0);
		rc = errno;

		if (offset->offset == ULONG_MAX && rc == ERANGE)
			offset->error |= OFFSET_OVERFLOW;

		if ((offset->error & OFFSET_NEGATIVE) &&
		    (offset->error & OFFSET_OVERFLOW))
			printf("Error: The '%s' offset is negative "
			       "and too big as well.\n",
			       offset->optarg);
		else if (offset->error & OFFSET_NEGATIVE)
			printf("Error: The '%s' offset is negative"
			       " but offsets cannot be negative.\n",
			       offset->optarg);
		else if (offset->error & OFFSET_OVERFLOW)
			printf("Error: The '%s' offset is too big.\n",
			       offset->optarg);

		if (offset->error)
			return -EINVAL;
	}

	return 0;
}

static int get_imei(struct command *command, void *arg)
{
	struct imei *imei = arg;

	if (command->options & OPTION_IMEI) {
		if ((command->required_options & OPTION_IMEI) &&
		    !(imei->option_set)) {
			printf("IMEI option required\n");
			printf("See nv_data-imei %s -h for more details.\n",
			       command->name);
			return -EINVAL;
		} else if (imei->option_set) {
			bool str_is_digit = true;
			bool str_len_valid;
			size_t len;
			size_t i;

			len = strlen(imei->optarg);
			str_len_valid = !!(len == IMEI_LENGTH);

			for (i = 0; i < len; i++) {
				if (!isdigit(imei->optarg[i])) {
					str_is_digit = false;
					break;
				}
			}

			if (!str_is_digit && !str_len_valid) {
				printf("The '%s' "
				       "IMEI is invalid"
				       " as it does not only contains digits\n",
				       imei->optarg);
				printf("In addition it is also invalid"
				       " as it is composed of "
				       "%d digits instead of %d.\n",
				       len, IMEI_LENGTH);
				return -EINVAL;
			} else if (!str_is_digit) {
				printf("The '%s' "
				       "IMEI is invalid"
				       " as it does not only contains digits\n",
				       imei->optarg);
				return -EINVAL;
			} else if (!str_len_valid) {
				printf("The '%s' "
				       "IMEI is invalid as it is composed of "
				       "%d digits instead of %d.\n",
				       imei->optarg, len, IMEI_LENGTH);
				return -EINVAL;
			}

			/* imei.imei is IMEI_LENGTH + 1 */
			strncpy(imei->imei, imei->optarg, IMEI_LENGTH);

			return 0;
		}
	} else if (imei->option_set) {
		printf("The %s command doesn't have an -i or --imei option\n",
		       command->name);
		printf("See 'nv_data-imei %s -h' for more details\n",
		       command->name);
		return -EINVAL;
	}

	return 0;
}

static struct command_option commands_options[] = {
	{
		OPTION_FILE,
		"",
		"",
		"",
		NULL
	},
	{
		OPTION_HELP,
		"-h|--help",
		"Display the command specific help message",
		"-h",
		NULL,
	},
	{
		OPTION_OFFSET,
		"-o OFFSET|--offset=OFFSET",
		"Use the given OFFSET",
		"--offset=0xEC80",
		get_offset,
	},
	{
		OPTION_IMEI,
		"-i IMEI|--imei=IMEI",
		"Use the given IMEI",
		"--imei=355921041234567",
		get_imei,
	},
	{ /* Sentinel */ },
};

static struct command commands[] = {
	{
		"list-supported",
		"Display supported devices and EFS",
		NO_OPTIONS,
		NO_OPTIONS,
		NULL,
	},
	{
		"read-imei",
		"Show the current IMEI from nv_data",
		OPTION_FILE|OPTION_OFFSET,
		OPTION_FILE,
		read_imei,
	},
	{
		"write-imei",
		"Store the given IMEI to nv_data (may or may not work)",
		OPTION_FILE|OPTION_IMEI|OPTION_OFFSET,
		OPTION_FILE|OPTION_IMEI|OPTION_OFFSET,
		write_imei,
	},
	{
		"bruteforce-imei",
		"Find the IMEI offset in the nv_data with the given IMEI",
		OPTION_FILE|OPTION_IMEI,
		OPTION_FILE|OPTION_IMEI,
		bruteforce_imei_offset,
	},
	{ /* Sentinel */ },
};

#if DEBUG
static int print_args(int argc, char *argv[], int optind_index)
{
	int i;

	printf("argc: %d optind: %d ", argc, optind_index);
	for (i = 0; i < argc; i++) {
		printf("[%d]%s", i, argv[i]);
		if (i != (argc - 1))
			printf(" ");
	}
	printf("\n");

	return 0;
}
#endif /* DEBUG */

static const char warning_msg[] =
	"\n"
	"+------------------------------------------------------+\n"
	"| /!\\ This tool is experimental, use at your own risk  |\n"
	"+------------------------------------------------------+\n"
	"\n"
	"It is also dangerous if used improperly: This tool can overwrite any\n"
	"part of the nv_data.bin file, without any checks or warnings: If you\n"
	"give it any location/offset inside that file, it will proceed\n"
	"blindly. This raises several concerns:\n"
	"- As we don't know how to recreate valid nv_data.bin files from\n"
	"  scratch, so you will need to make a backup of it before. We can\n"
	"  recreate a dummy one, however the result is that it ends up with a\n"
	"  generic IMEI. This means that most networks will refuse to let you\n"
	"  register and use their services, which will prevent you from doing\n"
	"  regular calls / SMS. Changing the IMEI in this generic file has\n"
	"  not been tested. It's unknown if some other parameters also need\n"
	"  to be changed to make it work.\n"
	"- As we don't know much about the content of the nv_data.bin file,\n"
	"  it could potentially be dangerous to modify data in the wrong\n"
	"  location as we don't know the effects. Effects like disrupting the\n"
	"  telephony network which could in turn prevent people from calling\n"
	"  medial emergency services cannot be excluded as this file contains\n"
	"  modem data / parameters and we don't know what they do.\n"
	"\n"
	"How to use this program.\n"
	"- First you need to obtain your current IMEI. This can be done\n"
	"  through various ways, from looking at the sticker that is written\n"
	"  under the back cover of your phone, to software means like looking\n"
	"  in Settings->About Phone->Status->IMEI information in Replicant 6.\n"
	"- Then once you have the IMEI you can either use the show-imei\n"
	"  command and verify that the text you get matches the IMEI you're\n"
	"  supposed to have, or you can also try to bruteforce the IMEI\n"
	"  location with the known IMEI.\n"
	"- Once this is done you can change the IMEI, and confirm it has been\n"
	"  changed by looking at\n"
	"  Settings->About Phone->Status->IMEI information in Replicant 6 or\n"
	"  through other software means.\n"
	"\n"
	"In the case where you don't have a valid IMEI, and you are still\n"
	"trying to change it (knowing the risks), you will need not to forget\n"
	"to verify if it has been changed by looking at\n"
	"Settings->About Phone->Status->IMEI information in Replicant 6 or\n"
	"through other software means. This will make sure that you only\n"
	"modified the offset where the IMEI is really stored and not some\n"
	"random offset with potentially crucial modem data.\n"
;

static void print_warnings(void)
{
	printf("%s", warning_msg);
}

/* TODO: Enforce type to only allow valid OPTION_* */
static void *get_option(uint8_t given_option)
{
	int i = 0;

	while (true) {
		struct command_option *command_option =
			&(commands_options[i++]);

		/* TODO: Get C to do something like if (!option) */
		if (!command_option->option)
			break;

		if (command_option->option == given_option)
			return command_option;
	}

	return NULL;
}

static int print_all_options(void)
{
	int i = 0;

	while (true) {
		struct command_option *option = &(commands_options[i++]);

		/* TODO: Get C to do something like if (!option) */
		if (!option->option)
			break;

		/* Skip options without help like OPTION_FILE */
		if (strlen(option->option_string))
			printf("\t%s # %s\n", option->option_string,
			       option->help);
	}

	return 0;
}

static int nv_data_imei_help(void)
{
	int i = 0;

	print_warnings();
	printf("\n");

	printf("Usage:\n");
	printf("\tnv_data-imei FILE COMMAND [OPTIONS]\n");
	printf("\tnv_data-imei COMMAND -h|--help "
		"# Display the command specific help message\n");

	printf("Commands:\n");

	while (true) {
		struct command *cmd = &(commands[i++]);

		/* TODO: Get C to do something like if (!cmd) */
		if (!cmd->name)
			break;

		assert(cmd->name);
		assert(cmd->help);
		printf("\t%s # %s\n", cmd->name, cmd->help);
	}

	printf("Options:\n");
	print_all_options();

	return 0;
}

static void *get_command(const char *name)
{
	int i = 0;

	while (true) {
		struct command *cmd = &(commands[i++]);

		/* TODO: Get C to do something like if (!cmd) */
		if (!cmd->name)
			break;

		if (strlen(cmd->name) != strlen(name))
			continue;

		if (!strncmp(cmd->name, name, strlen(cmd->name)))
			return cmd;
	}

	return NULL;

}

static int command_help(const char *command_name)
{

	struct command *command;
	size_t i;

	command = get_command(command_name);
	if (!command)
		return EX_USAGE;

	printf("Usage:\n");

	printf("\tnv_data-imei %s%s",
	       (command->options & OPTION_FILE) ? "FILE " : "",
	       command->name);

	if (command->options) {
		for (i = 0; i < (8 * sizeof(command->options)); i++) {
			if (command->options & BIT(i)) {
				bool required = !!(command->required_options &
						   BIT(i));
				struct command_option *option = get_option(
					command->options & BIT(i));

				/* Check if option and commands are in sync */
				assert(option != NULL);

				if (strlen(option->option_string)) {
					if (required)
						printf(" <%s>",
						       option->option_string);
					else
						printf(" [%s]",
						       option->option_string);
				}
			}
		}
	}

	printf("\n");

	if (command->options) {
		printf("Options:\n");
		for (i = 0; i < (8 * sizeof(command->options)); i++) {
			if (command->options & BIT(i)) {
				struct command_option *option = get_option(
					command->options & BIT(i));

				/* Check if option and commands are in sync */
				assert(option != NULL);

				if (strlen(option->option_string))
					printf("\t%s #%s\n",
					       option->option_string,
					       option->help);
			}
		}
	}

	printf("Example:\n");

	printf("\tnv_data-imei %s%s",
	       (command->options & OPTION_FILE) ?
	       "./efs_backup_copy/nv_data.bin" : "",
	       command->name);

	if (command->options) {
		for (i = 0; i < (8 * sizeof(command->options)); i++) {
			if (command->required_options & BIT(i)) {
				struct command_option *option = get_option(
					command->options & BIT(i));

				/* Check if option and commands are in sync */
				assert(option != NULL);

				printf(" %s", option->example);
			}
		}
	}

	printf("\n");

	return 0;
}

static int list_supported(void)
{
	/* TODO:
	 * - Print the result in a parsable format (json?)
	 * - Print the result in and a human format (a table for instance)
	 * - Add IMEI location (under the battery, unknown, etc)
	 * - Add IMEI known offsets
	 */
	printf("Supported devices:\n");

	/* Offset: 0xE80, other?
	 * Location: Under the battery
	 */
	printf("\tNexus S (GT-I902x)\n");

	return 0;
}

static void modem_log_handler(__attribute__((unused)) void *user_data,
			      const char *msg)
{
	int i, l;

	char *message;

	message = strdup(msg);
	l = strlen(message);

	if (l > 1) {
		for (i = l ; i > 0 ; i--) {
			if (message[i] == '\n')
				message[i] = 0;
			else if (message[i] != 0)
				break;
		}
		printf("%s\n", message);
	}

	free(message);
}

static int ipc_setup(struct ipc_client **client)
{
	*client = ipc_client_create(IPC_CLIENT_TYPE_DUMMY);
	if (*client == NULL) {
		printf("Creating client failed\n");
		return -EBADE;
	}

	ipc_client_log_callback_register(*client, modem_log_handler,
					 NULL);

	return 0;
}

static int decode_imei(unsigned char *buf, struct imei *imei)
{
	int i = 0;

	i += snprintf(imei->imei + i, IMEI_LENGTH + 1 - i, "%01x",
		      (*buf & 0xf0) >> 4);

	buf += sizeof(unsigned char);

	while (i < IMEI_LENGTH) {
		i += snprintf(imei->imei + i, IMEI_LENGTH + 1 - i,
			      "%02x",
			      (*buf >> 4) | ((*buf & 0x0f) << 4));
		buf += sizeof(unsigned char);
	}

	return 0;
}

static int encode_imei(unsigned char *buf, struct imei *imei)
{
	int i = 0;
	unsigned int v;
	int count = 0;

	count = sscanf(imei->imei, "%01x", &v);
	if (count != 1) {
		printf("%s: first sscanf failed with result: %d\n",
		       __func__, count);
		assert(false);
	}

	*buf++ = (v << 4) | 0xA;

	i++;
	while (i < IMEI_LENGTH) {
		count = sscanf(imei->imei + i, "%02x", &v);
		if (count != 1) {
			printf("%s: second sscanf failed with result: %d\n",
			       __func__, count);
			assert(false);
		}

		*buf++ = v << 4 | ((v & 0xf0) >> 4);
		i += 2;
	}

	return 0;
}

int bruteforce_imei_offset(char *nv_data_path, struct imei *given_imei)
{
	struct ipc_client *client = NULL;
	size_t file_size;
	size_t search_size;
	size_t nv_data_chunk_size;
	char *buffer = NULL;
	char *ptr = NULL;
	unsigned char given_imei_buffer[(IMEI_LENGTH + 1) / 2] = { 0 };
	bool found_imei = false;
	int rc;

	rc = ipc_setup(&client);
	if (rc)
		return rc;

	ipc_client_log(client,
		       "Starting bruteforce\nnv_data_path: %s",
		       nv_data_path);

	/* The sizes are device dependent, so ipc_client_nv_data_size and
	 * ipc_client_nv_data_chunk_size were used before.
	 * However we want the tool to also be able to run on any computer,
	 * instead of just being able to run on a device.
	 */
	file_size = file_data_size(client, nv_data_path);
	if (file_size == (size_t)-1) {
		rc = errno;
		goto error;
	}

	ipc_client_log(client, "nv_data size: %d\n", file_size);

	/* We only support one device so far */
	nv_data_chunk_size = XMM616_NV_DATA_CHUNK_SIZE;

	buffer = file_data_read(client, nv_data_path, file_size,
				nv_data_chunk_size, 0);

	if (buffer == NULL) {
		ipc_client_log(client, "Reading nv_data failed");
		rc = -1;
		goto error;
	}

	rc = encode_imei((unsigned char *)&given_imei_buffer, given_imei);
	if (rc < 0)
		return rc;

	ptr = buffer;
	search_size = file_size;

	do {
		ptr = memchr(ptr, given_imei_buffer[0], search_size);
		if (ptr) {
			if (!strncmp((void*)given_imei_buffer, ptr,
				     sizeof(given_imei_buffer))) {
				ipc_client_log(client,
					       "=> Found IMEI at 0x%x (%d)",
					       (ptr - buffer),
					       (ptr - buffer));
				found_imei = true;
			}

			/* Continue searching even if we already found
			 * it just in case we find the IMEI at a second
			 * location too.
			 */
			search_size = file_size - (ptr - buffer);
			ptr ++;
		}
	} while (ptr);

	if (!found_imei) {
		rc = 0;
		ipc_client_log(client, "=> IMEI not found");
	}

error:
	if (buffer)
		free(buffer);

	ipc_client_destroy(client);

	return rc;
}

int read_imei(char *nv_data_path, struct offset *offset)
{
	struct ipc_client *client = NULL;
	struct imei imei;
	size_t file_size;
	size_t nv_data_chunk_size;
	unsigned char *buffer = NULL;
	int rc;

	memset(&imei, 0, sizeof(imei));

	rc = ipc_setup(&client);
	if (rc)
		return rc;

	/* We only support one device so far */
	file_size = XMM616_NV_DATA_SIZE;
	nv_data_chunk_size = XMM616_NV_DATA_CHUNK_SIZE;

	buffer = file_data_read(client, nv_data_path, file_size,
				nv_data_chunk_size, 0);
	if (buffer == NULL) {
		ipc_client_log(client, "Reading nv_data failed\n");
		rc = -1;
		goto error;
	}

	rc = decode_imei(buffer + offset->offset, &imei);
	if (rc)
		goto error;

	ipc_client_log(client, "IMEI: %s\n", imei.imei);

	rc = 0;
	goto complete;

error:
complete:
	if (buffer)
		free(buffer);

	ipc_client_destroy(client);

	return rc;
}

int write_imei(char *nv_data_path, struct offset *offset,
		      struct imei *imei)
{
	struct ipc_client *client = NULL;
	char *md5_path = NULL;
	char *nv_data_secret;
	size_t nv_data_chunk_size;
	size_t file_size;
	char *md5_string = NULL;
	unsigned char buffer[(IMEI_LENGTH + 1) / 2] = { 0 };
	size_t length;
	int rc;

	rc = ipc_setup(&client);
	if (rc)
		return rc;

	assert(imei->imei);
	assert(strlen(imei->imei) == IMEI_LENGTH);

	asprintf(&md5_path, "%s.md5", nv_data_path);

	/* We only support one device so far */
	nv_data_secret = XMM616_NV_DATA_SECRET;
	file_size = XMM616_NV_DATA_SIZE;
	nv_data_chunk_size = XMM616_NV_DATA_CHUNK_SIZE;

	rc = encode_imei((unsigned char *)&buffer, imei);
	if (rc < 0)
		return rc;

	rc = file_data_write(client, nv_data_path, buffer, sizeof(buffer),
			     sizeof(buffer), offset->offset);
	if (rc == -1) {
		rc = errno;
		ipc_client_log(client, "Writing nv_data failed\n");
		goto complete;
	}

	md5_string = ipc_nv_data_md5_calculate(client, nv_data_path,
					       nv_data_secret, file_size,
					       nv_data_chunk_size);
	if (md5_string == NULL) {
		ipc_client_log(client, "Calculating nv_data md5 failed\n");
		goto error;
	}

	length = strlen(md5_string);

	unlink(md5_path);

	rc = file_data_write(client, md5_path, md5_string, length, length, 0);
	if (rc == -1) {
		rc = errno;
		ipc_client_log(client, "Writing nv_data md5 failed\n");
		goto complete;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (md5_path)
		free(md5_path);

	ipc_client_destroy(client);

	return rc;
}

static int errno_to_sysexit(int err)
{
	switch (err) {
	case 0:
		return EX_OK;
	case EACCES:
		return EX_NOINPUT;
	case -EINVAL:
		return EX_USAGE;
	default:
		printf("%s: error: unknown error code %d.\n", __func__, err);
		printf("%s: error code %d needs to be implemented\n", __func__,
		       err);
		assert(false);
	}

	return 0;
}

int main(int argc, char * const argv[])
{
	opterr = 0;
	struct imei imei;
	struct offset offset;
	struct command *command = NULL;
	struct command_option *option = NULL;
	char *nv_data_path;
	int c, rc;

	memset(&imei, 0, sizeof(imei));
	memset(&offset, 0, sizeof(offset));

	if (argc == 1) {
		printf("Not enough options.\n");
		printf("Try -h to print the help.\n");
		return EX_USAGE;
	}

	while (1) {
		static struct option long_options[] = {
			{"help", no_argument, 0, 'h' },
			{"imei", required_argument, 0, 'i' },
			{"offset", required_argument, 0, 'o' },
			{0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "-hi:o:", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 1:
			/* from "man 3 getopt": If the first character of
			 * optstring is '-', then each nonoption argv-element is
			 * handled as if it were the argument of an option with
			 * character code 1. (This is used by programs that were
			 * written to expect options and other argv-elements in
			 * any order and that care about the ordering of the
			 * two.)
			 */

			/* nv_data-imei list-supported */
			if (optind == 2 && argc == 2 &&
			    strlen("list-supported") ==
			    strlen(argv[optind - 1]) &&
			    !strncmp("list-supported", argv[optind - 1],
				     strlen("list-supported"))) {
				printf("nv_data-imei list-supported\n");
				return list_supported();
			/* nv_data-imei FILE COMMAND [...] */
			} else if (optind == 3 && argc >= 3) {
				nv_data_path = argv[optind - 2];
				command = get_command(argv[optind - 1]);
				if (!command) {
					printf("There is no '%s' command\n",
					       argv[optind - 1]);
					printf("Try nv_data-imei -h"
					       " to print the help.\n");
					return EX_USAGE;
				}
				/* Some commands don't take arguments nor files.
				 * The help command is already handled but some
				 * other command like list-supported need to be
				 * handled here.
				 * nv_data-imei FILE list-supported
				 */
				if (!command->options) {
					printf("The '%s' command"
					       " accepts no options\n",
					       argv[2]);
					printf("Try nv_data-imei %s --help to"
					       " print the %s command help.\n",
					       argv[optind - 1],
					       argv[optind - 1]);
					return EX_USAGE;
				}
			/* nv_data-imei <INVALID_COMMAND> */
			} else if (optind == 2 && argc == 2) {
				command = get_command(argv[optind - 1]);

				if (!command) {
					printf("There is no '%s' command\n",
					       argv[optind - 1]);
					printf("Try nv_data-imei -h"
					       " to print the help.\n");
				} else if (command->options & OPTION_FILE) {
					printf("Error: the '%s' command "
					       "needs a FILE argument.\n",
					       argv[optind - 1]);
					printf("See 'nv_data-imei %s -h'"
					       " for more details.\n",
					       argv[optind - 1]);
				} else {
					assert(false);
				}
				return EX_USAGE;

			}
			break;
		case 'h':
			/* nv_data-imei -h|--help */
			if (argc == 2) {
				return nv_data_imei_help();

			/* nv_data-imei COMMAND -h|--help */
			} else if (argc == 3) {
				rc = command_help(argv[1]);
				if (rc) {
					printf("There is no '%s' command\n",
					       argv[1]);
					printf("Try nv_data-imei -h"
					       " to print the help.\n");
					return rc;
				}

				/* nv_data-imei FILE COMMAND -h|--help|help is
				 * not supported because I didn't find an easy
				 * and robust way to differentiate between
				 * argv[1] being a file or a command. In other
				 * words If it was, supported, and that we are
				 * here, what would be the command? argv[2] or
				 * argv[3]? How to know for sure that argv[2] is
				 * really a command and not a file of the same
				 * name than the command?
				 */

				return 0;
			} else if (argc > 3) {
				printf("Wrong number of arguments."
				       " Try 'nv_data-imei COMMAND -h' instead"
				       "\n");
				printf("Example:\n");
				printf("\tnv_data-imei %s -h\n",
				       commands[0].name);
				return EX_USAGE;
			}
			break;
		case 'i':
			imei.optarg = optarg;
			if (imei.option_set) {
				printf("The %s command doesn't have an -i"
				       " or --imei option\n",
				       command->name);
				printf("See 'nv_data-imei %s -h'"
				       " for more details\n",
				       commands->name);
				return EX_USAGE;
			}

			imei.option_set = true;

			break;
		case 'o':
			offset.optarg = optarg;
			if (offset.option_set) {
				printf("The %s command"
				       " doesn't have an -o or --offset option"
				       "\n", command->name);
				printf("See 'nv_data-imei %s -h'"
				       " for more details\n",
				       commands->name);
				return EX_USAGE;
			}

			offset.option_set = true;

			break;
		case '?':
			printf("Unknown option '%s'.\n", argv[optind - 1]);
			printf("Try nv_data-imei -h to print the help.\n");
			return EX_USAGE;
		default:
			printf("case '%c':\n", c);
			printf("Unknown option '%s'.\n", argv[optind - 1]);
			printf("Try nv_data-imei -h to print the help.\n");
			return EX_USAGE;
		}
	}

	/* We use the - in optstring so all arguments go in the 'case 1:' */
	assert(optind == argc);

	if (argc == 2) {
		/* If none of the commands or options were reached, we are in
		 * the case where users ran 'nv_data-imei FILE'.
		 */
		printf("Missing options, commands or invalid command '%s'\n",
		       argv[1]);
		printf("Try -h to print the help.\n");
		return EX_USAGE;
	}

	assert(command->options & OPTION_FILE);
	assert(command->func);


	option = get_option(OPTION_IMEI);
	rc = option->get_data(command, &imei);
	if (rc)
		return errno_to_sysexit(rc);

	option = get_option(OPTION_OFFSET);

	rc = option->get_data(command, &offset);
	if (rc)
		return errno_to_sysexit(rc);

	if (command->options & OPTION_IMEI &&
	    command->options & OPTION_OFFSET) {
		rc = command->func(nv_data_path, &offset, &imei);
		return errno_to_sysexit(rc);
	}

	if (command->options & OPTION_IMEI) {
		rc = command->func(nv_data_path, &imei);
		return errno_to_sysexit(rc);
	}

	if (command->options & OPTION_OFFSET) {
		rc = command->func(nv_data_path, &offset);
		return errno_to_sysexit(rc);
	}

	assert(false);

	return 0;
}
