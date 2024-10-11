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
#include "cmd.hpp"
#include "db.hpp"
#include "util.hpp"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>

int command_add(void) {
	std::string name, description, ingredients, tags;
	int recipe_id, ingredient_id, tag_id;

	std::cout << "Name: ";
	getline(std::cin, name);

	std::cout << "Description: ";
	getline(std::cin, description);

	std::cout << "Ingredients (comma separated): ";
	getline(std::cin, ingredients);

	std::cout << "Tags (comma separated): ";
	getline(std::cin, tags);

	if(not db_open()) {
		std::cerr << "Failed to open database. Cannot add new entry." << std::endl;
		return EXIT_FAILURE;
	}

	if((recipe_id = db_get_recipe_id(name)) <= 0)
		recipe_id = db_add_recipe(name, description);

	for(auto &ingredient : split(ingredients, ",")) {
		trim(ingredient);

		if((ingredient_id = db_get_ingredient_id(ingredient)) <= 0)
			ingredient_id = db_add_ingredient(ingredient);
		db_conn_recipe_ingredient(recipe_id, ingredient_id);
	}

	for(auto &tag : split(tags, ",")) {
		trim(tag);

		if((tag_id = db_get_tag_id(tag)) <= 0)
			tag_id = db_add_tag(tag);
		db_conn_recipe_tag(recipe_id, tag_id);
	}

	db_close();

	return EXIT_SUCCESS;
}

int command_list(int argc, char *argv[]) {
	std::vector<std::string> ingredients, tags;
	int opt;

	while((opt = getopt(argc, argv, "i:t:")) != -1) {
		switch(opt) {
		case 'i':
			ingredients = split(optarg, ",");
			for(auto &i : ingredients)
				trim(i);
			break;
		case 't':
			tags = split(optarg, ",");
			for(auto &i : tags)
				trim(i);
			break;
		case '?':
			std::cerr << "Unknown option '" << static_cast<char>(optopt)
				<< "'. Use 'help' for information." << std::endl;
			return EXIT_FAILURE;
		}
	}

	if(not db_open()) {
		std::cerr << "Failed to open database. Cannot add new entry." << std::endl;
		return EXIT_FAILURE;
	}

	for(const auto &recipe : db_get_recipes(ingredients, tags))
		std::cout << recipe.id << "  |  " << recipe.name << "  |  " << recipe.description << std::endl;

	db_close();

	return EXIT_SUCCESS;
}

int command_delete(int argc, char *argv[]) {
	int ret = EXIT_SUCCESS;
	std::vector<int> recipe_ids;

	if(argc < 1) {
		std::cerr << "No specified IDs. Use 'help' for more information." << std::endl;
		return EXIT_FAILURE;
	}

	if(not db_open()) {
		std::cerr << "Failed to open database. Cannot add new entry." << std::endl;
		return EXIT_FAILURE;
	}

	for(int i = 0; i < argc; ++i) {
		const int id = std::stoi(argv[i]);

		if(not db_recipe_exists(id)) {
			std::cerr << "No recipe exists with ID " << id << "." << std::endl;
			return EXIT_FAILURE;
		} else {
			recipe_ids.push_back(id);
		}
	}

	ret = (db_del_recipes(recipe_ids)) ? EXIT_SUCCESS : EXIT_FAILURE;

	db_close();

	return ret;
}
