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

#include <gplugin/gplugin-native-plugin.h>

#include <gplugin/gplugin-native-private.h>

#include <gplugin/gplugin-loader.h>
#include <gplugin/gplugin-manager.h>

#include <gplugin/gplugin-core.h>

#include <glib/gi18n.h>

#define GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_NATIVE_PLUGIN, GPluginNativePluginPrivate))

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct _GPluginNativePluginPrivate       GPluginNativePluginPrivate;
typedef struct _GPluginNativePluginTypeInfo      GPluginNativePluginTypeInfo;
typedef struct _GPluginNativePluginInterfaceInfo GPluginNativePluginInterfaceInfo;

struct _GPluginNativePluginPrivate {
	GModule *module;

	gpointer load_func;
	gpointer unload_func;

	GSList *type_infos;
	GSList *interface_infos;

	guint use_count;
};

struct _GPluginNativePluginTypeInfo {
	gboolean loaded;
	GType type;
	GType parent;
	GTypeInfo info;
};

struct _GPluginNativePluginInterfaceInfo {
	gboolean loaded;
	GType instance_type;
	GType interface_type;
	GInterfaceInfo info;
};

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_MODULE,
	PROP_LOAD_FUNC,
	PROP_UNLOAD_FUNC,
	PROP_LAST,
};

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;

/******************************************************************************
 * Helpers
 *****************************************************************************/
static GPluginNativePluginTypeInfo *
gplugin_native_plugin_find_type_info(GPluginNativePlugin *plugin, GType type) {
	GPluginNativePluginPrivate *priv =
		GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);
	GSList *l = NULL;

	for(l = priv->type_infos; l; l = l->next) {
		GPluginNativePluginTypeInfo *info =
			(GPluginNativePluginTypeInfo *)(l->data);

		if(info->type == type)
			return info;
	}

	return NULL;
}

static GPluginNativePluginInterfaceInfo *
gplugin_native_plugin_find_interface_info(GPluginNativePlugin *plugin,
                                          GType instance_type,
                                          GType interface_type)
{
	GPluginNativePluginPrivate *priv =
		GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);
	GSList *l = NULL;

	for(l = priv->interface_infos; l; l = l->next) {
		GPluginNativePluginInterfaceInfo *info =
			(GPluginNativePluginInterfaceInfo *)(l->data);

		if(info->instance_type == instance_type &&
		   info->interface_type == interface_type)
		{
			return info;
		}
	}

	return NULL;
}

/******************************************************************************
 * GTypePlugin Implementation
 *****************************************************************************/
static void
gplugin_native_plugin_priv_use(GTypePlugin *plugin) {
	GPluginNativePlugin *native = GPLUGIN_NATIVE_PLUGIN(plugin);
	GError *error = NULL;

	if(!gplugin_native_plugin_use(native, &error)) {
		const GPluginPluginInfo *info =
			gplugin_plugin_get_info(GPLUGIN_PLUGIN(native));
		const gchar *name = NULL;

		if(GPLUGIN_IS_PLUGIN_INFO(info))
			name = gplugin_plugin_info_get_name(info);

		g_warning(_("Could not reload previously loaded plugin '%s': %s\n"),
		          name ? name : _("(unknown)"),
		          error ? error->message : _("unknown"));

		g_object_unref(G_OBJECT(info));

		if (error)
			g_error_free(error);
	}
}

static void
gplugin_native_plugin_priv_unuse(GTypePlugin *plugin) {
	GPluginNativePlugin *native = GPLUGIN_NATIVE_PLUGIN(plugin);
	GError *error = NULL;

	if(!gplugin_native_plugin_unuse(native, &error)) {
		const GPluginPluginInfo *info =
			gplugin_plugin_get_info(GPLUGIN_PLUGIN(native));
		const gchar *name = NULL;

		if(GPLUGIN_IS_PLUGIN_INFO(info))
			name = gplugin_plugin_info_get_name(info);

		g_warning(_("Could not unuse plugin '%s': %s\n"),
		          name ? name : _("(unknown)"),
		          error ? error->message : _("unknown"));

		g_object_unref(G_OBJECT(info));

		if (error)
			g_error_free(error);
	}
}

static void
gplugin_native_plugin_complete_type_info(GTypePlugin *plugin, GType type,
                                         GTypeInfo *info,
                                         GTypeValueTable *value_table)
{
	GPluginNativePlugin *native = GPLUGIN_NATIVE_PLUGIN(plugin);
	GPluginNativePluginTypeInfo *native_info =
		gplugin_native_plugin_find_type_info(native, type);

	*info = native_info->info;

	if(native_info->info.value_table)
		*value_table = *native_info->info.value_table;
}

static void
gplugin_native_plugin_complete_interface_info(GTypePlugin *plugin,
                                              GType instance_type,
                                              GType interface_type,
                                              GInterfaceInfo *info)
{
	GPluginNativePlugin *native = GPLUGIN_NATIVE_PLUGIN(plugin);
	GPluginNativePluginInterfaceInfo *iface_info =
		gplugin_native_plugin_find_interface_info(native, instance_type,
		                                          interface_type);

	*info = iface_info->info;
}

static void
gplugin_native_plugin_iface_init(GTypePluginClass *iface) {
	iface->use_plugin = gplugin_native_plugin_priv_use;
	iface->unuse_plugin = gplugin_native_plugin_priv_unuse;
	iface->complete_type_info = gplugin_native_plugin_complete_type_info;
	iface->complete_interface_info =
		gplugin_native_plugin_complete_interface_info;
}

/******************************************************************************
 * GObject Implementation
 *****************************************************************************/
static void
gplugin_native_plugin_get_property(GObject *obj, guint param_id, GValue *value,
                                   GParamSpec *pspec)
{
	GPluginNativePlugin *plugin = GPLUGIN_NATIVE_PLUGIN(obj);
	GPluginNativePluginPrivate *priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	switch(param_id) {
		case PROP_MODULE:
			g_value_set_pointer(value,
			                    gplugin_native_plugin_get_module(plugin));
			break;
		case PROP_LOAD_FUNC:
			g_value_set_pointer(value, priv->load_func);
			break;
		case PROP_UNLOAD_FUNC:
			g_value_set_pointer(value, priv->unload_func);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_native_plugin_set_property(GObject *obj, guint param_id,
                                   const GValue *value, GParamSpec *pspec)
{
	GPluginNativePluginPrivate *priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(obj);

	switch(param_id) {
		case PROP_MODULE:
			priv->module = g_value_get_pointer(value);
			break;
		case PROP_LOAD_FUNC:
			priv->load_func = g_value_get_pointer(value);
			break;
		case PROP_UNLOAD_FUNC:
			priv->unload_func = g_value_get_pointer(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_native_plugin_finalize(GObject *obj) {
	GPluginNativePluginPrivate *priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(obj);

	g_slist_free(priv->type_infos);
	g_slist_free(priv->interface_infos);

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_native_plugin_class_init(GPluginNativePluginClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginNativePluginPrivate));

	obj_class->finalize = gplugin_native_plugin_finalize;
	obj_class->get_property = gplugin_native_plugin_get_property;
	obj_class->set_property = gplugin_native_plugin_set_property;

	/**
	 * GPluginNativePlugin:module:
	 *
	 * The GModule instance for this plugin.
	 */
	g_object_class_install_property(obj_class, PROP_MODULE,
		g_param_spec_pointer("module", "module handle",
		                     "The GModule instance of the plugin",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginNativePlugin:load-func:
	 *
	 * A function pointer to the load method of the plugin.
	 */
	g_object_class_install_property(obj_class, PROP_LOAD_FUNC,
		g_param_spec_pointer("load-func", "load function pointer",
		                     "address pointer to load function",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginNativePlugin:unload-func:
	 *
	 * A function pointer to the unload method of the plugin.
	 */
	g_object_class_install_property(obj_class, PROP_UNLOAD_FUNC,
		g_param_spec_pointer("unload-func", "unload function pointer",
		                     "address pointer to the unload function",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

/******************************************************************************
 * API
 *****************************************************************************/
GType
gplugin_native_plugin_get_type(void) {
	static volatile gsize type_volatile = 0;

	if(g_once_init_enter(&type_volatile)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginNativePluginClass),
			.class_init = (GClassInitFunc)gplugin_native_plugin_class_init,
			.instance_size = sizeof(GPluginNativePlugin),
		};

		static const GInterfaceInfo iface_info = {
			.interface_init = (GInterfaceInitFunc)gplugin_native_plugin_iface_init,
		};

		type = g_type_register_static(GPLUGIN_TYPE_PLUGIN,
		                              "GPluginNativePlugin",
		                              &info, 0);

		g_type_add_interface_static(type, G_TYPE_TYPE_PLUGIN, &iface_info);

		g_once_init_leave(&type_volatile, type);
	}

	return type_volatile;
}

/**
 * gplugin_native_plugin_use:
 * @plugin: a #GPluginNativePlugin
 * @error: (out): return location for a #GError or null
 *
 * Increments the ref count of @plugin by one.
 *
 * See: g_type_module_use
 *
 * Returns: FALSE if @plugin needed to be loaded and loading failed.
 */
gboolean
gplugin_native_plugin_use(GPluginNativePlugin *plugin,
                          GPLUGIN_UNUSED GError **error)
{
	GPluginNativePluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), FALSE);

	priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	priv->use_count++;
	if(priv->use_count == 1) {
		GSList *l = NULL;

		for(l = priv->type_infos; l; l = l->next) {
			GPluginNativePluginTypeInfo *info =
				(GPluginNativePluginTypeInfo *)(l->data);

			if(!info->loaded) {
				const GPluginPluginInfo *plugin_info =
					gplugin_plugin_get_info(GPLUGIN_PLUGIN(plugin));
				const gchar *name = NULL;

				if(GPLUGIN_IS_PLUGIN_INFO(plugin_info))
					name = gplugin_plugin_info_get_name(plugin_info);

				if(name == NULL)
					name = _("(unknown)");

				g_warning(_("plugin '%s' failed to register type '%s'\n"),
				          name, g_type_name(info->type));

				g_object_unref(G_OBJECT(plugin_info));

				return FALSE;
			}
		}
	}

	return TRUE;
}

/**
 * gplugin_native_plugin_unuse:
 * @plugin: a #GPluginNativePlugin
 * @error: (out): return location for a #GError or null
 *
 * Decreases the ref count of @plugin by one.  If the result is zero, @plugin
 * is unloaded.
 *
 * See: g_type_module_unuse
 *
 * Returns: TRUE if successful, FALSE otherwise.
 */
gboolean
gplugin_native_plugin_unuse(GPluginNativePlugin *plugin,
                            GPLUGIN_UNUSED GError **error)
{
	GPluginNativePluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), FALSE);

	priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	g_return_val_if_fail(priv->use_count > 0, FALSE);

	priv->use_count--;

	if(priv->use_count == 0) {
		GSList *l = NULL;

		for(l = priv->type_infos; l; l = l->next) {
			GPluginNativePluginTypeInfo *info =
				(GPluginNativePluginTypeInfo *)(l->data);

			info->loaded = FALSE;
		}
	}

	return TRUE;
}

/**
 * gplugin_native_plugin_register_type:
 * @plugin: a #GPluginNativePlugin
 * @parent: the type for the parent class
 * @name: name for the type
 * @info: type information structure
 * @flags: flags field providing details about the type.
 *
 * Looks up or registers a type that is implemented in @plugin.
 *
 * See: g_type_module_register_type
 *
 * Returns: the new or existing type ID
 */
GType
gplugin_native_plugin_register_type(GPluginNativePlugin *plugin, GType parent,
                                    const gchar *name, const GTypeInfo *info,
                                    GTypeFlags flags)
{
	GPluginNativePluginPrivate *priv = NULL;
	GPluginNativePluginTypeInfo *type_info = NULL;
	GType type = G_TYPE_INVALID;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), G_TYPE_INVALID);
	g_return_val_if_fail(name, G_TYPE_INVALID);
	g_return_val_if_fail(info, G_TYPE_INVALID);

	priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	type = g_type_from_name(name);

	if(type != G_TYPE_INVALID) {
		/* this type is already loaded, let's make sure it's the same plugin
		 * loading it again.
		 */
		GTypePlugin *old = g_type_get_plugin(type);

		if(old != G_TYPE_PLUGIN(plugin)) {
			g_warning(_("Two different plugins tried to register '%s'"),
			          name);

			return G_TYPE_INVALID;
		}

		/* The same plugin is reloading the type */
		type_info = gplugin_native_plugin_find_type_info(plugin, type);
		if(type_info == NULL)
			return G_TYPE_INVALID;

		if(type_info->parent != parent) {
			/* eek, them bastards tried to reparent this type! */

			const gchar *parent_name = g_type_name(parent);

			g_warning(_("Type '%s' was recreated with a different parent type."
			          "(was '%s', now '%s')"),
			          name, g_type_name(type_info->parent),
			          (parent_name) ? parent_name : _("unknown"));

			return G_TYPE_INVALID;
		}

		/* we need to free the old vtable if the old type had one */
		if(type_info->info.value_table)
			g_free((GTypeValueTable *)type_info->info.value_table);
	} else {
		/* the type hasn't been loaded before, so now we need to add it */
		type_info = g_new(GPluginNativePluginTypeInfo, 1);

		type_info->parent = parent;
		type_info->type = g_type_register_dynamic(parent, name,
		                                          G_TYPE_PLUGIN(plugin),
		                                          flags);

		priv->type_infos = g_slist_prepend(priv->type_infos, type_info);
	}

	/* ok, now finish up */
	type_info->loaded = TRUE;
	type_info->info = *info;

	if(info->value_table) {
		type_info->info.value_table = g_memdup(info->value_table,
		                                       sizeof(GTypeValueTable));
	}

	if(g_type_is_a(type_info->type, GPLUGIN_TYPE_LOADER) &&
	   !G_TYPE_IS_ABSTRACT(type_info->type))
	{
		gplugin_manager_register_loader(type_info->type);
	}

	return type_info->type;
}

/**
 * gplugin_native_plugin_add_interface:
 * @plugin: a #GPluginNativePlugin
 * @instance_type: type to which to add the interface
 * @interface_type: interface type to add
 * @interface_info: type information structure
 *
 * Registers an additional interface for a type that lives in @plugin.
 *
 * See: g_type_module_add_interface
 */
void
gplugin_native_plugin_add_interface(GPluginNativePlugin *plugin,
                                    GType instance_type, GType interface_type,
                                    const GInterfaceInfo *interface_info)
{
	GPluginNativePluginPrivate *priv = NULL;
	GPluginNativePluginInterfaceInfo *iface_info = NULL;

	g_return_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin));
	g_return_if_fail(interface_info);

	priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	if(g_type_is_a(instance_type, interface_type)) {
		GTypePlugin *old = g_type_interface_get_plugin(instance_type,
		                                               interface_type);

		if(!old) {
			g_warning(_("Interface '%s' for '%s' was previously registered "
			          "statically or for a parent type."),
			          g_type_name(interface_type),
			          g_type_name(instance_type));
			return;
		} else if(old != G_TYPE_PLUGIN(plugin)) {
			g_warning(_("Two different plugins tried to register interface "
			          "'%s' for '%s')"),
			          g_type_name(interface_type),
			          g_type_name(instance_type));
			return;
		}

		iface_info = gplugin_native_plugin_find_interface_info(plugin,
		                                                       instance_type,
		                                                       interface_type);

		g_return_if_fail(iface_info);
	} else {
		iface_info = g_new0(GPluginNativePluginInterfaceInfo, 1);

		iface_info->instance_type = instance_type;
		iface_info->interface_type = interface_type;

		g_type_add_interface_dynamic(instance_type, interface_type,
		                             G_TYPE_PLUGIN(plugin));

		priv->interface_infos = g_slist_prepend(priv->interface_infos,
		                                        iface_info);
	}

	iface_info->loaded = TRUE;
	iface_info->info = *interface_info;
}

/**
 * gplugin_native_plugin_register_enum:
 * @plugin: a #GPluginNativePlugin
 * @name: a name for the type
 * @values: an array of %GEnumValue structs for the possible enumeration
 *          values.  The array is terminated by a struct with all members being
 *          0.
 *
 * Looks up or registers a new enumeration named @name with the given @values,
 * and associates it with @plugin.
 *
 * See: g_type_module_register_enum
 *
 * Returns: the new or existing type ID.
 */
GType
gplugin_native_plugin_register_enum(GPluginNativePlugin *plugin,
                                    const gchar *name,
                                    const GEnumValue *values)
{
	GTypeInfo enum_info;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), G_TYPE_INVALID);
	g_return_val_if_fail(name, G_TYPE_INVALID);
	g_return_val_if_fail(values, G_TYPE_INVALID);

	g_enum_complete_type_info(G_TYPE_ENUM, &enum_info, values);

	return gplugin_native_plugin_register_type(plugin, G_TYPE_ENUM, name,
	                                           &enum_info, 0);
}

/**
 * gplugin_native_plugin_register_flags:
 * @plugin: a #GPluginNativePlugin
 * @name: name for the type
 * @values: an array of %GFlagValue structs for the possible flags values.  The
 *          array is terminated by a struct with all members being 0.
 *
 * See: g_type_module_register_flags
 *
 * Returns: the new or existing type ID.
 */
GType
gplugin_native_plugin_register_flags(GPluginNativePlugin *plugin,
                                     const gchar *name,
                                     const GFlagsValue *values)
{
	GTypeInfo flags_info;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), G_TYPE_INVALID);
	g_return_val_if_fail(name, G_TYPE_INVALID);
	g_return_val_if_fail(values, G_TYPE_INVALID);

	g_flags_complete_type_info(G_TYPE_FLAGS, &flags_info, values);

	return gplugin_native_plugin_register_type(plugin, G_TYPE_FLAGS, name,
	                                           &flags_info, 0);
}

/**
 * gplugin_native_plugin_get_module: (skip)
 * @plugin: #GPluginNativePlugin instance
 *
 * Returns the %GModule associated with this plugin.  This should really only
 * be used if you need to make your plugin resident.
 *
 * Returns: The %GModule associated with this plugin.
 */
GModule *
gplugin_native_plugin_get_module(GPluginNativePlugin *plugin) {
	GPluginNativePluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_NATIVE_PLUGIN(plugin), NULL);

	priv = GPLUGIN_NATIVE_PLUGIN_GET_PRIVATE(plugin);

	return priv->module;
}

