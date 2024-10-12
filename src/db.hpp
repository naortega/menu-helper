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

#include <sqlite3.h>
#include <string>
#include <vector>

struct recipe {
	int id;
	std::string name;
	std::string description;
};

class db {
private:
	sqlite3 *sqlite_db;
	int table_get_id_by_name(const std::string &table, const std::string &name);

public:
	db() : sqlite_db(nullptr) {}
	~db() {
		sqlite3_close(sqlite_db);
	}
	void open(void);
	void close(void);

	/**
	 * @brief Add a new recipe to the database.
	 *
	 * @param name Name of the new recipe.
	 * @param description Short description.
	 *
	 * @return ID of newly created recipe.
	 */
	int add_recipe(const std::string &name, const std::string &description);
	void del_recipe(const int id);
	void del_recipes(const std::vector<int> &ids);
	inline int get_recipe_id(const std::string &name) {
		return table_get_id_by_name("recipes", name);
	}
	inline bool recipe_exists(const std::string &name) {
		return (get_recipe_id(name) > 0);
	}
	bool recipe_exists(const int id);
	struct recipe get_recipe(const int id);
	std::vector<struct recipe> get_recipes(const std::vector<std::string> &ingredients,
										   const std::vector<std::string> &tags);

	/**
	 * @brief Add a new ingredient to the database.
	 *
	 * @param name Name of the new ingredient.
	 *
	 * @return ID of newly created ingredient.
	 */
	int add_ingredient(const std::string &name);
	std::vector<std::string> get_recipe_ingredients(const int id);
	inline int get_ingredient_id(const std::string &name) {
		return table_get_id_by_name("ingredients", name);
	}
	inline bool ingredient_exists(const std::string &name) {
		return (get_ingredient_id(name) > 0);
	}

	/**
	 * @brief Add a new tag to the database.
	 *
	 * @param name Name of the new tag.
	 *
	 * @return ID of newly created tag, -1 if DB isn't open, -2 on other failure.
	 */
	int add_tag(const std::string &name);
	std::vector<std::string> get_recipe_tags(const int id);
	inline int get_tag_id(const std::string &name) {
		return table_get_id_by_name("tags", name);
	}
	inline bool tag_exists(const std::string &name) {
		return (get_tag_id(name) > 0);
	}

	void conn_recipe_ingredient(int recipe_id, int ingredient_id);
	void conn_recipe_tag(int recipe_id, int tag_id);
};
