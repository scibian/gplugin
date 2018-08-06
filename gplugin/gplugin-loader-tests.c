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

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gi18n.h>

#include <gplugin.h>

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	gchar *path;
	GTestDataFunc func;
} GPluginTestLoaderFunction;

/******************************************************************************
 * Tests
 *****************************************************************************/
static void
gplugin_test_loader_full(gconstpointer d) {
	GPluginPlugin *plugin = NULL;
	GPluginPluginInfo *info = NULL;
	GError *error = NULL;
	gchar *id = NULL;
	gchar **authors = NULL;
	const gchar * const r_authors[] = { "author1", NULL };
	gint i;

	id = g_strdup_printf("gplugin/%s-basic-plugin", (const gchar *)d);
	plugin = gplugin_manager_find_plugin(id);
	g_assert(plugin != NULL);

	info = gplugin_plugin_get_info(plugin);
	g_assert(info != NULL);

	g_assert_cmpstr(gplugin_plugin_info_get_id(info), ==, id);
	g_free(id);

	g_assert_cmpuint(gplugin_plugin_info_get_abi_version(info), ==,
	                 0x01020304);
	g_assert_cmpstr(gplugin_plugin_info_get_name(info), ==, "basic plugin");

	authors = (gchar **)gplugin_plugin_info_get_authors(info);
	for(i = 0; r_authors[i]; i++)
		g_assert_cmpstr(authors[i], ==, r_authors[i]);

	g_assert_cmpstr(gplugin_plugin_info_get_category(info), ==, "test");
	g_assert_cmpstr(gplugin_plugin_info_get_version(info), ==, "version");
	g_assert_cmpstr(gplugin_plugin_info_get_license_id(info), ==, "license");
	g_assert_cmpstr(gplugin_plugin_info_get_summary(info), ==, "summary");
	g_assert_cmpstr(gplugin_plugin_info_get_description(info), ==,
	                "description");
	g_assert_cmpstr(gplugin_plugin_info_get_website(info), ==, "website");

	g_object_unref(G_OBJECT(info));

	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_QUERIED);

	gplugin_manager_load_plugin(plugin, &error);
	g_assert_no_error(error);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);

	gplugin_manager_unload_plugin(plugin, &error);
	g_assert_no_error(error);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_QUERIED);

	g_object_unref(G_OBJECT(plugin));
}

static void
gplugin_test_loader_load_failed(gconstpointer d) {
	GPluginPlugin *plugin = NULL;
	GError *error = NULL;
	gchar *id = NULL;
	gboolean ret = FALSE;

	id = g_strdup_printf("gplugin/%s-load-failed", (const gchar *)d);
	plugin = gplugin_manager_find_plugin(id);
	g_free(id);
	g_assert(plugin != NULL);

	ret = gplugin_manager_load_plugin(plugin, &error);
	g_assert(ret == FALSE);
	g_assert_error(error, GPLUGIN_DOMAIN, 0);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

	g_object_unref(G_OBJECT(plugin));
}

static void
gplugin_test_loader_load_exception(gconstpointer d) {
	GPluginPlugin *plugin = NULL;
	GError *error = NULL;
	gchar *id = NULL;
	gboolean ret = FALSE;

	id = g_strdup_printf("gplugin/%s-load-exception", (const gchar *)d);
	plugin = gplugin_manager_find_plugin(id);
	g_free(id);
	g_assert(plugin != NULL);

	ret = gplugin_manager_load_plugin(plugin, &error);
	g_assert(ret == FALSE);
	g_assert_error(error, GPLUGIN_DOMAIN, 0);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

	g_object_unref(G_OBJECT(plugin));
}

static void
gplugin_test_loader_unload_failed(gconstpointer d) {
	GPluginPlugin *plugin = NULL;
	GError *error = NULL;
	gchar *id = NULL;
	gboolean ret = FALSE;

	id = g_strdup_printf("gplugin/%s-unload-failed", (const gchar *)d);
	plugin = gplugin_manager_find_plugin(id);
	g_free(id);
	g_assert(plugin != NULL);

	ret = gplugin_manager_load_plugin(plugin, &error);
	g_assert(ret != FALSE);
	g_assert_no_error(error);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);

	ret = gplugin_manager_unload_plugin(plugin, &error);
	g_assert(ret != TRUE);
	g_assert_error(error, GPLUGIN_DOMAIN, 0);
	g_assert_cmpint(gplugin_plugin_get_state(plugin), ==,
	                GPLUGIN_PLUGIN_STATE_LOADED);

	g_object_unref(G_OBJECT(plugin));
}

static void
gplugin_test_loader_dependencies(gconstpointer d) {
	GPluginPlugin *plugin = NULL;
	GPluginPluginInfo *info = NULL;
	gchar *id = NULL;
	gchar **deps = NULL;
	const gchar * const r_deps[] = { "dependency1", "dependency2", NULL };
	gint i = 0;

	id = g_strdup_printf("gplugin/%s-dependent-plugin", (const gchar *)d);
	plugin = gplugin_manager_find_plugin(id);
	g_free(id);
	g_assert(plugin != NULL);

	info = gplugin_plugin_get_info(plugin);
	g_assert(info != NULL);

	deps = (gchar **)gplugin_plugin_info_get_dependencies(info);
	for(i = 0; r_deps[i]; i++)
		g_assert_cmpstr(r_deps[i], ==, deps[i]);

	g_object_unref(G_OBJECT(plugin));
}

/******************************************************************************
 * Main
 *****************************************************************************/
static GPluginTestLoaderFunction test_functions[] = {
	{ "/loaders/%s/full", gplugin_test_loader_full },
	{ "/loaders/%s/load-failed", gplugin_test_loader_load_failed },
	{ "/loaders/%s/load-exception", gplugin_test_loader_load_exception },
	{ "/loaders/%s/unload-failed", gplugin_test_loader_unload_failed },
	{ "/loaders/%s/dependencies", gplugin_test_loader_dependencies },
	{ NULL, NULL },
};

void
gplugin_loader_tests_add_tests(const gchar *short_name) {
	gint i = 0;

	for(i = 0; test_functions[i].path; i++) {
		gchar *path = g_strdup_printf(test_functions[i].path, short_name);

		g_test_add_data_func(path, short_name, test_functions[i].func);

		g_free(path);
	}
}

void
gplugin_loader_tests_main(const gchar *loader_dir, const gchar *plugin_dir,
                          const gchar *short_name)
{
	gplugin_init();

	g_setenv("GI_TYPELIB_PATH", GI_TYPELIB_PATH, TRUE);

	if(loader_dir)
		gplugin_manager_append_path(loader_dir);

	if(plugin_dir)
		gplugin_manager_append_path(plugin_dir);

	gplugin_manager_refresh();

	gplugin_loader_tests_add_tests(short_name);
}

