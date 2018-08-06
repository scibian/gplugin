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

#ifndef GPLUGIN_PERL_PLUGIN_LOADER_H
#define GPLUGIN_PERL_PLUGIN_LOADER_H

#define GPLUGIN_TYPE_PERL_PLUGIN_LOADER            (gplugin_perl_plugin_loader_get_type())
#define GPLUGIN_PERL_PLUGIN_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_PERL_PLUGIN_LOADER, GPluginPerlLoader))
#define GPLUGIN_PERL_PLUGIN_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((obj), GPLUGIN_TYPE_PERL_PLUGIN_LOADER, GPluginPerlLoaderClass))
#define GPLUGIN_IS_PERL_PLUGIN_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_PERL_PLUGIN_LOADER))
#define GPLUGIN_IS_PERL_PLUGIN_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GPLUGIN_TYPE_PERL_PLUGIN_LOADER))
#define GPLUGIN_PERL_PLUGIN_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_PERL_PLUGIN_LOADER, GPluginPerlLoaderClass))

typedef struct _GPluginPerlLoader            GPluginPerlLoader;
typedef struct _GPluginPerlLoaderClass       GPluginPerlLoaderClass;

#include <gplugin.h>
#include <gplugin-native.h>

struct _GPluginPerlLoader {
	GPluginLoader parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

struct _GPluginPerlLoaderClass {
	GPluginLoaderClass parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

void gplugin_perl_plugin_loader_register(GPluginNativePlugin *plugin);
GType gplugin_perl_plugin_loader_get_type(void);

G_END_DECLS

#endif /* GPLUGIN_PERL_PLUGIN_LOADER_H */

