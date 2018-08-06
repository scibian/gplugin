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

#include <gtk/gtk.h>

#include <gplugin/gplugin.h>
#include <gplugin-gtk/gplugin-gtk.h>

/******************************************************************************
 * Globals
 *****************************************************************************/
static gboolean show_internal = FALSE;
static gboolean add_default_paths = TRUE, version_only = FALSE;
static gchar **paths = NULL;

/******************************************************************************
 * Callbacks
 *****************************************************************************/
static gboolean
window_closed_cb(GPLUGIN_UNUSED GtkWidget *w,
                 GPLUGIN_UNUSED GdkEvent *e,
                 GPLUGIN_UNUSED gpointer d)
{
	gtk_main_quit();

	return FALSE;
}

static void
selection_changed_cb(GtkTreeSelection *sel, gpointer data) {
	GPluginGtkPluginInfo *info = GPLUGIN_GTK_PLUGIN_INFO(data);
	GPluginPlugin *plugin = NULL;
	GtkTreeModel *model = NULL;
	GtkTreeIter iter;

	if(gtk_tree_selection_get_selected(sel, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
		                   GPLUGIN_GTK_STORE_PLUGIN_COLUMN, &plugin,
		                   -1);
	}

	gplugin_gtk_plugin_info_set_plugin(info, plugin);
}

/******************************************************************************
 * Helpers
 *****************************************************************************/
static gboolean
internal_cb(GPLUGIN_UNUSED const gchar *n,
            GPLUGIN_UNUSED const gchar *v,
            GPLUGIN_UNUSED gpointer d,
            GPLUGIN_UNUSED GError **e)
{
	show_internal = TRUE;

	return TRUE;
}

static gboolean
no_default_cb(GPLUGIN_UNUSED const gchar *n,
              GPLUGIN_UNUSED const gchar *v,
              GPLUGIN_UNUSED gpointer d,
              GPLUGIN_UNUSED GError **e)
{
	add_default_paths = FALSE;

	return TRUE;
}

static gboolean
version_cb(GPLUGIN_UNUSED const gchar *n,
           GPLUGIN_UNUSED const gchar *v,
           GPLUGIN_UNUSED gpointer d,
           GPLUGIN_UNUSED GError **e)
{
	version_only = TRUE;

	printf("gplugin-gtk-viewer %s\n", GPLUGIN_VERSION);

	return TRUE;
}

static GtkWidget *
create_window(void) {
	GtkWidget *window = NULL, *grid = NULL, *sw = NULL;
	GtkWidget *view = NULL, *info = NULL;
	GtkTreeSelection *sel = NULL;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "GPlugin Viewer");
	gtk_container_set_border_width(GTK_CONTAINER(window), 12);
	g_signal_connect(G_OBJECT(window), "delete-event",
	                 G_CALLBACK(window_closed_cb), NULL);

	grid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
	gtk_container_add(GTK_CONTAINER(window), grid);

	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
	                               GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw),
	                                    GTK_SHADOW_IN);
	gtk_grid_attach(GTK_GRID(grid), sw, 0, 0, 1, 1);

	view = gplugin_gtk_view_new();
	gplugin_gtk_view_set_show_internal(GPLUGIN_GTK_VIEW(view), show_internal);
	gtk_container_add(GTK_CONTAINER(sw), view);

	info = gplugin_gtk_plugin_info_new();
	gtk_grid_attach(GTK_GRID(grid), info, 1, 0, 1, 1);

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	g_signal_connect(G_OBJECT(sel), "changed",
	                 G_CALLBACK(selection_changed_cb), info);

	return window;
}

/******************************************************************************
 * Main Stuff
 *****************************************************************************/
static GOptionEntry entries[] = {
	{
		"internal", 'i', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		internal_cb, "Show internal plugins",
		NULL,
	}, {
		"no-default-paths", 'D', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		no_default_cb, "Do not search the default plugin paths",
		NULL,
	}, {
		"path", 'p', 0, G_OPTION_ARG_STRING_ARRAY,
		&paths, "Additional paths to look for plugins",
		"PATH",
	}, {
		"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		version_cb, "Display the version and exit",
		NULL,
	}, {
		NULL, 0, 0, 0, NULL, NULL, NULL,
	},
};

gint
main(gint argc, gchar **argv) {
	GError *error = NULL;
	GOptionContext *ctx = NULL;
	GtkWidget *window = NULL;

	gtk_init(&argc, &argv);

	gplugin_init();

	ctx = g_option_context_new("");
	g_option_context_add_main_entries(ctx, entries, NULL);
	g_option_context_add_group(ctx, gtk_get_option_group(TRUE));
	g_option_context_parse(ctx, &argc, &argv, &error);
	g_option_context_free(ctx);

	if(error) {
		fprintf(stderr, "%s\n", error->message);

		g_error_free(error);

		gplugin_uninit();

		return EXIT_FAILURE;
	}

	if(version_only) {
		return 0;
	}

	if(add_default_paths)
		gplugin_manager_add_default_paths();

	if(paths) {
		gint i;

		for(i = 0; paths[i]; i++)
			gplugin_manager_prepend_path(paths[i]);
	}

	gplugin_manager_refresh();

	/* now create and show the window */
	window = create_window();
	gtk_widget_show_all(window);

	gtk_main();

	gplugin_uninit();

	return 0;
}

