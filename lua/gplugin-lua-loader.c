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

#include "gplugin-lua-loader.h"

#include "gplugin-lua-plugin.h"

#include <glib/gi18n.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static GType type_real = 0;

/******************************************************************************
 * Helpers
 *****************************************************************************/
static void
_gplugin_lua_error_to_gerror(lua_State *L, GError **error) {
	const gchar *msg = NULL;

	if(error == NULL)
		return;

	msg = lua_tostring(L, -1);

	*error = g_error_new(GPLUGIN_DOMAIN, 0,
	                     "%s", (msg) ? msg : "Unknown");
}

static gboolean
_gplugin_lua_loader_load_unload_plugin(GPLUGIN_UNUSED GPluginLoader *loader,
                                       GPluginPlugin *plugin,
                                       const gchar *function,
                                       GError **error)
{
	gboolean ret = TRUE;
	lua_State *L = gplugin_lua_plugin_get_state(GPLUGIN_LUA_PLUGIN(plugin));

	lua_getglobal(L, function);
	lua_pushlightuserdata(L, plugin);
	if(lua_pcall(L, 1, 1, 0) != 0) {
		_gplugin_lua_error_to_gerror(L, error);

		return FALSE;
	}

	if(!lua_isboolean(L, -1)) {
		_gplugin_lua_error_to_gerror(L, error);

		return FALSE;
	}

	if(!lua_toboolean(L, -1)) {
		ret = FALSE;
		_gplugin_lua_error_to_gerror(L, error);
	}

	return ret;
}

/******************************************************************************
 * GPluginLoaderInterface API
 *****************************************************************************/
static GSList *
gplugin_lua_loader_class_supported_extensions(GPLUGIN_UNUSED const GPluginLoaderClass *klass) {
	GSList *exts = NULL;

	exts = g_slist_append(exts, "lua");
	exts = g_slist_append(exts, "moon");

	return exts;
}

static GPluginPlugin *
gplugin_lua_loader_query(GPluginLoader *loader, const gchar *filename,
                         GError **error)
{
	GPluginPlugin *plugin = NULL;
	GPluginPluginInfo *info = NULL;
	lua_State *L = NULL;
	gchar *ext = NULL;

	L = luaL_newstate();
	luaL_openlibs(L);

	/* check the extension to see if we need to load moonscript */
	ext = g_utf8_strrchr(filename, -1, g_utf8_get_char("."));
	if(ext && g_utf8_collate(ext, ".moon") == 0) {
		lua_getglobal(L, "require");
		lua_pushstring(L, "moonscript");

		if(lua_pcall(L, 1, 1, 0) != 0) {
			_gplugin_lua_error_to_gerror(L, error);

			return NULL;
		}

		if(!lua_istable(L, -1)) {
			if(error)
				*error = g_error_new(GPLUGIN_DOMAIN, 0, "moonscript returned an unexpected value");

			return NULL;
		}

		lua_getfield(L, -1, "loadfile");
		lua_pushstring(L, filename);

		if(lua_pcall(L, 1, 1, 0) != 0) {
			_gplugin_lua_error_to_gerror(L, error);

			return NULL;
		}
	} else {
		if(luaL_loadfile(L, filename) != 0) {
			_gplugin_lua_error_to_gerror(L, error);

			return NULL;
		}
	}

	/* run the script */
	if(lua_pcall(L, 0, 0, 0) != 0) {
		_gplugin_lua_error_to_gerror(L, error);

		return NULL;
	}

	lua_getglobal(L, "gplugin_query");
	if(lua_isnil(L, -1)) {
		if(error)
			*error = g_error_new(GPLUGIN_DOMAIN, 0, "no gplugin_query function found");

		return NULL;
	}
	if(lua_pcall(L, 0, 1, 0) != 0) {
		_gplugin_lua_error_to_gerror(L, error);

		return NULL;
	}

	if(!lua_isuserdata(L, -1)) {
		_gplugin_lua_error_to_gerror(L, error);

		return NULL;
	}

	lua_getfield(L, -1, "_native");
	info = lua_touserdata(L, -1);
	lua_pop(L, 1);

	plugin = g_object_new(GPLUGIN_TYPE_LUA_PLUGIN,
	                      "filename", filename,
	                      "loader", loader,
	                      "state", L,
	                      "info", info,
	                      NULL);

	return plugin;
}

static gboolean
gplugin_lua_loader_load(GPluginLoader *loader, GPluginPlugin *plugin,
                        GError **error)
{
	return _gplugin_lua_loader_load_unload_plugin(loader, plugin,
	                                              "gplugin_load", error);
}

static gboolean
gplugin_lua_loader_unload(GPluginLoader *loader, GPluginPlugin *plugin,
                          GError **error)
{
	return _gplugin_lua_loader_load_unload_plugin(loader, plugin,
	                                              "gplugin_unload",
	                                              error);
}

/******************************************************************************
 * GObject Stuff
 *****************************************************************************/
static void
gplugin_lua_loader_class_init(GPluginLuaLoaderClass *klass) {
	GPluginLoaderClass *loader_class = GPLUGIN_LOADER_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	loader_class->supported_extensions =
		gplugin_lua_loader_class_supported_extensions;
	loader_class->query = gplugin_lua_loader_query;
	loader_class->load = gplugin_lua_loader_load;
	loader_class->unload = gplugin_lua_loader_unload;
}

/******************************************************************************
 * API
 *****************************************************************************/
void
gplugin_lua_loader_register(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginLuaLoaderClass),
			.class_init = (GClassInitFunc)gplugin_lua_loader_class_init,
			.instance_size = sizeof(GPluginLuaLoader),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           GPLUGIN_TYPE_LOADER,
		                                           "GPluginLuaLoader",
		                                           &info,
		                                           0);

		g_once_init_leave(&type_real, type);
	}
}

GType
gplugin_lua_loader_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("gplugin_lua_loader_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

