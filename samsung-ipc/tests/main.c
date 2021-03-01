/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2016 Paul Kocialkowsk <contact@paulk.fr>
 * Copyright (C) 2021 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include <samsung-ipc.h>

/* libsamsung-ipc internal headers */
#include <ipc.h>
#include "partitions/android.h"

struct test {
	char *name;
	int (*func)(struct ipc_client *client);
};

static struct test tests[] = {
	{
		"open_android_modem_partition",
		test_open_android_modem_partition
	},
};

static void usage(const char *progname)
{
	printf("Usage: %s list-tests\n", progname);
	printf("Usage: %s test <test>\n", progname);
}

static void list_tests(void)
{
	unsigned int i;

	printf("Available tests:\n");

	for (i = 0; i < sizeof(tests) / sizeof(struct test); i++)
		printf("  %s\n", tests[i].name);
}

static struct test *get_test(char *name)
{
	unsigned int i;

	for (i = 0; i < sizeof(tests) / sizeof(struct test); i++) {
		if (!strcmp(tests[i].name, name))
			return &(tests[i]);
	}

	return NULL;
}

static void log_callback(__attribute__((unused)) void *data,
			 __attribute__((unused)) const char *message)
{
	/* TODO: add better logging mechanism in libsamsung-ipc with
	 * tags and log levels which would enable to filter the
	 * messages from different provenance (this file, the code
	 * under test, other parts of libsamsung-ipc, etc).
	 */
#ifdef DEBUG
	char *buffer;
	size_t length;
	int i;

	if (message == NULL)
		return;

	buffer = strdup(message);
	length = strlen(message);

	for (i = length; i > 0; i--) {
		if (buffer[i] == '\n')
			buffer[i] = '\0';
		else if (buffer[i] != '\0')
			break;
	}

	printf("[ipc] %s\n", buffer);

	free(buffer);
#endif
}

int main(int argc, char *argv[])
{
	struct ipc_client *client = NULL;
	const char *progname = "libsamsung-ipc-test";
	char *given_test_name;
	struct test *test;
	int rc = 0;

	if (argc == 2 && !strcmp("list-tests", argv[1])) {
		list_tests();
		return 0;
	} else if (argc == 3 && !strcmp("test", argv[1])) {
		given_test_name = argv[2];
	} else {
		usage(progname);
		return EX_USAGE;
	}

	test = get_test(given_test_name);
	if (test == NULL) {
		printf("Unknown test %s\n", given_test_name);
		return EX_USAGE;
	}

	client = ipc_client_create(IPC_CLIENT_TYPE_DUMMY);
	if (client == NULL) {
		printf("[ !! ] Creating client failed\n");
		goto error;
	}

	rc = ipc_client_log_callback_register(client, log_callback, NULL);
	if (rc < 0) {
		printf("[ !! ] Registering log callback failed: error %d\n",
		       rc);
		goto error;
	}

	rc = test->func(client);
	if (rc == 0) {
		printf("[ OK ] %s succedded\n", test->name);
	} else {
		printf("[ !! ] %s failed\n", test->name);
		goto error;
	}

	return 0;

error:
	if (client != NULL)
		ipc_client_destroy(client);

	return EX_SOFTWARE;
}
