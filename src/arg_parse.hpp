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

#include <iostream>
#include <string>
#include <map>
#include <vector>

enum cmd_id {
	CMD_UNKNOWN = 0,
	CMD_ADD,
	CMD_DEL,
	CMD_LIST,
	CMD_INFO,
	CMD_EDIT_NAME,
	CMD_EDIT_DESC,
	CMD_ADD_INGR,
	CMD_RM_INGR,
	CMD_ADD_TAG,
	CMD_RM_TAG,
	CMD_HELP,
	CMD_VERSION,
};

static const std::map<enum cmd_id, std::vector<std::string>> commands = {
	{ CMD_ADD, {"add", "new"} },
	{ CMD_DEL, {"del", "rm"} },
	{ CMD_LIST, {"list", "ls"} },
	{ CMD_INFO, {"info", "i"} },
	{ CMD_EDIT_NAME, {"edit-name"} },
	{ CMD_EDIT_DESC, {"edit-description", "edit-desc"} },
	{ CMD_ADD_INGR, {"add-ingr"} },
	{ CMD_RM_INGR, {"rm-ingr"} },
	{ CMD_ADD_TAG, {"add-tag"} },
	{ CMD_RM_TAG, {"rm-tag"} },
	{ CMD_HELP, {"help", "-h", "--help"} },
	{ CMD_VERSION, {"version", "-v", "--version"} },
};

static inline void print_version(void) {
	std::cout << "menu-helper v" << VERSION << "\n" << std::endl;
}

static inline void print_usage(void) {
	std::cout << "USAGE: menu-helper <cmd> [options]\n" << std::endl;
}

static inline void print_help(void) {
	print_version();
	print_usage();

	std::cout << "COMMANDS:\n"
		   "\tadd, new                     Add a new recipe to the database.\n"
		   "\tdel, rm                      Delete recipe by ID.\n"
		   "\tlist, ls                     List recipes with filters.\n"
		   "\tinfo                         Show recipe information.\n"
		   "\tedit-name                    Change recipe name.\n"
		   "\tedit-description, edit-desc  Change recipe description.\n"
		   "\tadd-ingr                     Add ingredient to a recipe.\n"
		   "\trm-ingr                      Remove ingredient from a recipe.\n"
		   "\tadd-tag                      Add tag to a recipe.\n"
		   "\trm-tag                       Remove tag from a recipe.\n"
		   "\thelp, -h, --help             Show this help information.\n"
		   "\tversion, -v, --version       Show version information.\n"
		   << std::endl;
	std::cout << "For more information about subcommands, use 'man menu-helper'." << std::endl;
}

enum cmd_id parse_args(const std::string &cmd);
