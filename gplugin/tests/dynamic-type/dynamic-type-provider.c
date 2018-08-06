/*
 * Copyright (C) 2013 Ankit Vani <a@nevitus.org>
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

#include "dynamic-test.h"

static GType type_real = 0;

static void
dynamic_test_register_type(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(DynamicTestClass),
			.instance_size = sizeof(DynamicTest),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           G_TYPE_OBJECT,
		                                           "DynamicTest",
		                                           &info,
		                                           0);

		g_once_init_leave(&type_real, type);
	}
}

GType
dynamic_test_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("dynamic_test_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

G_MODULE_EXPORT GPluginPluginInfo *
gplugin_query(GPLUGIN_UNUSED GError **error) {
	return gplugin_plugin_info_new(
		"gplugin/dynamic-type-provider",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		NULL
	);
}

G_MODULE_EXPORT gboolean
gplugin_load(GPluginNativePlugin *plugin, GError **error) {
	dynamic_test_register_type(plugin);

	if (type_real == 0) {
		*error = g_error_new(GPLUGIN_DOMAIN, 0,
		                     "type DynamicTest was not registered");
		return FALSE;
	}

	return TRUE;
}

G_MODULE_EXPORT gboolean
gplugin_unload(GPLUGIN_UNUSED GPluginNativePlugin *plugin,
               GPLUGIN_UNUSED GError **error)
{
	return TRUE;
}

