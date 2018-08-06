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
#if !defined(GPLUGIN_GLOBAL_HEADER_INSIDE) && !defined(GPLUGIN_COMPILATION)
#error "only <gplugin.h> may be included directly"
#endif
#endif /* __GI_SCANNER__ */

#ifndef GPLUGIN_LOADER_H
#define GPLUGIN_LOADER_H

#define GPLUGIN_TYPE_LOADER            (gplugin_loader_get_type())
#define GPLUGIN_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_LOADER, GPluginLoader))
#define GPLUGIN_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_TYPE_LOADER, GPluginLoaderClass))
#define GPLUGIN_IS_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_LOADER))
#define GPLUGIN_IS_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_TYPE_LOADER))
#define GPLUGIN_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_LOADER, GPluginLoaderClass))

typedef struct _GPluginLoader          GPluginLoader;
typedef struct _GPluginLoaderClass     GPluginLoaderClass;

#include <glib.h>
#include <glib-object.h>

#include <gplugin/gplugin-plugin.h>

struct _GPluginLoader {
	GObject gparent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

struct _GPluginLoaderClass {
	GObjectClass gparent;

	GSList *(*supported_extensions)(const GPluginLoaderClass *klass);

	GPluginPlugin *(*query)(GPluginLoader *loader, const gchar *filename, GError **error);

	gboolean (*load)(GPluginLoader *loader, GPluginPlugin *plugin, GError **error);
	gboolean (*unload)(GPluginLoader *loader, GPluginPlugin *plugin, GError **error);

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

GType gplugin_loader_get_type(void);

GSList *gplugin_loader_class_get_supported_extensions(const GPluginLoaderClass *klass);

GPluginPlugin *gplugin_loader_query_plugin(GPluginLoader *loader, const gchar *filename, GError **error);

gboolean gplugin_loader_load_plugin(GPluginLoader *loader, GPluginPlugin *plugin, GError **error);
gboolean gplugin_loader_unload_plugin(GPluginLoader *loader, GPluginPlugin *plugin, GError **error);

G_END_DECLS

#endif /* GPLUGIN_LOADER_H */

