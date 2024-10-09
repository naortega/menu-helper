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

int command_add(void) {
	std::string name, description, ingredients, tags;
	int recipe_id, ingredient_id, tag_id;

	if(not db_open()) {
		std::cerr << "Failed to open database. Cannot add new entry." << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Name: ";
	getline(std::cin, name);

	std::cout << "Description: ";
	getline(std::cin, description);

	std::cout << "Ingredients (comma separated): ";
	getline(std::cin, ingredients);

	std::cout << "Tags (comma separated): ";
	getline(std::cin, tags);

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
