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

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

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

	try {
		switch(id) {
		case CMD_ADD:
			if(argc not_eq 2)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_add();
			break;
		case CMD_DEL:
			if(argc not_eq 3)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_delete(argc - 2, argv + 2);
			break;
		case CMD_LIST:
			if(argc > 6)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_list(argc - 1, argv + 1);
			break;
		case CMD_INFO:
			if(argc not_eq 3)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_info(std::stoi(argv[2]));
			break;
		case CMD_EDIT_NAME:
			if(argc not_eq 3)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_edit_name(std::stoi(argv[2]));
			break;
		case CMD_EDIT_DESC:
			if(argc not_eq 3)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_edit_desc(std::stoi(argv[2]));
			break;
		case CMD_ADD_INGR:
			if(argc not_eq 4)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_add_ingr(std::stoi(argv[2]), argv[3]);
			break;
		case CMD_RM_INGR:
			if(argc not_eq 4)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_rm_ingr(std::stoi(argv[2]), argv[3]);
			break;
		case CMD_ADD_TAG:
			if(argc not_eq 4)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_add_tag(std::stoi(argv[2]), argv[3]);
			break;
		case CMD_RM_TAG:
			if(argc not_eq 4)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			ret = cmd_rm_tag(std::stoi(argv[2]), argv[3]);
			break;
		case CMD_HELP:
			if(argc not_eq 2)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			print_help();
			break;
		case CMD_VERSION:
			if(argc not_eq 2)
				throw "Invalid number of arguments. Use 'help' subcommand for more information.";
			print_version();
			break;
		default:
			std::cerr << "No such command '" << argv[1] << "'. Use 'help' sub-command." << std::endl;
			print_usage();
			ret = EXIT_FAILURE;
			break;
		}
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
		ret = EXIT_FAILURE;
	}

	return ret;
}
