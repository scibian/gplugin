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

/* This test kind of sucks because theres no way for us to lookup whether or
 * not a module handle has had it's symbols bound locally.  Therefore, right
 * now we have to settle to see if it was loaded correctly.
 */
static void
test_bind_local(void) {
	GPluginPlugin *plugin = NULL;

	gplugin_manager_remove_paths();
	gplugin_manager_append_path(TEST_BIND_LOCAL_DIR);
	gplugin_manager_refresh();

	plugin = gplugin_manager_find_plugin("gplugin/bind-local");
	g_assert(plugin);
	g_assert(GPLUGIN_IS_PLUGIN(plugin));
	g_assert(GPLUGIN_IS_NATIVE_PLUGIN(plugin));
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {

	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	/* test the load on query flag */
	g_test_add_func("/loaders/native/bind-local",
	                test_bind_local);

	return g_test_run();
}

