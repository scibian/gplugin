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
#include <gplugin-native.h>

#include <glib.h>

#define test_string(var, value) G_STMT_START { \
	g_assert_cmpstr((var), ==, (value)); \
	g_assert_cmpstr((var), ==, gplugin_plugin_info_get_##var(info)); \
	g_free((var)); \
} G_STMT_END

#define test_uint(var, value) G_STMT_START { \
	g_assert_cmpuint((var), ==, (value)); \
	g_assert_cmpuint((var), ==, gplugin_plugin_info_get_##var(info)); \
} G_STMT_END

#define test_bool(var, value) G_STMT_START { \
	g_assert((var) == (value)); \
	g_assert((var) == gplugin_plugin_info_get_##var(info)); \
} G_STMT_END

typedef gchar **(*TestStringVFunc)(GPluginPluginInfo *info);

static void
test_stringv(gchar **got, const gchar * const * const expected,
             TestStringVFunc func, GPluginPluginInfo *info)
{
        gint i = 0;
        gchar **tmp = NULL;

        for(i = 0; got[i]; i++)
                g_assert_cmpstr(got[i], ==, expected[i]);

        tmp = func(info);

        for(i = 0; got[i]; i++)
                g_assert_cmpstr(got[i], ==, tmp[i]);

        g_strfreev(tmp);
}

/******************************************************************************
 * Tests
 *****************************************************************************/
static void
test_gplugin_plugin_info_construction(void) {
	GPluginPluginInfo *info = NULL;
	gchar *id = NULL, *name = NULL, *version = NULL;
	gchar *license_id = NULL, *license_text = NULL, *license_url = NULL;
	gchar *icon = NULL, *summary = NULL, *description = NULL, *category = NULL;
	gchar *help = NULL, *website = NULL;
	gchar **authors = NULL, **dependencies = NULL;
	guint abi_version = 0;
	gboolean internal = FALSE, load_on_query = FALSE;
	const gchar * const r_authors[] = { "author", NULL };
	const gchar * const r_dependencies[] = { "dependency", NULL };

	info = g_object_new(GPLUGIN_TYPE_PLUGIN_INFO,
		"id", "gplugin-test/plugin-info-test",
		"abi_version", GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"internal", TRUE,
		"load-on-query", TRUE,
		"name", "name",
		"version", "version",
		"license-id", "license-id",
		"license-text", "license-text",
		"license-url", "license-url",
		"icon", "icon",
		"summary", "summary",
		"description", "description",
		"category", "category",
		"authors", r_authors,
		"help", "help",
		"website", "website",
		"dependencies", r_dependencies,
		NULL
	);

	g_assert(GPLUGIN_IS_PLUGIN_INFO(info));

	g_object_get(G_OBJECT(info),
		"id", &id,
		"abi_version", &abi_version,
		"internal", &internal,
		"load-on-query", &load_on_query,
		"name", &name,
		"version", &version,
		"license-id", &license_id,
		"license-text", &license_text,
		"license-url", &license_url,
		"icon", &icon,
		"summary", &summary,
		"description", &description,
		"category", &category,
		"authors", &authors,
		"help", &help,
		"website", &website,
		"dependencies", &dependencies,
		NULL
	);

	test_string(id, "gplugin-test/plugin-info-test");
	test_uint(abi_version, GPLUGIN_NATIVE_PLUGIN_ABI_VERSION);
	test_bool(internal, TRUE);
	test_bool(load_on_query, TRUE);
	test_string(name, "name");
	test_string(version, "version");
	test_string(license_id, "license-id");
	test_string(license_text, "license-text");
	test_string(license_url, "license-url");
	test_string(icon, "icon");
	test_string(summary, "summary");
	test_string(description, "description");
	test_string(category, "category");
	test_stringv(authors, r_authors,
	             (TestStringVFunc)gplugin_plugin_info_get_authors, info);
	test_string(help, "help");
	test_string(website, "website");
	test_stringv(dependencies, r_dependencies,
	             (TestStringVFunc)gplugin_plugin_info_get_dependencies, info);
}

static void
test_gplugin_plugin_info_new_empty(void) {
	GPluginPluginInfo *info = NULL;
	gchar *id = NULL;
	guint32 abi_version = 0;

	info = gplugin_plugin_info_new("empty", 1, NULL);

	g_assert(GPLUGIN_IS_PLUGIN_INFO(info));

	g_object_get(G_OBJECT(info),
	             "id", &id,
	             "abi-version", &abi_version,
	             NULL);

	test_string(id, "empty");
	test_uint(abi_version, 1);

	g_object_unref(G_OBJECT(info));
}

static void
test_gplugin_plugin_info_new_full(void) {
	GPluginPluginInfo *info = NULL;
	gchar *id = NULL, *name = NULL, *version = NULL;
	gchar *license_id = NULL, *license_text = NULL, *license_url = NULL;
	gchar *icon = NULL, *summary = NULL, *description = NULL, *category = NULL;
	gchar *website = NULL, *help = NULL;
	gchar **authors = NULL, **dependencies = NULL;
	guint abi_version = 0;
	gboolean internal = FALSE, load_on_query = FALSE;
	const gchar * const r_authors[] = { "author", NULL };
	const gchar * const r_dependencies[] = { "dependency", NULL };

	info = gplugin_plugin_info_new(
		"gplugin-test/plugin-info-test",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"internal", TRUE,
		"load-on-query", TRUE,
		"name", "name",
		"version", "version",
		"license-id", "license-id",
		"license-text", "license-text",
		"license-url", "license-url",
		"icon", "icon",
		"summary", "summary",
		"description", "description",
		"category", "category",
		"authors", r_authors,
		"help", "help",
		"website", "website",
		"dependencies", r_dependencies,
		NULL
	);

	g_assert(GPLUGIN_IS_PLUGIN_INFO(info));

	g_object_get(G_OBJECT(info),
		"id", &id,
		"abi_version", &abi_version,
		"internal", &internal,
		"load-on-query", &load_on_query,
		"name", &name,
		"version", &version,
		"license-id", &license_id,
		"license-text", &license_text,
		"license-url", &license_url,
		"icon", &icon,
		"summary", &summary,
		"description", &description,
		"category", &category,
		"authors", &authors,
		"help", &help,
		"website", &website,
		"dependencies", &dependencies,
		NULL
	);

	test_string(id, "gplugin-test/plugin-info-test");
	test_uint(abi_version, GPLUGIN_NATIVE_PLUGIN_ABI_VERSION);
	test_bool(internal, TRUE);
	test_bool(load_on_query, TRUE);
	test_string(name, "name");
	test_string(version, "version");
	test_string(license_id, "license-id");
	test_string(license_text, "license-text");
	test_string(license_url, "license-url");
	test_string(icon, "icon");
	test_string(summary, "summary");
	test_string(description, "description");
	test_string(category, "category");
	test_stringv(authors, r_authors,
	             (TestStringVFunc)gplugin_plugin_info_get_authors, info);
	test_string(help, "help");
	test_string(website, "website");
	test_stringv(dependencies, r_dependencies,
	             (TestStringVFunc)gplugin_plugin_info_get_dependencies, info);
}

static void
test_gplugin_plugin_info_authors_single(void) {
	GPluginPluginInfo *info = NULL;
	const gchar * const authors[] = { "author", NULL };
	const gchar * const *g_authors = NULL;
	gint i;

	info = gplugin_plugin_info_new(
		"test/single-author",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"authors", authors,
		NULL);

	g_authors = gplugin_plugin_info_get_authors(info);

	for(i = 0; authors[i]; i++)
		g_assert_cmpstr(authors[i], ==, g_authors[i]);

	g_object_unref(G_OBJECT(info));
}

static void
test_gplugin_plugin_info_authors_multiple(void) {
	GPluginPluginInfo *info = NULL;
	const gchar * const authors[] = { "author1", "author2", NULL };
	const gchar * const *g_authors = NULL;
	gint i;

	info = gplugin_plugin_info_new(
		"test/single-author",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"authors", authors,
		NULL);

	g_authors = gplugin_plugin_info_get_authors(info);

	for(i = 0; authors[i]; i++)
		g_assert_cmpstr(authors[i], ==, g_authors[i]);

	g_object_unref(G_OBJECT(info));
}


static void
test_gplugin_plugin_info_dependencies_single(void) {
	GPluginPluginInfo *info = NULL;
	gchar *dependencies[] = { "dependency1", NULL };
	const gchar * const *g_dependencies = NULL;
	gint i;

	info = gplugin_plugin_info_new(
		"test/single-dependency",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"dependencies", dependencies,
		NULL);

	g_dependencies = gplugin_plugin_info_get_dependencies(info);

	for(i = 0; dependencies[i]; i++)
		g_assert_cmpstr(dependencies[i], ==, g_dependencies[i]);

	g_object_unref(G_OBJECT(info));
}

static void
test_gplugin_plugin_info_dependencies_multiple(void) {
	GPluginPluginInfo *info = NULL;
	gchar *dependencies[] = { "dependencie1", "dependencie2", NULL };
	const gchar * const *g_dependencies = NULL;
	gint i;


	info = gplugin_plugin_info_new(
		"test/single-dependencie",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"dependencies", dependencies,
		NULL);

	g_dependencies = gplugin_plugin_info_get_dependencies(info);

	for(i = 0; dependencies[i]; i++)
		g_assert_cmpstr(dependencies[i], ==, g_dependencies[i]);

	g_object_unref(G_OBJECT(info));
}

/******************************************************************************
 * version_func tests
 *****************************************************************************/
static void
test_gplugin_plugin_info_version_func(GPluginVersionCompareFunc func) {
	GPluginPluginInfo *info  = NULL;
	GPluginVersionCompareFunc got = NULL;

	info = gplugin_plugin_info_new(
		"test/version-func",
		GPLUGIN_NATIVE_PLUGIN_ABI_VERSION,
		"version-func", func,
		NULL);

	got = gplugin_plugin_info_get_version_func(info);
	g_assert(func == got);
}

static gint
test_gplugin_version_compare(GPLUGIN_UNUSED const gchar *v1,
                             GPLUGIN_UNUSED const gchar *v2) {
	return 0;
}

static void
test_gplugin_plugin_info_version_func_null(void) {
	test_gplugin_plugin_info_version_func(NULL);
}

static void
test_gplugin_plugin_info_version_func_default(void) {
	test_gplugin_plugin_info_version_func(
		(GPluginVersionCompareFunc)gplugin_version_compare
	);
}

static void
test_gplugin_plugin_info_version_func_custom(void) {
	test_gplugin_plugin_info_version_func(
		(GPluginVersionCompareFunc)test_gplugin_version_compare
	);
}

/******************************************************************************
 * Main
 *****************************************************************************/
gint
main(gint argc, gchar **argv) {
	g_test_init(&argc, &argv, NULL);

	gplugin_init();

	g_test_add_func("/plugin-info/construction",
	                test_gplugin_plugin_info_construction);

	g_test_add_func("/plugin-info/new/empty",
	                test_gplugin_plugin_info_new_empty);
	g_test_add_func("/plugin-info/new/full",
	                test_gplugin_plugin_info_new_full);

	g_test_add_func("/plugin-info/authors/single",
	                test_gplugin_plugin_info_authors_single);
	g_test_add_func("/plugin-info/authors/multiple",
	                test_gplugin_plugin_info_authors_multiple);

	g_test_add_func("/plugin-info/dependencies/single",
	                test_gplugin_plugin_info_dependencies_single);
	g_test_add_func("/plugin-info/dependencies/multiple",
	                test_gplugin_plugin_info_dependencies_multiple);

	g_test_add_func("/plugin-info/version-func/null",
	                test_gplugin_plugin_info_version_func_null);
	g_test_add_func("/plugin-info/version-func/default",
	                test_gplugin_plugin_info_version_func_default);
	g_test_add_func("/plugin-info/version-func/custom",
	                test_gplugin_plugin_info_version_func_custom);

	return g_test_run();
}

