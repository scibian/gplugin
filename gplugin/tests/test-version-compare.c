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

/******************************************************************************
 * Tests
 *****************************************************************************/
/* bad versions */
static void
test_gplugin_version_null__null_subprocess(void) {
	gplugin_version_compare(NULL, NULL, NULL);
}

static void
test_gplugin_version_null__null(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/version-check/null__null/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_gplugin_version_null__null_subprocess();
#endif

	g_test_trap_assert_failed();
	g_test_trap_assert_stderr("*gplugin_version_compare*assertion*");
}

static void
test_gplugin_version_null__1_2_3_subprocess(void) {
	gplugin_version_compare(NULL, "1.2.3", NULL);
}

static void
test_gplugin_version_null__1_2_3(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/version-check/null__1_2_3/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_gplugin_version_null__1_2_3_subprocess();
#endif

	g_test_trap_assert_failed();
	g_test_trap_assert_stderr("*gplugin_version_compare*assertion*");
}

static void
test_gplugin_version_1_2_3__null_subprocess(void) {
	gplugin_version_compare("1.2.3", NULL, NULL);
}

static void
test_gplugin_version_1_2_3__null(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/version-check/1_2_3__null/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_gplugin_version_1_2_3__null_subprocess();
#endif

	g_test_trap_assert_failed();
	g_test_trap_assert_stderr("*gplugin_version_compare*assertion*");
}

static void
test_gplugin_version_abc__1_2_3_subprocess(void) {
	GError *error = NULL;
	gint t = 0;

	t = gplugin_version_compare("abc", "1.2.3", &error);

	g_assert(t == 1);
	g_assert_no_error(error);
}

static void
test_gplugin_version_abc__1_2_3(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/version-check/abc__1_2_3/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_gplugin_version_abc__1_2_3_subprocess();
#endif

	g_test_trap_assert_failed();
	g_test_trap_assert_stderr("*assertion*");
}

static void
test_gplugin_version_1_2_3__abc_subprocess(void) {
	GError *error = NULL;
	gint t = 0;

	t = gplugin_version_compare("1.2.3", "abc", &error);

	g_assert(t == -1);
	g_assert_no_error(error);
}

static void
test_gplugin_version_1_2_3__abc(void) {
#if GLIB_CHECK_VERSION(2,38,0)
	g_test_trap_subprocess("/version-check/1_2_3__abc/subprocess", 0, 0);
#else
	if(g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR))
		test_gplugin_version_1_2_3__abc_subprocess();
#endif

	g_test_trap_assert_failed();
	g_test_trap_assert_stderr("*assertion*");
}

/* major version tests */
static void
test_gplugin_version_1_0_0__0_0_0(void) {
	g_assert(gplugin_version_compare("1.0.0", "0.0.0", NULL) == -1);
}

static void
test_gplugin_version_1_0_0__1_0_0(void) {
	g_assert(gplugin_version_compare("1.0.0", "1.0.0", NULL) == 0);
}

static void
test_gplugin_version_0_0_0__1_0_0(void) {
	g_assert(gplugin_version_compare("0.0.0", "1.0.0", NULL) == 1);
}

/* minor version tests */
static void
test_gplugin_version_0_1_0__0_0_0(void) {
	g_assert(gplugin_version_compare("0.1.0", "0.0.0", NULL) == -1);
}

static void
test_gplugin_version_0_1_0__0_1_0(void) {
	g_assert(gplugin_version_compare("0.1.0", "0.1.0", NULL) == 0);
}

static void
test_gplugin_version_0_0_0__0_1_0(void) {
	g_assert(gplugin_version_compare("0.0.0", "0.1.0", NULL) == 1);
}

/* micro version tests */
static void
test_gplugin_version_0_0_1__0_0_0(void) {
	g_assert(gplugin_version_compare("0.1.0", "0.0.0", NULL) == -1);
}

static void
test_gplugin_version_0_0_1__0_0_1(void) {
	g_assert(gplugin_version_compare("0.1.0", "0.1.0", NULL) == 0);
}

static void
test_gplugin_version_0_0_0__0_0_1(void) {
	g_assert(gplugin_version_compare("0.0.0", "0.1.0", NULL) == 1);
}

/* major-minor tests */
static void test_gplugin_version_1_0__0_1(void) {
	g_assert_cmpint(gplugin_version_compare("1.0", "0.1", NULL), ==, -1);
}

static void test_gplugin_version_1_0__1_0(void) {
	g_assert_cmpint(gplugin_version_compare("1.0", "1.0", NULL), ==, 0);
}

static void test_gplugin_version_0_1__1_0(void) {
	g_assert_cmpint(gplugin_version_compare("0.1", "1.0", NULL), ==, 1);
}

/* major tests */
static void test_gplugin_version_1__0(void) {
	g_assert_cmpint(gplugin_version_compare("1", "0", NULL), ==, -1);
}

static void test_gplugin_version_1__1(void) {
	g_assert_cmpint(gplugin_version_compare("1", "1", NULL), ==, 0);
}

static void test_gplugin_version_0__1(void) {
	g_assert_cmpint(gplugin_version_compare("0", "1", NULL), ==, 1);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {
	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	/* bad versions */
	g_test_add_func("/version-check/null__null",
	                test_gplugin_version_null__null);
	g_test_add_func("/version-check/null__1_2_3",
	                test_gplugin_version_null__1_2_3);
	g_test_add_func("/version-check/1_2_3__null",
	                test_gplugin_version_1_2_3__null);
	g_test_add_func("/version-check/abc__1_2_3",
	                test_gplugin_version_abc__1_2_3);
	g_test_add_func("/version-check/1_2_3__abc",
	                test_gplugin_version_1_2_3__abc);

#if GLIB_CHECK_VERSION(2,38,0)
	g_test_add_func("/version-check/null__null/subprocess",
	                test_gplugin_version_null__null_subprocess);
	g_test_add_func("/version-check/null__1_2_3/subprocess",
	                test_gplugin_version_null__1_2_3_subprocess);
	g_test_add_func("/version-check/1_2_3__null/subprocess",
	                test_gplugin_version_1_2_3__null_subprocess);
	g_test_add_func("/version-check/abc__1_2_3/subprocess",
	                test_gplugin_version_abc__1_2_3_subprocess);
	g_test_add_func("/version-check/1_2_3__abc/subprocess",
	                test_gplugin_version_1_2_3__abc_subprocess);
#endif

	/* major version */
	g_test_add_func("/version-check/1_0_0__0_0_0",
	                test_gplugin_version_1_0_0__0_0_0);
	g_test_add_func("/version-check/1_0_0__1_0_0",
	                test_gplugin_version_1_0_0__1_0_0);
	g_test_add_func("/version-check/0_0_0__1_0_0",
	                test_gplugin_version_0_0_0__1_0_0);

	/* minor version */
	g_test_add_func("/version-check/0_1_0__0_0_0",
	                test_gplugin_version_0_1_0__0_0_0);
	g_test_add_func("/version-check/0_1_0__0_1_0",
	                test_gplugin_version_0_1_0__0_1_0);
	g_test_add_func("/version-check/0_0_0__0_1_0",
	                test_gplugin_version_0_0_0__0_1_0);

	/* micro version */
	g_test_add_func("/version-check/0_0_1__0_0_0",
	                test_gplugin_version_0_0_1__0_0_0);
	g_test_add_func("/version-check/0_0_1__0_0_1",
	                test_gplugin_version_0_0_1__0_0_1);
	g_test_add_func("/version-check/0_0_0__0_0_1",
	                test_gplugin_version_0_0_0__0_0_1);

	/* major-minor */
	g_test_add_func("/version-check/1_0__0_1",
	                test_gplugin_version_1_0__0_1);
	g_test_add_func("/version-check/1_0__1_0",
	                test_gplugin_version_1_0__1_0);
	g_test_add_func("/version-check/0_1__1_0",
	                test_gplugin_version_0_1__1_0);

	/* major */
	g_test_add_func("/version-check/1__0",
	                test_gplugin_version_1__0);
	g_test_add_func("/version-check/1__1",
	                test_gplugin_version_1__1);
	g_test_add_func("/version-check/0__1",
	                test_gplugin_version_0__1);

	return g_test_run();
}

