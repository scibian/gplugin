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

/******************************************************************************
 * Tests
 *****************************************************************************/
/* load on query */
static void
test_load_with_dependencies(void) {
	GPluginPlugin *plugin = NULL;
	GError *error = NULL;

	gplugin_manager_remove_paths();
	gplugin_manager_append_path(TEST_VERSIONED_DEPENDENCY_DIR);
	gplugin_manager_refresh();

	plugin = gplugin_manager_find_plugin("gplugin/super-dependent");
	g_assert(plugin);
	g_assert(GPLUGIN_IS_PLUGIN(plugin));

	gplugin_manager_load_plugin(plugin, &error);
	g_assert_no_error(error);

	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	/* now make sure each dependent plugin that's available was loaded */
	plugin = gplugin_manager_find_plugin("gplugin/test-no-version");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-exact1");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-exact2");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-greater");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-greater-equal");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-less");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/test-less-equal");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/bar");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/baz");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));

	plugin = gplugin_manager_find_plugin("gplugin/fez");
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);
	g_object_unref(G_OBJECT(plugin));
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {

	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	/* test the load on query flag */
	g_test_add_func("/dependent-versions/super-dependent",
	                test_load_with_dependencies);

	return g_test_run();
}

