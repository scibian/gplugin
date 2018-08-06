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

#ifndef GPLUGIN_LUA_PLUGIN_H
#define GPLUGIN_LUA_PLUGIN_H

#define GPLUGIN_TYPE_LUA_PLUGIN            (gplugin_lua_plugin_get_type())
#define GPLUGIN_LUA_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_LUA_PLUGIN, GPluginLuaPlugin))
#define GPLUGIN_LUA_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((obj), GPLUGIN_TYPE_LUA_PLUGIN, GPluginLuaPluginClass))
#define GPLUGIN_IS_LUA_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_LUA_PLUGIN))
#define GPLUGIN_IS_LUA_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GPLUGIN_TYPE_LUA_PLUGIN))
#define GPLUGIN_LUA_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_LUA_PLUGIN, GPluginLuaPluginClass))

typedef struct _GPluginLuaPlugin            GPluginLuaPlugin;
typedef struct _GPluginLuaPluginClass       GPluginLuaPluginClass;

#include <gplugin.h>
#include <gplugin-native.h>

#include <lua.h>

struct _GPluginLuaPlugin {
	GPluginPlugin parent;
};

struct _GPluginLuaPluginClass {
	GPluginPluginClass parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

void gplugin_lua_plugin_register(GPluginNativePlugin *plugin);
GType gplugin_lua_plugin_get_type(void);

lua_State *gplugin_lua_plugin_get_state(const GPluginLuaPlugin *plugin);

G_END_DECLS

#endif /* GPLUGIN_LUA_PLUGIN_H */

