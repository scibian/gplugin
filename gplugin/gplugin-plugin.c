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

#include <gplugin/gplugin-plugin.h>

#include <gplugin/gplugin-enums.h>
#include <gplugin/gplugin-marshallers.h>
#include <gplugin/gplugin-private.h>

#define GPLUGIN_PLUGIN_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_PLUGIN, GPluginPluginPrivate))

/**
 * GPluginPluginState:
 * @GPLUGIN_PLUGIN_STATE_UNKNOWN: The state of the plugin is unknown
 * @GPLUGIN_PLUGIN_STATE_ERROR: There was an error loading or unloading the
 *  plugin
 * @GPLUGIN_PLUGIN_STATE_QUERIED: The plugin has been queried but not loaded
 * @GPLUGIN_PLUGIN_STATE_REQUERY: The plugin should be requeried
 * @GPLUGIN_PLUGIN_STATE_LOADED: The plugin is loaded
 * @GPLUGIN_PLUGIN_STATE_LOAD_FAILED: The plugin failed to load
 *
 * The expected states of a plugin.
 */

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	gchar *filename;

	GPluginLoader *loader;
	GPluginPluginInfo *info;

	GPluginPluginState state;
} GPluginPluginPrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_FILENAME,
	PROP_LOADER,
	PROP_INFO,
	PROP_STATE,
	PROP_LAST,
};

enum {
	SIG_STATE_CHANGED,
	SIG_LAST,
};

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static guint signals[SIG_LAST] = {0, };

/******************************************************************************
 * Private API
 *****************************************************************************/
static void
gplugin_plugin_set_filename(GPluginPlugin *plugin, const gchar *filename) {
	GPluginPluginPrivate *priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	g_free(priv->filename);

	priv->filename = (filename) ? g_strdup(filename) : NULL;

	g_object_notify(G_OBJECT(plugin), "filename");
}

static void
gplugin_plugin_set_loader(GPluginPlugin *plugin, GPluginLoader *loader) {
	GPluginPluginPrivate *priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	if(priv->loader)
		g_object_unref(G_OBJECT(priv->loader));

	if(GPLUGIN_IS_LOADER(loader))
		priv->loader = g_object_ref(G_OBJECT(loader));
	else
		priv->loader = NULL;
}

static void
gplugin_plugin_set_info(GPluginPlugin *plugin, GPluginPluginInfo *info) {
	GPluginPluginPrivate *priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	if(priv->info)
		g_object_unref(G_OBJECT(priv->info));

	priv->info = (info) ? g_object_ref(G_OBJECT(info)) : NULL;
}

gchar *
gplugin_plugin_get_internal_filename(GPluginPlugin *plugin) {
	GPluginPluginPrivate *priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	return priv->filename;
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_plugin_get_property(GObject *obj, guint param_id, GValue *value,
                            GParamSpec *pspec)
{
	GPluginPlugin *plugin = GPLUGIN_PLUGIN(obj);

	switch(param_id) {
		case PROP_FILENAME:
			g_value_set_string(value, gplugin_plugin_get_filename(plugin));
			break;
		case PROP_LOADER:
			g_value_take_object(value, gplugin_plugin_get_loader(plugin));
			break;
		case PROP_INFO:
			g_value_take_object(value, gplugin_plugin_get_info(plugin));
			break;
		case PROP_STATE:
			g_value_set_enum(value, gplugin_plugin_get_state(plugin));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_plugin_set_property(GObject *obj, guint param_id, const GValue *value,
                            GParamSpec *pspec)
{
	GPluginPlugin *plugin = GPLUGIN_PLUGIN(obj);

	switch(param_id) {
		case PROP_FILENAME:
			gplugin_plugin_set_filename(plugin, g_value_get_string(value));
			break;
		case PROP_LOADER:
			gplugin_plugin_set_loader(plugin, g_value_get_object(value));
			break;
		case PROP_INFO:
			gplugin_plugin_set_info(plugin, g_value_get_object(value));
			break;
		case PROP_STATE:
			gplugin_plugin_set_state(plugin, g_value_get_enum(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_plugin_finalize(GObject *obj) {
	GPluginPluginPrivate *priv = GPLUGIN_PLUGIN_GET_PRIVATE(obj);

	g_free(priv->filename);
	g_object_unref(priv->loader);

	if(priv->info)
		g_object_unref(G_OBJECT(priv->info));

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_plugin_class_init(GPluginPluginClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginPluginPrivate));

	obj_class->get_property = gplugin_plugin_get_property;
	obj_class->set_property = gplugin_plugin_set_property;
	obj_class->finalize = gplugin_plugin_finalize;

	/**
	 * GPluginPlugin:filename:
	 *
	 * The absolute path to the plugin on disk.
	 */
	g_object_class_install_property(obj_class, PROP_FILENAME,
		g_param_spec_string("filename", "filename",
		                    "The filename of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPlugin:loader:
	 *
	 * The #GPluginLoader that loaded this plugin.
	 */
	g_object_class_install_property(obj_class, PROP_LOADER,
		g_param_spec_object("loader", "loader",
		                    "The loader for this plugin",
		                    GPLUGIN_TYPE_LOADER,
		                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPlugin:info:
	 *
	 * The #GPluginPluginInfo from this plugin.
	 */
	g_object_class_install_property(obj_class, PROP_INFO,
		g_param_spec_object("info", "info",
		                   "The information for the plugin",
		                   GPLUGIN_TYPE_PLUGIN_INFO,
		                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPlugin:state:
	 *
	 * The #GPluginPluginState that this plugin is in.
	 */
	g_object_class_install_property(obj_class, PROP_STATE,
		g_param_spec_enum("state", "state",
		                  "The state of the plugin",
	                      GPLUGIN_TYPE_PLUGIN_STATE,
		                  GPLUGIN_PLUGIN_STATE_UNKNOWN,
		                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	/**
	 * GPluginPlugin::state-changed:
	 * @plugin: The #GPluginPlugin that changed states.
	 * @oldstate: The old #GPluginPluginState.
	 * @newstate: The new state of @plugin.
	 *
	 * Emitted when @plugin changes state.
	 */
	signals[SIG_STATE_CHANGED] =
		g_signal_new("state-changed",
		             G_OBJECT_CLASS_TYPE(klass),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginPluginClass, state_changed),
		             NULL, NULL,
		             gplugin_marshal_VOID__ENUM_ENUM,
		             G_TYPE_NONE,
		             2,
		             GPLUGIN_TYPE_PLUGIN_STATE, GPLUGIN_TYPE_PLUGIN_STATE);
}

/******************************************************************************
 * GPluginPlugin API
 *****************************************************************************/
GType
gplugin_plugin_get_type(void) {
	static volatile gsize type_volatile = 0;

	if(g_once_init_enter(&type_volatile)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginPluginClass),
			.class_init = (GClassInitFunc)gplugin_plugin_class_init,
			.instance_size = sizeof(GPluginPlugin),
		};

		type = g_type_register_static(G_TYPE_OBJECT,
		                              "GPluginPlugin",
		                              &info, G_TYPE_FLAG_ABSTRACT);

		g_once_init_leave(&type_volatile, type);
	}

	return type_volatile;
}

/**
 * gplugin_plugin_get_filename:
 * @plugin: #GPluginPlugin instance
 *
 * Return value: The filename of @plugin
 */
const gchar *
gplugin_plugin_get_filename(const GPluginPlugin *plugin) {
	GPluginPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	return priv->filename;
}

/**
 * gplugin_plugin_get_loader:
 * @plugin: #GPluginPlugin instance
 *
 * Return Value: (transfer full): The #GPluginLoader that loaded @plugin
 */
GPluginLoader *
gplugin_plugin_get_loader(const GPluginPlugin *plugin) {
	GPluginPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	return (priv->info) ? g_object_ref(G_OBJECT(priv->loader)) : NULL;
}

/**
 * gplugin_plugin_get_info:
 * @plugin: #GPluginPlugin instance
 *
 * Return value: (transfer full): The #GPluginPluginInfo instance for @plugin
 */
GPluginPluginInfo *
gplugin_plugin_get_info(const GPluginPlugin *plugin) {
	GPluginPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	return (priv->info) ? g_object_ref(G_OBJECT(priv->info)) : NULL;
}

/**
 * gplugin_plugin_get_state:
 * @plugin: #GPluginPlugin instance
 *
 * Gets the current state of @plugin
 *
 * Return value: (transfer full): The current state of @plugin
 */
GPluginPluginState
gplugin_plugin_get_state(const GPluginPlugin *plugin) {
	GPluginPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), GPLUGIN_PLUGIN_STATE_UNKNOWN);

	priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	return priv->state;
}

/**
 * gplugin_plugin_set_state:
 * @plugin: #GPluginPlugin instance
 * @state: new #GPluginPluginState for @plugin
 *
 * Changes the state of @plugin to @state.  This function should only be called
 * by loaders.
 */
void
gplugin_plugin_set_state(GPluginPlugin *plugin, GPluginPluginState state) {
	GPluginPluginPrivate *priv = NULL;
	GPluginPluginState oldstate = GPLUGIN_PLUGIN_STATE_UNKNOWN;

	g_return_if_fail(GPLUGIN_IS_PLUGIN(plugin));

	priv = GPLUGIN_PLUGIN_GET_PRIVATE(plugin);

	oldstate = priv->state;
	priv->state = state;

	g_signal_emit(plugin, signals[SIG_STATE_CHANGED], 0,
	              oldstate, priv->state);
}

