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

#include "gplugin-lua-plugin.h"

#include <lua.h>

#define GPLUGIN_LUA_PLUGIN_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_LUA_PLUGIN, GPluginLuaPluginPrivate))

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct {
	lua_State *L;
} GPluginLuaPluginPrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_STATE,
	PROP_LAST,
};

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static GType type_real = 0;

/******************************************************************************
 * Private Stuff
 *****************************************************************************/
static void
gplugin_lua_plugin_set_state(GPluginLuaPlugin *plugin, lua_State *L) {
	GPluginLuaPluginPrivate *priv = GPLUGIN_LUA_PLUGIN_GET_PRIVATE(plugin);

	priv->L = L;
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_lua_plugin_get_property(GObject *obj, guint param_id, GValue *value,
                                   GParamSpec *pspec)
{
	GPluginLuaPlugin *plugin = GPLUGIN_LUA_PLUGIN(obj);

	switch(param_id) {
		case PROP_STATE:
			g_value_set_pointer(value,
			                    gplugin_lua_plugin_get_state(plugin));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_lua_plugin_set_property(GObject *obj, guint param_id,
                                   const GValue *value, GParamSpec *pspec)
{
	GPluginLuaPlugin *plugin = GPLUGIN_LUA_PLUGIN(obj);

	switch(param_id) {
		case PROP_STATE:
			gplugin_lua_plugin_set_state(plugin,
			                             g_value_get_pointer(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_lua_plugin_finalize(GObject *obj) {
	GPluginLuaPluginPrivate *priv = GPLUGIN_LUA_PLUGIN_GET_PRIVATE(obj);

	if(priv->L)
		lua_close(priv->L);

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_lua_plugin_class_init(GPluginLuaPluginClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginLuaPluginPrivate));

	obj_class->get_property = gplugin_lua_plugin_get_property;
	obj_class->set_property = gplugin_lua_plugin_set_property;
	obj_class->finalize = gplugin_lua_plugin_finalize;

	g_object_class_install_property(obj_class, PROP_STATE,
		g_param_spec_pointer("state", "state",
		                     "The lua state for the plugin",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

/******************************************************************************
 * API
 *****************************************************************************/
void
gplugin_lua_plugin_register(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginLuaPluginClass),
			.class_init = (GClassInitFunc)gplugin_lua_plugin_class_init,
			.instance_size = sizeof(GPluginLuaPlugin),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           GPLUGIN_TYPE_PLUGIN,
		                                           "GPluginLuaPlugin",
		                                           &info,
		                                           0);

		g_once_init_leave(&type_real, type);
	}
}

GType
gplugin_lua_plugin_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("gplugin_lua_plugin_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

lua_State *
gplugin_lua_plugin_get_state(const GPluginLuaPlugin *plugin) {
	GPluginLuaPluginPrivate *priv = GPLUGIN_LUA_PLUGIN_GET_PRIVATE(plugin);

	return priv->L;
}

