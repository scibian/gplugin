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

#include <glib.h>

/******************************************************************************
 * Tests
 *****************************************************************************/
static void
test_gplugin_init_uninit(void) {
	gplugin_init();
	gplugin_uninit();
}

static void
test_gplugin_init_uninit_with_refresh(void) {
	gplugin_init();
	gplugin_manager_refresh();
	gplugin_uninit();
}

static void
test_gplugin_init_uninit_with_refresh_plugins(void) {
	gplugin_init();
	gplugin_manager_append_path(TEST_DIR);
	gplugin_manager_refresh();
	gplugin_uninit();
}

static void
test_gplugin_init_uninit_with_double_refresh_plugins(void) {
	GList *l = NULL;
	GSList *sl = NULL;
	gint f_ids = 0, s_ids = 0, f_plugins = 0, s_plugins = 0;

	gplugin_init();
	gplugin_manager_append_path(TEST_DIR);
	gplugin_manager_append_path(TEST_ID_DIR);

	/* run the first refresh and count everything */
	gplugin_manager_refresh();
	for(l = gplugin_manager_list_plugins(); l; l = l->next) {
		const gchar *id = (const gchar *)l->data;

		f_ids += 1;

		for(sl = gplugin_manager_find_plugins(id); sl; sl = sl->next)
			f_plugins += 1;
	}

	/* now run the second refresh and make sure the counts match */
	gplugin_manager_refresh();
	for(l = gplugin_manager_list_plugins(); l; l = l->next) {
		const gchar *id = (const gchar *)l->data;

		s_ids += 1;

		for(sl = gplugin_manager_find_plugins(id); sl; sl = sl->next)
			s_plugins += 1;
	}

	g_assert_cmpint(f_ids, >, 0);
	g_assert_cmpint(f_plugins, >, 0);

	g_assert_cmpint(f_ids, ==, s_ids);
	g_assert_cmpint(f_plugins, ==, s_plugins);

	gplugin_uninit();
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {

	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	g_test_add_func("/core/init_uninit",
	                test_gplugin_init_uninit);
	g_test_add_func("/core/init_uninit_with_refresh",
	                test_gplugin_init_uninit_with_refresh);
	g_test_add_func("/core/init_uninit_with_refresh_plugins",
	                test_gplugin_init_uninit_with_refresh_plugins);
	g_test_add_func("/core/init_uninit_with_double_refresh_plugins",
	                test_gplugin_init_uninit_with_double_refresh_plugins);

	return g_test_run();
}

