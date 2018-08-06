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

#include <gplugin-gtk/gplugin-gtk-view.h>
#include <gplugin-gtk/gplugin-gtk-store.h>

#define GPLUGIN_GTK_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_GTK_TYPE_VIEW, GPluginGtkViewPrivate))

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	gboolean show_internal;
} GPluginGtkViewPrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_SHOW_INTERNAL,
	N_PROPERTIES,
};

/******************************************************************************
 * Globals
 *****************************************************************************/

/******************************************************************************
 * GObject Stuff
 *****************************************************************************/
G_DEFINE_TYPE(GPluginGtkView, gplugin_gtk_view, GTK_TYPE_TREE_VIEW);

static void
gplugin_gtk_view_set_property(GObject *obj, guint prop_id, const GValue *value,
                              GParamSpec *pspec)
{
	GPluginGtkView *view = GPLUGIN_GTK_VIEW(obj);

	switch(prop_id) {
		case PROP_SHOW_INTERNAL:
			gplugin_gtk_view_set_show_internal(view,
			                                   g_value_get_boolean(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
gplugin_gtk_view_get_property(GObject *obj, guint prop_id, GValue *value,
                              GParamSpec *pspec)
{
	GPluginGtkView *view = GPLUGIN_GTK_VIEW(obj);

	switch(prop_id) {
		case PROP_SHOW_INTERNAL:
			g_value_set_boolean(value,
			                    gplugin_gtk_view_get_show_internal(view));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
gplugin_gtk_view_constructed(GObject *obj) {
	G_OBJECT_CLASS(gplugin_gtk_view_parent_class)->constructed(obj);
}

static void
gplugin_gtk_view_dispose(GObject *obj) {
	G_OBJECT_CLASS(gplugin_gtk_view_parent_class)->dispose(obj);
}

static void
gplugin_gtk_view_init(GPluginGtkView *view) {
	GtkTreeViewColumn *col = NULL;
	GtkCellRenderer *rend = NULL;

	gtk_widget_set_has_tooltip(GTK_WIDGET(view), TRUE);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);

	/* create the first column */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Enabled");
	gtk_tree_view_column_set_resizable(col, FALSE);

	rend = gtk_cell_renderer_toggle_new();
	gtk_tree_view_column_pack_start(col, rend, FALSE);

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
	gtk_tree_view_column_add_attribute(col, rend, "active",
	                                   GPLUGIN_GTK_STORE_LOADED_COLUMN);

	/* create the markup column */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Name");
	gtk_tree_view_column_set_resizable(col, FALSE);

	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, rend, TRUE);

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
	gtk_tree_view_column_add_attribute(col, rend, "markup",
	                                   GPLUGIN_GTK_STORE_MARKUP_COLUMN);
}

static void
gplugin_gtk_view_class_init(GPluginGtkViewClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	g_type_class_add_private(obj_class, sizeof(GPluginGtkViewPrivate));

	obj_class->get_property = gplugin_gtk_view_get_property;
	obj_class->set_property = gplugin_gtk_view_set_property;
	obj_class->constructed = gplugin_gtk_view_constructed;
	obj_class->dispose = gplugin_gtk_view_dispose;

	/* properties */

	/**
	 * GPluginGtkView:show-internal:
	 *
	 * Whether or not to show internal plugins.
	 */
	g_object_class_install_property(obj_class, PROP_SHOW_INTERNAL,
		g_param_spec_boolean("show-internal",
		                     "show-internal",
		                     "Whether or not to show internal plugins",
		                     FALSE,
		                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

/******************************************************************************
 * API
 *****************************************************************************/
GtkWidget *
gplugin_gtk_view_new(void) {
	GObject *ret = NULL;
	GPluginGtkStore *store = gplugin_gtk_store_new();

	ret = g_object_new(GPLUGIN_GTK_TYPE_VIEW,
	                   "model", GTK_TREE_MODEL(store),
	                   NULL);

	return GTK_WIDGET(ret);
}

void
gplugin_gtk_view_set_show_internal(GPluginGtkView *view,
                                   gboolean show_internal)
{
	GPluginGtkViewPrivate *priv = NULL;

	g_return_if_fail(GPLUGIN_GTK_IS_VIEW(view));

	priv = GPLUGIN_GTK_VIEW_GET_PRIVATE(view);

	priv->show_internal = show_internal;

	g_object_notify(G_OBJECT(view), "show-internal");
}

gboolean
gplugin_gtk_view_get_show_internal(const GPluginGtkView *view) {
	GPluginGtkViewPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_GTK_IS_VIEW(view), FALSE);

	priv = GPLUGIN_GTK_VIEW_GET_PRIVATE(view);

	return priv->show_internal;
}

