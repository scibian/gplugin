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

#ifndef __GI_SCANNER__ /* hide this bit from g-ir-scanner */
#if !defined(GPLUGIN_GTK_GLOBAL_HEADER_INSIDE) && !defined(GPLUGIN_GTK_COMPILATION)
#error "only <gplugin/gplugin-gtk.h> may be included directly"
#endif
#endif /* __GI_SCANNER__ */

#ifndef GPLUGIN_GTK_VIEW_H
#define GPLUGIN_GTK_VIEW_H

#define GPLUGIN_GTK_TYPE_VIEW            (gplugin_gtk_view_get_type())
#define GPLUGIN_GTK_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_GTK_TYPE_VIEW, GPluginGtkView))
#define GPLUGIN_GTK_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_GTK_TYPE_VIEW, GPluginGtkViewClass))
#define GPLUGIN_GTK_IS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_GTK_TYPE_VIEW))
#define GPLUGIN_GTK_IS_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_GTK_TYPE_VIEW))
#define GPLUING_GTK_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_GTK_TYPE_VIEW, GPluginGtkViewClass))

typedef struct _GPluginGtkView           GPluginGtkView;
typedef struct _GPluginGtkViewClass      GPluginGtkViewClass;

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

struct _GPluginGtkView {
	GtkTreeView parent;

	void (*_gplugin_reserved1)(void);
	void (*_gplugin_reserved2)(void);
	void (*_gplugin_reserved3)(void);
	void (*_gplugin_reserved4)(void);
};

struct _GPluginGtkViewClass {
	GtkTreeViewClass parent;

	void (*_gplugin_reserved1)(void);
	void (*_gplugin_reserved2)(void);
	void (*_gplugin_reserved3)(void);
	void (*_gplugin_reserved4)(void);
};

G_BEGIN_DECLS

GType gplugin_gtk_view_get_type(void);

GtkWidget *gplugin_gtk_view_new(void);

void gplugin_gtk_view_set_show_internal(GPluginGtkView *view, gboolean show_internal);
gboolean gplugin_gtk_view_get_show_internal(const GPluginGtkView *view);

G_END_DECLS

#endif /* GPLUGIN_GTK_VIEW_H */

