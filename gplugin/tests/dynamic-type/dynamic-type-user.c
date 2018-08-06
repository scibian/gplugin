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

static DynamicTest *test_object = NULL;

G_MODULE_EXPORT GPluginPluginInfo *
gplugin_query(GPLUGIN_UNUSED GError **error) {
	const gchar * const dependencies[] = {
		"gplugin/dynamic-type-provider",
		NULL
	};

	return gplugin_plugin_info_new(
		"gplugin/dynamic-type-user",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"dependencies", dependencies,
		NULL
	);
}

G_MODULE_EXPORT gboolean
gplugin_load(GPLUGIN_UNUSED GPluginNativePlugin *plugin, GError **error) {
	test_object = g_object_new(DYNAMIC_TYPE_TEST, NULL);

	if (test_object == NULL) {
		*error = g_error_new(GPLUGIN_DOMAIN, 0,
		                     "could not create an instance of DynamicTest");
		return FALSE;
	}

	return TRUE;
}

G_MODULE_EXPORT gboolean
gplugin_unload(GPLUGIN_UNUSED GPluginNativePlugin *plugin, GError **error) {
	g_object_unref(test_object);

	if (DYNAMIC_IS_TEST(test_object)) {
		*error = g_error_new(GPLUGIN_DOMAIN, 0,
		                     "test_object is still valid");
		return FALSE;
	}

	return TRUE;
}

