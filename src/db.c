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
#include "db.h"

#include <sqlite3.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static sqlite3 *db = NULL;
static const int db_version = 1;

int db_open(void) {
	const char *xdg_data_home;
	char *db_path;
	int new_db = 0;
	int rc;

	if(!(xdg_data_home = getenv("XDG_DATA_HOME"))) {
		printf("Cannot find environment variable XDG_DATA_HOME. Please define it before continuing. E.g.:\nexport XDG_DATA_HOME=\"$HOME/.local/share\"\n");
		return 0;
	}

	db_path = malloc(strlen(xdg_data_home) + strlen("/menu-helper") + strlen("/recipes.db") + 1);
	strcpy(db_path, xdg_data_home);
	strcat(db_path, "/menu-helper");

	if(access(db_path, F_OK) != 0)
		mkdir(db_path, 0700);

	strcat(db_path, "/recipes.db");

	if(access(db_path, F_OK) != 0) {
		printf("Creating database: %s\n", db_path);
		new_db = 1;
	}

	rc = sqlite3_open(db_path, &db);
	free(db_path);

	if(rc == SQLITE_OK && new_db) {
		char insert_version_stmt[64];
		sqlite3_exec(db, "CREATE TABLE db_version(version INTEGER UNIQUE NOT NULL);", NULL, NULL, NULL);
		snprintf(insert_version_stmt, 64, "INSERT INTO db_version VALUES(%d);", db_version);
		sqlite3_exec(db, insert_version_stmt, NULL, NULL, NULL);
		sqlite3_exec(db, "CREATE TABLE tags(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", NULL, NULL, NULL);
		sqlite3_exec(db, "CREATE TABLE ingredients(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE);", NULL, NULL, NULL);
		sqlite3_exec(db, "CREATE TABLE recipes(id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING UNIQUE, description STRING);", NULL, NULL, NULL);
		sqlite3_exec(db, "CREATE TABLE recipe_tag(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, tag_id INTEGER REFERENCES tags(id) ON DELETE CASCADE);", NULL, NULL, NULL);
		sqlite3_exec(db, "CREATE TABLE recipe_ingredient(recipe_id INTEGER REFERENCES recipes(id) ON DELETE CASCADE, ingredient_id INTEGER REFERENCES ingredients(id) ON DELETE CASCADE);", NULL, NULL, NULL);
	}

	return rc == SQLITE_OK;
}

void db_close(void) {
	if(!db)
		return;

	sqlite3_close(db);
}

int query_id_cb(void *recipe_id_var, int col_num, char **col_data, char **col_name) {
	int *recipe_id_ptr = (int*)recipe_id_var;
	int ret = 1;

	for(int i = 0; i < col_num; ++i) {
		if(strcmp(col_name[i], "id") == 0) {
			*recipe_id_ptr = atoi(col_data[i]);
			ret = 0;
			break;
		}
	}

	return ret;
}

int table_get_id_by_name(const char *table, const char *name) {
	const char *sel_query_fmt = "SELECT id FROM %s WHERE lower(name)=lower('%s');";
	char *sel_query;
	int id = 0;

	sel_query = malloc(strlen(table) + strlen(name) + strlen(sel_query_fmt) + 1);
	sprintf(sel_query, sel_query_fmt, table, name);
	if(sqlite3_exec(db, sel_query, query_id_cb, &id, NULL) != SQLITE_OK) {
		free(sel_query);
		return -2;
	}

	free(sel_query);

	return id;
}

int db_add_recipe(const char *name, const char *description) {
	const char *add_query_fmt = "INSERT INTO recipes(name,description) VALUES('%s','%s');";
	char *add_query;

	if(!db)
		return -1;

	add_query = malloc(strlen(name) + strlen(description) + strlen(add_query_fmt) + 1);
	sprintf(add_query, add_query_fmt, name, description);
	if(sqlite3_exec(db, add_query, NULL, NULL, NULL) != SQLITE_OK) {
		free(add_query);
		return -2;
	}
	free(add_query);

	return db_get_recipe_id(name);
}

int db_get_recipe_id(const char *name) {
	if(!db)
		return -1;

	return table_get_id_by_name("recipes", name);
}

int db_add_ingredient(const char *name) {
	const char *add_query_fmt = "INSERT INTO ingredients(name) VALUES(lower('%s'));";
	char *add_query;

	if(!db)
		return -1;

	add_query = malloc(strlen(name) + strlen(add_query_fmt) + 1);
	sprintf(add_query, add_query_fmt, name);
	if(sqlite3_exec(db, add_query, NULL, NULL, NULL) != SQLITE_OK) {
		free(add_query);
		return -2;
	}
	free(add_query);

	return db_get_ingredient_id(name);
}

int db_get_ingredient_id(const char *name) {
	if(!db)
		return -1;

	return table_get_id_by_name("ingredients", name);
}

int db_add_tag(const char *name) {
	const char *add_query_fmt = "INSERT INTO tags(name) VALUES('%s');";
	char *add_query;

	if(!db)
		return -1;

	add_query = malloc(strlen(name) + strlen(add_query_fmt) + 1);
	sprintf(add_query, add_query_fmt, name);
	if(sqlite3_exec(db, add_query, NULL, NULL, NULL) != SQLITE_OK) {
		free(add_query);
		return -2;
	}
	free(add_query);

	return db_get_tag_id(name);
}

int db_get_tag_id(const char *name) {
	if(!db)
		return -1;

	return table_get_id_by_name("tags", name);
}

int db_conn_recipe_ingredient(int recipe_id, int ingredient_id) {
	const char *add_conn_fmt = "INSERT INTO recipe_ingredient(recipe_id, ingredient_id) VALUES(%d,%d);";
	char *add_conn_query;

	if(!db)
		return -1;

	add_conn_query = malloc(strlen(add_conn_fmt) + (recipe_id % 10) + (ingredient_id % 10));
	sprintf(add_conn_query, add_conn_fmt, recipe_id, ingredient_id);
	if(sqlite3_exec(db, add_conn_query, NULL, NULL, NULL) != SQLITE_OK) {
		free(add_conn_query);
		return -2;
	}
	free(add_conn_query);

	return 1;
}

int db_conn_recipe_tag(int recipe_id, int tag_id) {
	const char *add_conn_fmt = "INSERT INTO recipe_tag(recipe_id, tag_id) VALUES(%d,%d);";
	char *add_conn_query;

	if(!db)
		return -1;

	add_conn_query = malloc(strlen(add_conn_fmt) + (recipe_id % 10) + (tag_id % 10));
	sprintf(add_conn_query, add_conn_fmt, recipe_id, tag_id);
	if(sqlite3_exec(db, add_conn_query, NULL, NULL, NULL) != SQLITE_OK) {
		free(add_conn_query);
		return -2;
	}
	free(add_conn_query);

	return 1;
}
