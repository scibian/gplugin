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

#include <gplugin-gtk/gplugin-gtk-plugin-info.h>

#include <gplugin/gplugin.h>

#define GPLUGIN_GTK_PLUGIN_INFO_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_GTK_TYPE_PLUGIN_INFO, GPluginGtkPluginInfoPrivate))

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	GPluginPlugin *plugin;
	GtkBuilder *builder;
} GPluginGtkPluginInfoPrivate;

/*****************************************************************************s
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_PLUGIN,
	N_PROPERTIES,
};

/******************************************************************************
 * Callbacks
 *****************************************************************************/
static void
_gplugin_gtk_plugin_info_expander_activate_cb(GtkExpander *expander,
                                              GPLUGIN_UNUSED gpointer data)
{
	if(gtk_expander_get_expanded(expander))
		gtk_expander_set_label(expander, "More");
	else
		gtk_expander_set_label(expander, "Less");
}

/******************************************************************************
 * Helpers
 *****************************************************************************/
static void
_gplugin_gtk_plugin_info_refresh(GPluginGtkPluginInfoPrivate *priv,
                                 GPluginPlugin *plugin)
{
	GtkWidget *authors_grid = NULL, *widget = NULL;
	GList *children = NULL, *iter = NULL;
	gchar *markup = NULL;
	gchar *name = NULL, *version = NULL, *website = NULL;
	gchar *description = NULL, *id = NULL, *abi_version = NULL;
	gchar *loader = NULL;
	gchar **authors = NULL;
	guint32 abi_version_uint;
	gboolean loq = FALSE, internal = FALSE;
	const gchar *filename = NULL;

	authors_grid = GTK_WIDGET(gtk_builder_get_object(priv->builder,
	                                                 "authors_grid"));

	/* remove all the children from the authors grid */
	children = gtk_container_get_children(GTK_CONTAINER(authors_grid));
	for(iter = children; iter; iter = iter->next)
		gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);

	/* now get the info if we can */
	if(GPLUGIN_IS_PLUGIN(plugin)) {
		GPluginPluginInfo *plugin_info = gplugin_plugin_get_info(plugin);
		GPluginLoader *plugin_loader = gplugin_plugin_get_loader(plugin);

		filename = gplugin_plugin_get_filename(plugin);

		if(plugin_loader && GPLUGIN_IS_LOADER(plugin_loader)) {
			const char *loader_name = G_OBJECT_TYPE_NAME(plugin_loader);
			loader = g_strdup(loader_name);
			g_object_unref(G_OBJECT(plugin_loader));
		}

		g_object_get(G_OBJECT(plugin_info),
		             "abi_version", &abi_version_uint,
		             "authors", &authors,
		             "description", &description,
		             "id", &id,
		             "internal", &internal,
		             "load-on-query", &loq,
		             "name", &name,
		             "version", &version,
		             "website", &website,
		             NULL);

		/* fanagle the plugin name */
		markup = g_markup_printf_escaped("<span font_size=\"large\" "
		                                 "font_weight=\"bold\">%s</span>",
		                                 (name) ? name : "Unnamed");
		g_free(name);
		name = markup;

		/* fanagle the website */
		if(website) {
			markup = g_markup_printf_escaped("<a href=\"%s\">%s</a>",
			                                 website, website);
			g_free(website);
			website = markup;
		}

		/* fanagle the abi_version */
		abi_version = g_strdup_printf("%08x", abi_version_uint);

		g_object_unref(G_OBJECT(plugin_info));
	}

	/* set the name */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "name"));
	gtk_label_set_markup(GTK_LABEL(widget), (name) ? name : "Unnamed");

	/* set the version */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "version"));
	gtk_label_set_text(GTK_LABEL(widget), (version) ? version : "");

	/* set the authors */
	if(authors) {
		gint i = 0;

		for(i = 0; authors[i]; i++) {
			widget = gtk_label_new(authors[i]);
			gtk_widget_set_halign(widget, GTK_ALIGN_START);
			gtk_widget_set_valign(widget, GTK_ALIGN_START);
			gtk_grid_attach(GTK_GRID(authors_grid), widget, 0, i, 1, 1);
			gtk_widget_show(widget);
		}
	}

	/* set the website */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "website"));
	gtk_label_set_markup(GTK_LABEL(widget), (website) ? website : "");

	/* set the description */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "description"));
	gtk_label_set_text(GTK_LABEL(widget), (description) ? description : "");

	/* set the plugin id */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "id"));
	gtk_label_set_text(GTK_LABEL(widget), (id) ? id : "");

	/* set the filename */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "filename"));
	gtk_label_set_text(GTK_LABEL(widget), (filename) ? filename : "");

	/* set the abi_version */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "abi_version"));
	gtk_label_set_text(GTK_LABEL(widget), (abi_version) ? abi_version : "");

	/* set the loader */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "loader"));
	gtk_label_set_text(GTK_LABEL(widget), (loader) ? loader : "Unknown");

	/* set the internal */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "internal"));
	gtk_label_set_text(GTK_LABEL(widget), (internal) ? "Yes" : "No");

	/* set the load on query label */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder,
	                                           "load_on_query"));
	gtk_label_set_text(GTK_LABEL(widget), (loq) ? "Yes" : "No");

	/* cleanup */
	g_strfreev(authors);
	g_free(description);
	g_free(id);
	g_free(name);
	g_free(version);
	g_free(website);
	g_free(loader);
}

/******************************************************************************
 * GObject Stuff
 *****************************************************************************/
G_DEFINE_TYPE(GPluginGtkPluginInfo, gplugin_gtk_plugin_info, GTK_TYPE_FRAME);

static void
gplugin_gtk_plugin_info_set_property(GObject *obj, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
	GPluginGtkPluginInfo *info = GPLUGIN_GTK_PLUGIN_INFO(obj);

	switch(prop_id) {
		case PROP_PLUGIN:
			gplugin_gtk_plugin_info_set_plugin(info, g_value_get_object(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
gplugin_gtk_plugin_info_get_property(GObject *obj, guint prop_id,
                                     GValue *value, GParamSpec *pspec)
{
	GPluginGtkPluginInfo *info = GPLUGIN_GTK_PLUGIN_INFO(obj);

	switch(prop_id) {
		case PROP_PLUGIN:
			g_value_set_object(value,
			                   gplugin_gtk_plugin_info_get_plugin(info));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
gplugin_gtk_plugin_info_constructed(GObject *obj) {
	GPluginGtkPluginInfoPrivate *priv = NULL;
	GtkWidget *widget = NULL;
	GError *error = NULL;
	gchar *filename = NULL;

	G_OBJECT_CLASS(gplugin_gtk_plugin_info_parent_class)->constructed(obj);

	priv = GPLUGIN_GTK_PLUGIN_INFO_GET_PRIVATE(obj);

	/* load the ui from the GtkBuilder file */
	priv->builder = gtk_builder_new();
	filename = g_build_filename(PREFIX, "share", "gplugin", "gplugin-gtk",
	                            "gplugin-gtk-plugin-info.ui", NULL);
	gtk_builder_add_from_file(priv->builder, filename, &error);
	if(error) {
		g_warning("%s", error->message);
		g_error_free(error);
	}
	g_free(filename);

	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder, "plugin_info"));
	gtk_container_add(GTK_CONTAINER(obj), widget);

	/* add a callback for the expander */
	widget = GTK_WIDGET(gtk_builder_get_object(priv->builder,
	                                           "more_expander"));
	g_signal_connect(G_OBJECT(widget), "activate",
	                 G_CALLBACK(_gplugin_gtk_plugin_info_expander_activate_cb),
	                 NULL);
}

static void
gplugin_gtk_plugin_info_init(GPLUGIN_UNUSED GPluginGtkPluginInfo *info) {
}

static void
gplugin_gtk_plugin_info_class_init(GPluginGtkPluginInfoClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	g_type_class_add_private(obj_class, sizeof(GPluginGtkPluginInfoPrivate));

	obj_class->get_property = gplugin_gtk_plugin_info_get_property;
	obj_class->set_property = gplugin_gtk_plugin_info_set_property;
	obj_class->constructed = gplugin_gtk_plugin_info_constructed;

	/* properties */
}

/******************************************************************************
 * API
 *****************************************************************************/
GtkWidget *
gplugin_gtk_plugin_info_new(void) {
	GObject *ret = NULL;

	ret = g_object_new(GPLUGIN_GTK_TYPE_PLUGIN_INFO,
	                   "shadow-type", GTK_SHADOW_NONE,
	                   NULL);

	return GTK_WIDGET(ret);
}

/**
 * gplugin_gtk_plugin_info_set_plugin:
 * @info: The #GPluginGtkPluginInfo instance
 * @plugin: The #GPluginPlugin instance
 *
 * Sets the #GPluginPlugin that should be displayed.
 *
 * A @plugin value of NULL will clear the widget.
 */
void
gplugin_gtk_plugin_info_set_plugin(GPluginGtkPluginInfo *info,
                                   GPluginPlugin *plugin)
{
	GPluginGtkPluginInfoPrivate *priv = NULL;

	g_return_if_fail(GPLUGIN_GTK_IS_PLUGIN_INFO(info));

	priv = GPLUGIN_GTK_PLUGIN_INFO_GET_PRIVATE(info);

	if(GPLUGIN_IS_PLUGIN(priv->plugin))
		g_object_unref(G_OBJECT(priv->plugin));

	if(GPLUGIN_IS_PLUGIN(plugin))
		priv->plugin = g_object_ref(G_OBJECT(plugin));
	else
		priv->plugin = NULL;

	_gplugin_gtk_plugin_info_refresh(priv, plugin);
}

/**
 * gplugin_gtk_plugin_info_get_plugin:
 * @info: The #GPluginGtkPluginInfo instance
 *
 * Returns the #GPluginPlugin that's being displayed.
 *
 * Return Value: (transfer full): The #GPluginPlugin that's being
 *  displayed.
 */
GPluginPlugin *
gplugin_gtk_plugin_info_get_plugin(const GPluginGtkPluginInfo *info) {
	GPluginGtkPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_GTK_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_GTK_PLUGIN_INFO_GET_PRIVATE(info);

	return (priv->plugin) ? g_object_ref(G_OBJECT(priv->plugin)) : NULL;
}

