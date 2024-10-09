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
#include "util.hpp"
#include <algorithm>
#include <cctype>

std::vector<std::string> split(std::string str, const std::string &delim) {
	std::vector<std::string> result;
	std::string substr;
	size_t pos = 0;

	while((pos = str.find(delim)) not_eq std::string::npos) {
		substr = str.substr(0, pos);
		result.push_back(substr);
		str.erase(0, pos + delim.size());
	}
	result.push_back(str);

	return result;
}

void trim(std::string &str) {
	str.erase(str.begin(),
			  std::find_if(str.begin(), str.end(), [](char c) {
								 return not std::isspace(c);
								 }));
	str.erase(std::find_if(str.rbegin(), str.rend(), [](char c) {
						   return not std::isspace(c);
						   }).base(), str.end());
}
