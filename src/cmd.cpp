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

#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

int cmd_add(void) {
	db db;
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

	db.open();

	if((recipe_id = db.get_recipe_id(name)) <= 0)
		recipe_id = db.add_recipe(name, description);

	for(auto &ingredient : split(ingredients, ",")) {
		trim(ingredient);

		if((ingredient_id = db.get_ingredient_id(ingredient)) <= 0)
			ingredient_id = db.add_ingredient(ingredient);
		db.conn_recipe_ingredient(recipe_id, ingredient_id);
	}

	for(auto &tag : split(tags, ",")) {
		trim(tag);

		if((tag_id = db.get_tag_id(tag)) <= 0)
			tag_id = db.add_tag(tag);
		db.conn_recipe_tag(recipe_id, tag_id);
	}

	db.close();

	return EXIT_SUCCESS;
}

int cmd_list(int argc, char *argv[]) {
	db db;
	std::vector<std::string> ingredients, tags;
	int opt;

	while((opt = getopt(argc, argv, "i:t:")) not_eq -1) {
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

	db.open();

	for(const auto &recipe : db.get_recipes(ingredients, tags))
		std::cout << recipe.id << "  |  " << recipe.name << "  |  " << recipe.description << std::endl;

	db.close();

	return EXIT_SUCCESS;
}

int cmd_delete(int argc, char *argv[]) {
	db db;
	std::vector<int> recipe_ids;

	if(argc < 1) {
		std::cerr << "No specified IDs. Use 'help' for more information." << std::endl;
		return EXIT_FAILURE;
	}

	db.open();

	for(int i = 0; i < argc; ++i) {
		const int id = std::stoi(argv[i]);

		if(not db.recipe_exists(id)) {
			std::cerr << "No recipe exists with ID " << id << "." << std::endl;
			db.close();
			return EXIT_FAILURE;
		} else {
			recipe_ids.push_back(id);
		}
	}

	db.del_recipes(recipe_ids);
	db.close();

	return EXIT_SUCCESS;
}

int cmd_info(const int id) {
	db db;
	struct recipe recipe;
	std::vector<std::string> ingredients, tags;

	db.open();

	if(not db.recipe_exists(id)) {
		std::cerr << "No recipe with ID '" << id << "'";
		db.close();
		return EXIT_FAILURE;
	}

	recipe = db.get_recipe(id);
	ingredients = db.get_recipe_ingredients(id);
	tags = db.get_recipe_tags(id);

	db.close();

	std::cout << "Name: " << recipe.name << "\n"
		<< "Description: " << recipe.description << "\n"
		<< "ID: " << recipe.id << "\n"
		<< std::endl;

	std::cout << "Ingredients:" << std::endl;
	for(auto &ingredient : ingredients)
		std::cout << "\t- " << ingredient << std::endl;
	std::cout << std::endl;

	std::cout << "Tags:" << std::endl;
	for(auto &tag : tags)
		std::cout << "\t- " << tag << std::endl;
	std::cout << std::endl;

	return EXIT_SUCCESS;
}

int cmd_add_ingr(const int recipe_id, const char *ingredients) {
	db db;
	std::vector<std::string> ingr_list = split(ingredients, ",");

	db.open();
	if(not db.recipe_exists(recipe_id)) {
		std::cerr << "Recipe with ID " << recipe_id << " does not exist." << std::endl;
		db.close();
		return EXIT_FAILURE;
	}

	for(auto &i : ingr_list) {
		int ingr_id;
		trim(i);

		if(not db.ingredient_exists(i))
			ingr_id = db.add_ingredient(i);
		else
			ingr_id = db.get_ingredient_id(i);

		db.conn_recipe_ingredient(recipe_id, ingr_id);
	}

	db.close();

	return EXIT_SUCCESS;
}

int cmd_rm_ingr(const int recipe_id, const char *ingredients) {
	db db;
	std::vector<std::string> ingr_list = split(ingredients, ",");

	db.open();
	if(not db.recipe_exists(recipe_id)) {
		std::cerr << "Recipe with ID " << recipe_id << " does not exist." << std::endl;
		db.close();
		return EXIT_FAILURE;
	}

	for(auto &i : ingr_list) {
		int ingr_id;
		trim(i);

		if(not db.ingredient_exists(i)) {
			std::cerr << "Could not find ingredient '" << i << "'. Skipping!" << std::endl;
			continue;
		}

		ingr_id = db.get_ingredient_id(i);
		db.disconn_recipe_ingredient(recipe_id, ingr_id);
	}

	db.close();

	return EXIT_SUCCESS;
}

int cmd_add_tag(const int recipe_id, const char *tags) {
	db db;
	std::vector<std::string> tag_list = split(tags, ",");

	db.open();
	if(not db.recipe_exists(recipe_id)) {
		std::cerr << "Recipe with ID " << recipe_id << " does not exist." << std::endl;
		db.close();
		return EXIT_FAILURE;
	}

	for(auto &i : tag_list) {
		int tag_id;
		trim(i);

		if(not db.tag_exists(i))
			tag_id = db.add_tag(i);
		else
			tag_id = db.get_ingredient_id(i);

		db.conn_recipe_tag(recipe_id, tag_id);
	}

	db.close();

	return EXIT_SUCCESS;
}

int cmd_rm_tag(const int recipe_id, const char *tags) {
	db db;
	std::vector<std::string> tag_list = split(tags, ",");

	db.open();
	if(not db.recipe_exists(recipe_id)) {
		std::cerr << "Recipe with ID " << recipe_id << " does not exist." << std::endl;
		db.close();
		return EXIT_FAILURE;
	}

	for(auto &i : tag_list) {
		int tag_id;
		trim(i);

		if(not db.tag_exists(i)) {
			std::cerr << "Could not find tag '" << i << "'. Skipping!" << std::endl;
			continue;
		}

		tag_id = db.get_tag_id(i);
		db.disconn_recipe_tag(recipe_id, tag_id);
	}

	db.close();

	return EXIT_SUCCESS;
}
