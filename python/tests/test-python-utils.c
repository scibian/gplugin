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

#include <Python.h>

#include "gplugin-python-utils.h"

#include <glib.h>

/******************************************************************************
 * filename to module tests
 *****************************************************************************/
static void
test_filename_to_module_NULL_subprocess(void) {
	gplugin_python_filename_to_module(NULL);
}

static void
test_filename_to_module_NULL(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/loaders/python/utils/filename_to_module/NULL/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_filename_to_module_NULL_subprocess();
#endif

	g_test_trap_assert_failed();
}

static void
test_filename_to_module_empty(void) {
	gchar *module = gplugin_python_filename_to_module("");

	g_assert_cmpstr(module, ==, "");

	g_free(module);
}

static void
test_filename_to_module_no_extension(void) {
	gchar *module = gplugin_python_filename_to_module("foo");

	g_assert_cmpstr(module, ==, "foo");

	g_free(module);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {
	g_test_init(&argc, &argv, NULL);

	/* tests */
	g_test_add_func("/loaders/python/utils/filename_to_module/NULL",
	                test_filename_to_module_NULL);
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_add_func("/loaders/python/utils/filename_to_module/NULL/subprocess",
	                test_filename_to_module_NULL_subprocess);
#endif
	g_test_add_func("/loaders/python/utils/filename_to_module/empty",
	                test_filename_to_module_empty);
	g_test_add_func("/loaders/python/utils/filename_to_module/no-extension",
	                test_filename_to_module_no_extension);

	return g_test_run();
}

