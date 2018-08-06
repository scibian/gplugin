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

#include <gplugin/gplugin.h>

#include <gplugin-gtk/gplugin-gtk-store.h>

#define GPLUGIN_GTK_STORE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_GTK_TYPE_STORE, GPluginGtkStorePrivate))

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	gboolean dummy;
} GPluginGtkStorePrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/

/******************************************************************************
 * Globals
 *****************************************************************************/
static const GType column_types[] = {
	G_TYPE_BOOLEAN,
	G_TYPE_OBJECT,
	G_TYPE_STRING,
};

G_STATIC_ASSERT(G_N_ELEMENTS(column_types) == GPLUGIN_GTK_STORE_N_COLUMNS);

/******************************************************************************
 * Helpers
 *****************************************************************************/
static void
gplugin_gtk_store_add_plugin(GPluginGtkStore *store, GPluginPlugin *plugin) {
	GtkTreeIter iter;
	GPluginPluginInfo *info = gplugin_plugin_get_info(plugin);
	GString *str = g_string_new("");
	gchar *name = NULL, *summary = NULL;

	g_object_get(G_OBJECT(info),
	             "name", &name,
	             "summary", &summary,
	             NULL);

	g_string_append_printf(str, "<b>%s</b>\n",
	                       (name) ? name : "<i>Unnamed</i>");
	g_string_append_printf(str, "%s",
	                       (summary) ? summary : "<i>No Summary</i>");

	g_free(name);
	g_free(summary);

	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store), &iter,
	                   GPLUGIN_GTK_STORE_LOADED_COLUMN, FALSE,
	                   GPLUGIN_GTK_STORE_PLUGIN_COLUMN, g_object_ref(plugin),
	                   GPLUGIN_GTK_STORE_MARKUP_COLUMN, str->str,
	                   -1);

	g_string_free(str, TRUE);
	g_object_unref(G_OBJECT(info));
}

static void
gplugin_gtk_store_add_plugin_by_id(GPluginGtkStore *store, const gchar * id) {
	GSList *plugins = NULL, *l = NULL;

	plugins = gplugin_manager_find_plugins(id);
	for(l = plugins; l; l = l->next)
		gplugin_gtk_store_add_plugin(store, GPLUGIN_PLUGIN(l->data));
	gplugin_manager_free_plugin_list(plugins);
}

/******************************************************************************
 * GObject Stuff
 *****************************************************************************/
G_DEFINE_TYPE(GPluginGtkStore, gplugin_gtk_store, GTK_TYPE_LIST_STORE);

static void
gplugin_gtk_store_constructed(GObject *obj) {
	GList *l, *ids = NULL;

	G_OBJECT_CLASS(gplugin_gtk_store_parent_class)->constructed(obj);

	ids = gplugin_manager_list_plugins();
	for(l = ids; l; l = l->next)
		gplugin_gtk_store_add_plugin_by_id(GPLUGIN_GTK_STORE(obj),
		                                   (const gchar *)l->data);
	g_list_free(ids);
}

static void
gplugin_gtk_store_dispose(GObject *obj) {
	G_OBJECT_CLASS(gplugin_gtk_store_parent_class)->dispose(obj);
}

static void
gplugin_gtk_store_init(GPluginGtkStore *store) {
	GType *types = (GType *)gplugin_gtk_get_store_column_types();

	gtk_list_store_set_column_types(GTK_LIST_STORE(store),
	                                GPLUGIN_GTK_STORE_N_COLUMNS,
	                                types);
}

static void
gplugin_gtk_store_class_init(GPluginGtkStoreClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	g_type_class_add_private(obj_class, sizeof(GPluginGtkStorePrivate));

	obj_class->constructed = gplugin_gtk_store_constructed;
	obj_class->dispose = gplugin_gtk_store_dispose;

}

/******************************************************************************
 * API
 *****************************************************************************/
GPluginGtkStore *
gplugin_gtk_store_new(void) {
	return GPLUGIN_GTK_STORE(g_object_new(GPLUGIN_GTK_TYPE_STORE, NULL));
}

const GType *
gplugin_gtk_get_store_column_types(void) {
	return column_types;
}

