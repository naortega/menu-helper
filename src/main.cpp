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

#include <iostream>
#include <cstdlib>

#include "arg_parse.hpp"
#include "cmd.hpp"

int main(int argc, char *argv[]) {
	enum cmd_id id;
	int ret = EXIT_SUCCESS;

	if(argc < 2) {
		std::cerr << "Invalid number of arguments. Use 'help' sub-command." << std::endl;
		print_usage();
		return EXIT_FAILURE;
	}

	id = parse_args(argv[1]);

	switch(id) {
	case CMD_ADD:
		ret = command_add();
		break;
	case CMD_LIST:
		ret = command_list(argc - 1, argv + 1);
		break;
	case CMD_DEL:
		ret = command_delete(argc - 2, argv + 2);
		break;
	case CMD_HELP:
		print_help();
		break;
	case CMD_VERSION:
		print_version();
		break;
	default:
		std::cerr << "No such command '" << argv[1] << "'. Use 'help' sub-command." << std::endl;
		print_usage();
		return EXIT_FAILURE;
	}

	return ret;
}
