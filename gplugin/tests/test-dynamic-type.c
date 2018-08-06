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

#include <stdlib.h>

#include <gplugin.h>
#include <gplugin-native.h>

#include <glib.h>

#include <gplugin/gplugin-loader-tests.h>

/******************************************************************************
 * Tests
 *****************************************************************************/
static void
test_dynamic_type(void) {
	GPluginPlugin *provider = NULL, *user = NULL;
	GPluginPluginState state;
	GError *error = NULL;

	gplugin_manager_append_path(TEST_DYNAMIC_DIR);
	gplugin_manager_refresh();

	provider = gplugin_manager_find_plugin("gplugin/dynamic-type-provider");

	g_assert(provider);
	g_assert(gplugin_manager_load_plugin(provider, &error));
	g_assert_no_error(error);

	state = gplugin_plugin_get_state(provider);
	g_assert_cmpint(state, ==, GPLUGIN_PLUGIN_STATE_LOADED);

	user = gplugin_manager_find_plugin("gplugin/dynamic-type-user");

	g_assert(user);
	g_assert(gplugin_manager_load_plugin(user, &error));
	g_assert_no_error(error);

	state = gplugin_plugin_get_state(user);
	g_assert_cmpint(state, ==, GPLUGIN_PLUGIN_STATE_LOADED);

	/* now unload the plugin */
	g_assert(gplugin_manager_unload_plugin(user, &error));
	g_assert_no_error(error);

	state = gplugin_plugin_get_state(user);
	g_assert_cmpint(state, ==, GPLUGIN_PLUGIN_STATE_QUERIED);

	g_assert(gplugin_manager_unload_plugin(provider, &error));
	g_assert_no_error(error);

	state = gplugin_plugin_get_state(provider);
	g_assert_cmpint(state, ==, GPLUGIN_PLUGIN_STATE_QUERIED);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {

	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	/* test dynamic types */
	g_test_add_func("/loaders/native/dynamic-type",
	                test_dynamic_type);

	return g_test_run();
}

