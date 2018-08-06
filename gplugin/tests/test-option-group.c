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
test_gplugin_option_group_paths(const gchar *args, GList *expected) {
	GError *error = NULL;
	GOptionContext *ctx = NULL;
	GOptionGroup *group = NULL;
	GList *paths = NULL, *l1 = NULL, *l2 = NULL;
	gchar **argv = NULL;
	gint argc = 0;

	argv = g_strsplit(args, ",", 0);
	argc = g_strv_length(argv);

	gplugin_init();

	ctx = g_option_context_new(NULL);
	group = gplugin_get_option_group();
	g_option_context_add_group(ctx, group);
	g_option_context_parse(ctx, &argc, &argv, &error);
	g_option_context_free(ctx);

	g_strfreev(argv);

	paths = gplugin_manager_get_paths();
	paths = g_list_sort(paths, g_str_equal);

	expected = g_list_sort(expected, g_str_equal);

	g_assert_cmpuint(g_list_length(paths), ==, g_list_length(expected));

	for(l1 = paths, l2 = expected; l1 && l2; l1 = l1->next, l2 = l2->next) {
		g_assert_cmpstr(l1->data, ==, l2->data);
	}

	/* now check to see if we exited early */
	g_assert(l1 == NULL || l2 == NULL);

	for(l2 = expected; l2; l2 = l2->next)
		g_free(l2->data);

	g_list_free(expected);

	gplugin_uninit();
}

static GList *
test_gplugin_manager_default_paths(void) {
	GList *paths = NULL;
	gchar *path = NULL;

	path = g_build_filename(PREFIX, LIBDIR, "gplugin", NULL);
	paths = g_list_prepend(paths, path);

	path = g_build_filename(g_get_user_config_dir(), "gplugin", NULL);
	paths = g_list_prepend(paths, path);

	return paths;
}

static void
test_gplugin_option_group_default_paths(void) {
	GList *expected = NULL;
	gchar *args = "test-option-group";

	expected = test_gplugin_manager_default_paths();

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_default_paths_with_single(void) {
	GList *expected = NULL;
	gchar *args = "test-option-group,-p,foo";

	expected = test_gplugin_manager_default_paths();
	expected = g_list_prepend(expected, g_strdup("foo"));

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_default_paths_with_multiple(void) {
	GList *expected = NULL;
	gchar *args = "test-option-group,-p,foo,-p,bar,-p,baz";

	expected = test_gplugin_manager_default_paths();
	expected = g_list_prepend(expected, g_strdup("foo"));
	expected = g_list_prepend(expected, g_strdup("bar"));
	expected = g_list_prepend(expected, g_strdup("baz"));

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_no_default_paths_short(void) {
	gchar *args = "test-option-group,-D";

	test_gplugin_option_group_paths(args, NULL);
}

static void
test_gplugin_option_group_no_default_paths_long(void) {
	gchar *args = "test-option-group,--no-defualt-paths";

	test_gplugin_option_group_paths(args, NULL);
}

static void
test_gplugin_option_group_no_default_paths_with_single(void) {
	GList *expected = NULL;
	gchar *args = "test-option-group,-D,-p,foo";

	expected = g_list_prepend(expected, g_strdup("foo"));

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_no_default_paths_with_multiple(void) {
	GList *expected = NULL;
	gchar *args = "test-option-group,-D,-p,foo,-p,bar,-p,baz";

	expected = g_list_prepend(expected, g_strdup("foo"));
	expected = g_list_prepend(expected, g_strdup("bar"));
	expected = g_list_prepend(expected, g_strdup("baz"));

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_add_path_short(void) {
	GList *expected = g_list_prepend(NULL, g_strdup("foo"));
	gchar *args = "test-option-group,-D,-p,foo";

	test_gplugin_option_group_paths(args, expected);
}

static void
test_gplugin_option_group_add_path_long(void) {
	GList *expected = g_list_prepend(NULL, g_strdup("foo"));
	gchar *args = "test-option-group,-D,--path,foo";

	test_gplugin_option_group_paths(args, expected);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/option-group/default-paths",
	                test_gplugin_option_group_default_paths);
	g_test_add_func("/option-group/default-paths/single",
	                test_gplugin_option_group_default_paths_with_single);
	g_test_add_func("/option-group/default-paths/multiple",
	                test_gplugin_option_group_default_paths_with_multiple);

	g_test_add_func("/option-group/no-default-paths/short",
	                test_gplugin_option_group_no_default_paths_short);
	g_test_add_func("/option-group/no-default-paths/long",
	                test_gplugin_option_group_no_default_paths_long);
	g_test_add_func("/option-group/no-default-paths/single",
	                test_gplugin_option_group_no_default_paths_with_single);
	g_test_add_func("/option-group/no-default-paths/multiple",
	                test_gplugin_option_group_no_default_paths_with_multiple);

	g_test_add_func("/option-group/add-path/short",
	                test_gplugin_option_group_add_path_short);
	g_test_add_func("/option-group/add-path/long",
	                test_gplugin_option_group_add_path_long);

	return g_test_run();
}

