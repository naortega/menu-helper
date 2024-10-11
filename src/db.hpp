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

#include <string>
#include <vector>

struct recipe {
	int id;
	std::string name;
	std::string description;
};

int db_open(void);
void db_close(void);

/**
 * @brief Add a new recipe to the database.
 *
 * @param name Name of the new recipe.
 * @param description Short description.
 *
 * @return ID of newly created recipe, -1 if DB isn't open, -2 on other failure.
 */
int db_add_recipe(const std::string &name, const std::string &description);
int db_get_recipe_id(const std::string &name);
static inline int db_recipe_exists(const std::string &name) {
	return (db_get_recipe_id(name) > 0);
}
std::vector<struct recipe> db_get_recipes(const std::vector<std::string> &ingredients,
										  const std::vector<std::string> &tags);

/**
 * @brief Add a new ingredient to the database.
 *
 * @param name Name of the new ingredient.
 *
 * @return ID of newly created ingredient, -1 if DB isn't open, -2 on other failure.
 */
int db_add_ingredient(const std::string &name);
int db_get_ingredient_id(const std::string &name);
static inline int db_ingredient_exists(const std::string &name) {
	return (db_get_ingredient_id(name) > 0);
}

/**
 * @brief Add a new tag to the database.
 *
 * @param name Name of the new tag.
 *
 * @return ID of newly created tag, -1 if DB isn't open, -2 on other failure.
 */
int db_add_tag(const std::string &name);
int db_get_tag_id(const std::string &name);
static inline int db_tag_exists(const std::string &name) {
	return (db_get_tag_id(name) > 0);
}

int db_conn_recipe_ingredient(int recipe_id, int ingredient_id);
int db_conn_recipe_tag(int recipe_id, int tag_id);
