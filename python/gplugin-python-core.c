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

#include <gplugin.h>
#include <gplugin-native.h>

#include "gplugin-python-loader.h"
#include "gplugin-python-plugin.h"

G_MODULE_EXPORT GPluginPluginInfo *
gplugin_query(GPLUGIN_UNUSED GError **error) {
	const gchar * const authors[] = {
		"Gary Kramlich <grim@reaperworld.com>",
		NULL
	};

	return gplugin_plugin_info_new(
		"gplugin/python-loader",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"internal", TRUE,
		"load-on-query", TRUE,
		"name", "Python Plugin Loader",
		"version", GPLUGIN_VERSION,
		"license-id", "GPL3",
		"summary", "A plugin that can load python plugins",
		"description", "This plugin allows the loading of plugins written in "
		               "the python programming language.",
		"authors", authors,
		"website", GPLUGIN_WEBSITE,
		"category", "loaders",
		NULL
	);
}

G_MODULE_EXPORT gboolean
gplugin_load(GPluginNativePlugin *plugin,
             GPLUGIN_UNUSED GError **error)
{
	gplugin_python_plugin_register(plugin);
	gplugin_python_loader_register(plugin);

	gplugin_manager_register_loader(GPLUGIN_TYPE_PYTHON_LOADER);

	return TRUE;
}

G_MODULE_EXPORT gboolean
gplugin_unload(GPLUGIN_UNUSED GPluginNativePlugin *plugin,
               GPLUGIN_UNUSED GError **error)
{
	return FALSE;
}

