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

int cmd_add(void);
int cmd_list(int argc, char *argv[]);
int cmd_delete(int argc, char *argv[]);
int cmd_info(const int id);
int cmd_edit_name(const int id);
int cmd_edit_desc(const int id);
int cmd_add_ingr(const int recipe_id, const char *ingredients);
int cmd_rm_ingr(const int recipe_id, const char *ingredients);
int cmd_add_tag(const int recipe_id, const char *tags);
int cmd_rm_tag(const int recipe_id, const char *tags);
