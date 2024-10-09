/*
 * Copyright (C) 2024  Nicolás Ortega Froysa <nicolas@ortegas.org>
 * Nicolás Ortega Froysa <nicolas@ortegas.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <stdio.h>

enum cmd_id {
	CMD_UNKNOWN = 0,
	CMD_ADD,
	CMD_HELP,
	CMD_VERSION,
};

struct cmd {
	enum cmd_id id;
	const char *str[3];
};

static const struct cmd commands[] = {
	{ CMD_ADD, {"add", "new"} },
	{ CMD_HELP, {"help", "-h", "--help"} },
	{ CMD_VERSION, {"version", "-v", "--version"} },
};

static inline void print_version(void) {
	printf("menu-helper v%s\n\n", VERSION);
}

static inline void print_usage(void) {
	printf("USAGE: menu-helper <cmd> [options]\n\n");
}

static inline void print_help(void) {
	print_version();
	print_usage();

	printf("COMMANDS:\n"
		   "\tadd, new                 Add a new recipe to the database\n"
		   "\thelp, -h, --help         Show this help information.\n"
		   "\tversion, -v, --version   Show version information.\n"
		   "\n");
}


enum cmd_id parse_args(const char *cmd);
