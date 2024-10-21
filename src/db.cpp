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

#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <sqlite3.h>
#include <stdexcept>

#define DB_VERSION 1

void db::open(void) {
	std::string xdg_data_home;
	std::string db_path;
	bool new_db = false;

	if((xdg_data_home = std::getenv("XDG_DATA_HOME")).empty())
		throw std::runtime_error("Cannot find environment variable XDG_DATA_HOME. Please define it before continuing.");

	db_path = xdg_data_home + "/menu-helper";

	if(not std::filesystem::exists(db_path))
		std::filesystem::create_directories(db_path);

	db_path += "/recipes.db";

	if(not std::filesystem::exists(db_path)) {
		std::cout << "Creating database: " << db_path << std::endl;
		new_db = true;
	}

	if(sqlite3_open(db_path.c_str(), &sqlite_db) not_eq SQLITE_OK)
		throw std::runtime_error("Failed to open database file " + db_path);

	if(new_db) {
		sqlite3_exec(sqlite_db, "CREATE TABLE db_version(version INTEGER UNIQUE NOT NULL);", nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, std::format("INSERT INTO db_version VALUES({});", DB_VERSION).c_str(), nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, "CREATE TABLE tags(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, "CREATE TABLE ingredients(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, "CREATE TABLE recipes(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE, description STRING);", nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, "CREATE TABLE recipe_tag(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, tag_id INTEGER REFERENCES tags(id) ON DELETE CASCADE, UNIQUE(recipe_id, tag_id));", nullptr, nullptr, nullptr);
		sqlite3_exec(sqlite_db, "CREATE TABLE recipe_ingredient(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, ingredient_id INTEGER REFERENCES ingredients(id) ON DELETE CASCADE, UNIQUE(recipe_id, ingredient_id));", nullptr, nullptr, nullptr);
	}
}

void db::close(void) {
	if(not sqlite_db)
		return;

	sqlite3_close(sqlite_db);
	sqlite_db = nullptr;
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

int db::table_get_id_by_name(const std::string &table, const std::string &name) {
	int id = 0;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("SELECT id FROM {} WHERE lower(name)=lower('{}');", table, name).c_str(),
					query_id_cb, &id, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to get ID of '{}' from table '{}'.", name, table));
	}

	return id;
}

int db::add_recipe(const std::string &name, const std::string &description) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("INSERT OR IGNORE INTO recipes(name,description) VALUES('{}','{}');", name, description).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error("Failed to insert new recipe into database.");
	}

	return get_recipe_id(name);
}

void db::del_recipe(const int id) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("DELETE FROM recipes WHERE id={}", id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to delete recipe with ID {} from database.", id));
	}
}

void db::del_recipes(const std::vector<int> &ids) {
	std::string stmt = "DELETE FROM recipes WHERE id IN (";

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	bool first = true;
	for(auto id : ids) {
		if(first)
			first = false;
		else
			stmt += ",";

		stmt += std::to_string(id);
	}

	stmt += ");";

	if(sqlite3_exec(sqlite_db, stmt.c_str(), nullptr, nullptr, nullptr) not_eq SQLITE_OK)
		throw std::runtime_error("Failed to delete recipes from database.");
}

bool db::recipe_exists(const int id) {
	bool exists = false;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("SELECT id FROM recipes WHERE id={}", id).c_str(),
					[](void *found,int,char**,char**) {
					*static_cast<bool*>(found) = true;
					return 0;
					}, &exists, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error("Failed to select from database.");
	}

	return exists;
}

struct recipe db::get_recipe(const int id) {
	struct recipe recipe;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("SELECT * FROM recipes WHERE id={};", id).c_str(),
					[](void *recipe, int, char **col_data, char**) {
					*static_cast<struct recipe*>(recipe) = { std::atoi(col_data[0]), col_data[1], col_data[2] };
					return 0;
					}, &recipe, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error("Failed to select from database.");
	}

	return recipe;
}

std::vector<struct recipe> db::get_recipes(const std::vector<std::string> &ingredients,
										   const std::vector<std::string> &tags)
{
	std::vector<struct recipe> recipes;
	std::string stmt = "SELECT id,name,description FROM recipes";
	std::string filters;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

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

			if((id = get_ingredient_id(i)) <= 0)
				throw std::runtime_error(std::format("Failed to find ingredient '{}'", i));

			filters += std::to_string(id);
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

			if((id = get_tag_id(i)) <= 0)
				throw std::runtime_error("Failed to find tag '{}'");

			filters += std::to_string(id);
			filters += ")";
		}
	}

	stmt += filters + ";";

	if(sqlite3_exec(sqlite_db, stmt.c_str(),
					[](void *recipe_list, int, char **col_data, char**) {
					static_cast<std::vector<struct recipe>*>(recipe_list)->push_back({
																					 std::atoi(col_data[0]),
																					 col_data[1],
																					 col_data[2] });
					return 0;
					}, &recipes, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error("Failed to select recipes.");
	}

	return recipes;
}

int db::add_ingredient(const std::string &name) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("INSERT OR IGNORE INTO ingredients(name) VALUES(lower('{}'));", name).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to instert ingredient '{}'.", name));
	}

	return get_ingredient_id(name);
}

std::vector<std::string> db::get_recipe_ingredients(const int id) {
	std::vector<std::string> ingredients;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("SELECT name FROM ingredients WHERE id IN (SELECT ingredient_id FROM recipe_ingredient WHERE recipe_id={});", id).c_str(),
					[](void *ingredients, int, char **col_data, char**) {
					static_cast<std::vector<std::string>*>(ingredients)->push_back(col_data[0]);
					return 0;
					}, &ingredients, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to select ingredients from recipe with ID {}", id));
	}

	return ingredients;
}

int db::add_tag(const std::string &name) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("INSERT OR IGNORE INTO tags(name) VALUES('{}');", name).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to insert tag '{}'", name));
	}

	return get_tag_id(name);
}

std::vector<std::string> db::get_recipe_tags(const int id) {
	std::vector<std::string> tags;

	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("SELECT name FROM tags WHERE id IN (SELECT tag_id FROM recipe_tag WHERE recipe_id={});", id).c_str(),
					[](void *tags, int, char **col_data, char**) {
					static_cast<std::vector<std::string>*>(tags)->push_back(col_data[0]);
					return 0;
					}, &tags, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to select tags for recipe with ID {}", id));
	}

	return tags;
}

void db::conn_recipe_ingredient(const int recipe_id, const int ingredient_id) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("INSERT OR IGNORE INTO recipe_ingredient(recipe_id, ingredient_id) VALUES({},{});", recipe_id, ingredient_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to connect recipe with ID {} to ingredient with ID {}",
												  recipe_id, ingredient_id));
	}
}

void db::disconn_recipe_ingredient(const int recipe_id, const int ingredient_id) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("DELETE FROM recipe_ingredient WHERE recipe_id={} AND ingredient_id={};", recipe_id, ingredient_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to disconnect recipe with ID {} from ingredient with ID {}.", recipe_id, ingredient_id));
	}
}

void db::conn_recipe_tag(const int recipe_id, const int tag_id) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("INSERT OR IGNORE INTO recipe_tag(recipe_id, tag_id) VALUES({},{});", recipe_id, tag_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to connect recipe with ID {} to tag with ID {}",
												  recipe_id, tag_id));
	}
}

void db::disconn_recipe_tag(const int recipe_id, const int tag_id) {
	if(not sqlite_db)
		throw std::runtime_error(std::format("{}: Database not open! Please contact a developer.", __PRETTY_FUNCTION__));

	if(sqlite3_exec(sqlite_db, std::format("DELETE FROM recipe_tag WHERE recipe_id={} AND tag_id={};", recipe_id, tag_id).c_str(),
					nullptr, nullptr, nullptr) not_eq SQLITE_OK) {
		throw std::runtime_error(std::format("Failed to disconnect recipe with ID {} from tag with ID {}.", recipe_id, tag_id));
	}
}
