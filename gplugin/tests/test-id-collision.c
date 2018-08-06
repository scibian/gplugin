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
test_id_collision(void) {
	GSList *plugins = NULL;

	gplugin_manager_append_path(TEST_ID_DIR);
	gplugin_manager_refresh();

	plugins = gplugin_manager_find_plugins("gplugin/id-collision");
	g_assert(plugins);

	g_assert(g_slist_length(plugins) == 2);

	gplugin_manager_free_plugin_list(plugins);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {

	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	g_test_add_func("/loaders/native/id-collision",
	                test_id_collision);

	return g_test_run();
}

