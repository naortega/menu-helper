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
#include "cmd.h"
#include "db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int command_add(void) {
	char *name = NULL, *description = NULL, *ingredients = NULL, *tags = NULL;
	size_t name_len, description_len, ingredients_len, tags_len;
	int recipe_id, ingredient_id, tag_id;

	if(!db_open()) {
		fprintf(stderr, "Failed to open database. Cannot add new entry.\n");
		return 0;
	}

	printf("Name: ");
	getline(&name, &name_len, stdin);
	// eliminate trailing newline
	name[strlen(name) - 1] = '\0';

	printf("Description: ");
	getline(&description, &description_len, stdin);
	// eliminate trailing newline
	description[strlen(description) - 1] = '\0';

	printf("Ingredients (comma separated): ");
	getline(&ingredients, &ingredients_len, stdin);
	// eliminate trailing newline
	ingredients[strlen(ingredients) - 1] = '\0';

	printf("Tags (comma separated): ");
	getline(&tags, &tags_len, stdin);
	// eliminate trailing newline
	tags[strlen(tags) - 1] = '\0';

	if((recipe_id = db_get_recipe_id(name)) <= 0)
		recipe_id = db_add_recipe(name, description);
	free(name);
	free(description);

	for(char *i = strtok(ingredients, ","); i; i = strtok(NULL,",")) {
		// remove leading blank spaces
		while(isblank(i[0]))
			i += sizeof(char);

		// remove trailing blank spaces
		size_t i_len = strlen(i);
		while(isblank(i[i_len - 1])) {
			i[i_len - 1] = '\0';
			--i_len;
		}

		if((ingredient_id = db_get_ingredient_id(i)) <= 0)
			ingredient_id = db_add_ingredient(i);
		db_conn_recipe_ingredient(recipe_id, ingredient_id);
	}
	free(ingredients);

	for(char *i = strtok(tags, ","); i; i = strtok(NULL, ",")) {
		// remove leading blank spaces
		while(isblank(i[0]))
			i += sizeof(char);

		// remove trailing blank spaces
		size_t i_len = strlen(i);
		while(isblank(i[i_len - 1])) {
			i[i_len - 1] = '\0';
			--i_len;
		}

		if((tag_id = db_get_tag_id(i)) <= 0)
			tag_id = db_add_tag(i);
		db_conn_recipe_tag(recipe_id, tag_id);
	}
	free(tags);

	db_close();

	return 1;
}
