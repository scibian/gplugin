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

#include <gplugin/gplugin-loader.h>

#include <gplugin/gplugin-core.h>

/******************************************************************************
 * API
 *****************************************************************************/
GType
gplugin_loader_get_type(void) {
	static volatile gsize type_volatile = 0;

	if(g_once_init_enter(&type_volatile)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginLoaderClass),
			.instance_size = sizeof(GPluginLoader),
		};

		type = g_type_register_static(G_TYPE_OBJECT,
		                              "GPluginLoader",
		                              &info, G_TYPE_FLAG_ABSTRACT);

		g_once_init_leave(&type_volatile, type);
	}

	return type_volatile;
}

/**
 * gplugin_loader_query_plugin:
 * @loader: #GPluginLoader instance performing the query
 * @filename: filename to query
 * error: return location for a GError, or NULL
 *
 * Return value: (transfer full): A #GPluginPlugin instance or NULL on failure
 */
GPluginPlugin *
gplugin_loader_query_plugin(GPluginLoader *loader,
                            const gchar *filename, GError **error)
{
	GPluginLoaderClass *klass = NULL;

	g_return_val_if_fail(loader != NULL, NULL);
	g_return_val_if_fail(GPLUGIN_IS_LOADER(loader), NULL);
	g_return_val_if_fail(filename, NULL);
	g_return_val_if_fail(error != NULL, NULL);

	klass = GPLUGIN_LOADER_GET_CLASS(loader);

	if(klass && klass->query)
		return klass->query(loader, filename, error);

	return NULL;
}

/**
 * gplugin_loader_load_plugin:
 * @loader: #GPluginLoader instance performing the load
 * @plugin: #GPluginPlugin instance to load
 * error: return location for a GError, or NULL
 *
 * Return value: TRUE if @plugin was loaded successfully, FALSE otherwise
 */
gboolean
gplugin_loader_load_plugin(GPluginLoader *loader,
                           GPluginPlugin *plugin, GError **error)
{
	GPluginLoaderClass *klass = NULL;
	gboolean ret = FALSE;

	g_return_val_if_fail(loader != NULL, FALSE);
	g_return_val_if_fail(GPLUGIN_IS_LOADER(loader), FALSE);
	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);
	g_return_val_if_fail(error != NULL, FALSE);

	klass = GPLUGIN_LOADER_GET_CLASS(loader);

	if(klass && klass->load)
		ret = klass->load(loader, plugin, error);

	if(!ret && error && *error == NULL) {
		*error = g_error_new(GPLUGIN_DOMAIN, 0,
		                     "Failed to load plugin : unknown");
	}

	return ret;
}

/**
 * gplugin_loader_unload_plugin:
 * @loader: #GPluginLoader instance performing the unload
 * @plugin: #GPluginPlugin instance to unload
 * @error: return location for a GError, or NULL
 *
 * Return value: TRUE if @plugin was unloaded successfully, FALSE otherwise
 */
gboolean
gplugin_loader_unload_plugin(GPluginLoader *loader,
                             GPluginPlugin *plugin, GError **error)
{
	GPluginLoaderClass *klass = NULL;
	gboolean ret = FALSE;

	g_return_val_if_fail(loader != NULL, FALSE);
	g_return_val_if_fail(GPLUGIN_IS_LOADER(loader), FALSE);
	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);

	klass = GPLUGIN_LOADER_GET_CLASS(loader);

	if(klass && klass->unload)
		ret = klass->unload(loader, plugin, error);

	if(!ret && error && *error == NULL) {
		*error = g_error_new(GPLUGIN_DOMAIN, 0,
		                     "Failed to unload plugin : unknown");
	}

	return ret;
}

/**
 * gplugin_loader_class_get_supported_extensions:
 * @klass: #GPluginLoader instance
 *
 * Returns a #GSList of string for which extensions the loader supports.
 *
 * Return value: (element-type utf8) (transfer container): A #GSList of
 *               extensions that the loader supports.
 */
GSList *
gplugin_loader_class_get_supported_extensions(const GPluginLoaderClass *klass) {
	g_return_val_if_fail(GPLUGIN_IS_LOADER_CLASS(klass), NULL);

	if(klass->supported_extensions)
		return klass->supported_extensions(klass);

	return NULL;
}

