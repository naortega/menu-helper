# Copyright (C) 2020  Ortega Froysa, Nicolás <nicolas@ortegas.org>
# Author: Ortega Froysa, Nicolás <nicolas@ortegas.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

DEBUG=0
INCFLAGS=
LDFLAGS=-lsqlite3
DEFS=
CFLAGS=$(INCFLAGS) -std=c++20 -Wall -Wextra -Wfatal-errors -Werror
HDRS=src/util.hpp src/arg_parse.hpp src/db.hpp src/cmd.hpp
OBJS=src/main.o src/util.o src/arg_parse.o src/db.o src/cmd.o
DOCS=menu-helper.1
VERSION=1.0

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

ifeq ($(DEBUG),1)
	CFLAGS+=-g -O0
else
	CFLAGS+=-O2 -DNDEBUG
endif

%.o:%.cpp $(HDRS)
	$(CXX) -c -o $@ $< $(CFLAGS) -DVERSION=\"$(VERSION)\"

menu-helper: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

menu-helper.1.gz: $(DOCS)
	gzip -c $< > $@

.PHONY: clean distclean install

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) menu-helper.1.gz
	$(RM) menu-helper

install: menu-helper menu-helper.1.gz
	install -d $(PREFIX)/bin
	install -m 755 menu-helper $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/man1
	install -m 644 menu-helper.1.gz $(PREFIX)/share/man/man1/
