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
#error "only <gplugin.h> or <gplugin-native.h> may be included directly"
#endif
#endif /* __GI_SCANNER__ */

#ifndef GPLUGIN_NATIVE_LOADER_H
#define GPLUGIN_NATIVE_LOADER_H

#define GPLUGIN_TYPE_NATIVE_LOADER            (gplugin_native_loader_get_type())
#define GPLUGIN_NATIVE_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_NATIVE_LOADER, GPluginNativeLoader))
#define GPLUGIN_NATIVE_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((obj), GPLUGIN_TYPE_NATIVE_LOADER, GPluginNativeLoaderClass))
#define GPLUGIN_IS_NATIVE_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_NATIVE_LOADER))
#define GPLUGIN_IS_NATIVE_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GPLUGIN_TYPE_NATIVE_LOADER))
#define GPLUGIN_NATIVE_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_NATIVE_LOADER, GPluginNativeLoaderClass))

typedef struct _GPluginNativeLoader            GPluginNativeLoader;
typedef struct _GPluginNativeLoaderClass       GPluginNativeLoaderClass;

#include <glib.h>
#include <glib-object.h>

#include <gplugin/gplugin-loader.h>

#define GPLUGIN_NATIVE_PLUGIN_ABI_VERSION 0x01000000

struct _GPluginNativeLoader {
	GPluginLoader parent;
};

struct _GPluginNativeLoaderClass {
	GPluginLoaderClass parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

GType gplugin_native_loader_get_type(void);

G_END_DECLS

#endif /* GPLUGIN_NATIVE_LOADER_H */

