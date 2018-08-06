/*
 * Copyright (C) 2011-2014 Gary Kramlich <grim@reaperworld.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int
main(int argc, char *argv[]) {
	lua_State *L = luaL_newstate();
	int ret = 0;

	luaL_openlibs(L);

	lua_getglobal(L, "require");
	lua_pushstring(L, "lgi");

	ret = lua_pcall(L, 1, 1, 0);

	return ret;
}

