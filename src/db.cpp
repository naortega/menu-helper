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
#include "db.hpp"

#include <sqlite3.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <format>
#include <string>
#include <vector>

#define DB_VERSION 1
static sqlite3 *db = nullptr;

int db_open(void) {
	std::string xdg_data_home;
	std::string db_path;
	bool new_db = false;
	int rc;

	if((xdg_data_home = std::getenv("XDG_DATA_HOME")).empty()) {
		std::cerr << "Cannot find environment variable XDG_DATA_HOME. Please define it before continuing. E.g.:\n"
			"export XDG_DATA_HOME=\"$HOME/.local/share\"" << std::endl;
		return 0;
	}

	db_path = xdg_data_home + "/menu-helper";

	if(not std::filesystem::exists(db_path))
		std::filesystem::create_directories(db_path);

	db_path += "/recipes.db";

	if(not std::filesystem::exists(db_path)) {
		std::cout << "Creating database: " << db_path << std::endl;
		new_db = true;
	}

	rc = sqlite3_open(db_path.c_str(), &db);

	if(rc == SQLITE_OK and new_db) {
		sqlite3_exec(db, "CREATE TABLE db_version(version INTEGER UNIQUE NOT nullptr);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, std::format("INSERT INTO db_version VALUES({});", DB_VERSION).c_str(), nullptr, nullptr, nullptr);
		sqlite3_exec(db, "CREATE TABLE tags(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, "CREATE TABLE ingredients(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, "CREATE TABLE recipes(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE, description STRING);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, "CREATE TABLE recipe_tag(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, tag_id INTEGER REFERENCES tags(id) ON DELETE CASCADE);", nullptr, nullptr, nullptr);
		sqlite3_exec(db, "CREATE TABLE recipe_ingredient(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, ingredient_id INTEGER REFERENCES ingredients(id) ON DELETE CASCADE);", nullptr, nullptr, nullptr);
	}

	return rc == SQLITE_OK;
}

void db_close(void) {
	if(not db)
		return;

	sqlite3_close(db);
}

int query_id_cb(void *recipe_id_var, int col_num, char **col_data, char **col_name) {
	int *recipe_id_ptr = (int*)recipe_id_var;
	int ret = 1;

	for(int i = 0; i < col_num; ++i) {
		if(std::string(col_name[i]) == "id") {
			*recipe_id_ptr = std::atoi(col_data[i]);
			ret = 0;
			break;
		}
	}

	return ret;
}

int table_get_id_by_name(const std::string &table, const std::string &name) {
	int id = 0;

	if(sqlite3_exec(db, std::format("SELECT id FROM {} WHERE lower(name)=lower('{}');", table, name).c_str(),
					query_id_cb, &id, nullptr) not_eq SQLITE_OK)
		return -2;

	return id;
}

int db_add_recipe(const std::string &name, const std::string &description) {
	if(not db)
		return -1;

	if(sqlite3_exec(db, std::format("INSERT INTO recipes(name,description) VALUES('{}','{}');", name, description).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return -2;

	return db_get_recipe_id(name);
}

bool db_del_recipe(const int id) {
	if(not db)
		return false;

	if(sqlite3_exec(db, std::format("DELETE FROM recipes WHERE id={}", id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return false;

	return true;
}

bool db_del_recipes(const std::vector<int> &ids) {
	std::string stmt = "DELETE FROM recipes WHERE id IN (";

	if(not db)
		return false;

	bool first = true;
	for(auto id : ids) {
		if(first)
			first = false;
		else
			stmt += ",";

		stmt += std::to_string(id);
	}

	stmt += ");";

	if(sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return false;

	return true;
}

bool db_recipe_exists(const int id) {
	bool exists = false;

	if(not db)
		return false;

	sqlite3_exec(db, std::format("SELECT id FROM recipes WHERE id={}", id).c_str(),
				 [](void *found,int,char**,char**) {
				 *static_cast<bool*>(found) = true;
				 return 0;
				 }, &exists, nullptr);

	return exists;
}

int db_get_recipe_id(const std::string &name) {
	if(not db)
		return -1;

	return table_get_id_by_name("recipes", name);
}

struct recipe db_get_recipe(const int id) {
	struct recipe recipe;

	sqlite3_exec(db, std::format("SELECT * FROM recipes WHERE id={};", id).c_str(),
				 [](void *recipe, int, char **col_data, char**) {
				 *static_cast<struct recipe*>(recipe) = { std::atoi(col_data[0]), col_data[1], col_data[2] };
				 return 0;
				 }, &recipe, nullptr);

	return recipe;
}

std::vector<struct recipe> db_get_recipes(const std::vector<std::string> &ingredients,
										  const std::vector<std::string> &tags)
{
	std::vector<struct recipe> recipes;
	std::string stmt = "SELECT id,name,description FROM recipes";
	std::string filters;

	if(not ingredients.empty() or not tags.empty())
		filters += " WHERE";

	if(not ingredients.empty()) {
		bool first = true;
		for(auto &i : ingredients) {
			int id;

			if(first)
				first = false;
			else
				filters += " AND";

			filters += " id IN (SELECT recipe_id FROM recipe_ingredient WHERE ingredient_id=";

			// TODO: use throw?
			if((id = db_get_ingredient_id(i)) < 0) {
				std::cerr << "Failed to find ingredient '" << i << "'" << std::endl;
				return std::vector<struct recipe>();
			} else {
				filters += std::to_string(id);
			}

			filters += ")";
		}
	}

	if(not tags.empty()) {
		if(not filters.empty())
			filters += " AND";

		bool first = true;
		for(auto &i : tags) {
			int id;

			if(first)
				first = false;
			else
				filters += " AND";

			filters += " id IN (SELECT recipe_id FROM recipe_tag WHERE tag_id=";

			// TODO: use throw?
			if((id = db_get_tag_id(i)) < 0) {
				std::cerr << "Failed to find tag '" << i << "'" << std::endl;
				return std::vector<struct recipe>();
			} else {
				filters += std::to_string(id);
			}

			filters += ")";
		}
	}

	stmt += filters + ";";

	sqlite3_exec(db, stmt.c_str(),
				 [](void *recipe_list, int, char **col_data, char**) {
				 	auto recipe_vec = static_cast<std::vector<struct recipe>*>(recipe_list);
					recipe_vec->push_back({
										  std::atoi(col_data[0]),
										  col_data[1],
										  col_data[2] });
					return 0;
				 }, &recipes, nullptr);

	return recipes;
}

int db_add_ingredient(const std::string &name) {
	if(not db)
		return -1;

	if(sqlite3_exec(db, std::format("INSERT INTO ingredients(name) VALUES(lower('{}'));", name).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return -2;

	return db_get_ingredient_id(name);
}

std::vector<std::string> db_get_recipe_ingredients(const int id) {
	std::vector<std::string> ingredients;

	sqlite3_exec(db, std::format("SELECT name FROM ingredients WHERE id IN (SELECT ingredient_id FROM recipe_ingredient WHERE recipe_id={});", id).c_str(),
				 [](void *ingredients, int, char **col_data, char**) {
				 static_cast<std::vector<std::string>*>(ingredients)->push_back(col_data[0]);
				 return 0;
				 }, &ingredients, nullptr);

	return ingredients;
}

int db_get_ingredient_id(const std::string &name) {
	if(not db)
		return -1;

	return table_get_id_by_name("ingredients", name);
}

int db_add_tag(const std::string &name) {
	if(not db)
		return -1;

	if(sqlite3_exec(db, std::format("INSERT INTO tags(name) VALUES('{}');", name).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return -2;

	return db_get_tag_id(name);
}

std::vector<std::string> db_get_recipe_tags(const int id) {
	std::vector<std::string> tags;

	sqlite3_exec(db, std::format("SELECT name FROM tags WHERE id IN (SELECT tag_id FROM recipe_tag WHERE recipe_id={});", id).c_str(),
				 [](void *tags, int, char **col_data, char**) {
				 static_cast<std::vector<std::string>*>(tags)->push_back(col_data[0]);
				 return 0;
				 }, &tags, nullptr);

	return tags;
}

int db_get_tag_id(const std::string &name) {
	if(not db)
		return -1;

	return table_get_id_by_name("tags", name);
}

int db_conn_recipe_ingredient(int recipe_id, int ingredient_id) {
	if(not db)
		return -1;

	if(sqlite3_exec(db, std::format("INSERT INTO recipe_ingredient(recipe_id, ingredient_id) VALUES({},{});", recipe_id, ingredient_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return -2;

	return 1;
}

int db_conn_recipe_tag(int recipe_id, int tag_id) {
	if(not db)
		return -1;

	if(sqlite3_exec(db, std::format("INSERT INTO recipe_tag(recipe_id, tag_id) VALUES({},{});", recipe_id, tag_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		return -2;

	return 1;
}
