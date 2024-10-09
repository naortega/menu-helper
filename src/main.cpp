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
#include <stdlib.h>
#include <stdio.h>

#include "arg_parse.hpp"
#include "cmd.hpp"

int main(int argc, char *argv[]) {
	enum cmd_id id;

	if(argc < 2) {
		fprintf(stderr, "Invalid number of arguments. Use 'help' sub-command.\n");
		print_usage();
		return EXIT_FAILURE;
	}

	id = parse_args(argv[1]);

	switch(id) {
	case CMD_ADD:
		command_add();
		break;
	case CMD_HELP:
		print_help();
		break;
	case CMD_VERSION:
		print_version();
		break;
	default:
		fprintf(stderr, "No such command '%s'. Use 'help' sub-command.\n", argv[1]);
		print_usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
