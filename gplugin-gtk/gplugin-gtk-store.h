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

#ifndef GPLUGIN_GTK_STORE_H
#define GPLUGIN_GTK_STORE_H

#define GPLUGIN_GTK_TYPE_STORE            (gplugin_gtk_store_get_type())
#define GPLUGIN_GTK_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_GTK_TYPE_STORE, GPluginGtkStore))
#define GPLUGIN_GTK_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_GTK_TYPE_STORE, GPluginGtkStoreClass))
#define GPLUGIN_GTK_IS_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_GTK_TYPE_STORE))
#define GPLUGIN_GTK_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_GTK_TYPE_STORE))
#define GPLUING_GTK_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_GTK_TYPE_STORE, GPluginGtkStoreClass))

typedef struct _GPluginGtkStore           GPluginGtkStore;
typedef struct _GPluginGtkStoreClass      GPluginGtkStoreClass;

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

struct _GPluginGtkStore {
	GtkListStore parent;

	void (*_gplugin_reserved1)(void);
	void (*_gplugin_reserved2)(void);
	void (*_gplugin_reserved3)(void);
	void (*_gplugin_reserved4)(void);
};

struct _GPluginGtkStoreClass {
	GtkListStoreClass parent;

	void (*_gplugin_reserved1)(void);
	void (*_gplugin_reserved2)(void);
	void (*_gplugin_reserved3)(void);
	void (*_gplugin_reserved4)(void);
};

typedef enum {
	GPLUGIN_GTK_STORE_LOADED_COLUMN,
	GPLUGIN_GTK_STORE_PLUGIN_COLUMN,
	GPLUGIN_GTK_STORE_MARKUP_COLUMN,
	GPLUGIN_GTK_STORE_N_COLUMNS,
} GPluginGtkStoreColumns;


G_BEGIN_DECLS

GType gplugin_gtk_store_get_type(void);

GPluginGtkStore *gplugin_gtk_store_new(void);

const GType *gplugin_gtk_get_store_column_types(void);

G_END_DECLS

#endif /* GPLUGIN_GTK_STORE_H */

