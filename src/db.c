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
static const  int db_version = 1;

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

	if(access(db_path, F_OK) != 0) {
		printf("Creating database: %s\n", db_path);
		mkdir(db_path, 0700);
		new_db = 1;
	}

	strcat(db_path, "/recipes.db");

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
