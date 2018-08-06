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
#error "only <gplugin-native.h> may be included directly"
#endif
#endif /* __GI_SCANNER__ */

#ifndef GPLUGIN_NATIVE_PRIVATE_H
#define GPLUGIN_NATIVE_PRIVATE_H

#include <glib.h>

#include <gplugin/gplugin-plugin-info.h>

G_BEGIN_DECLS

typedef GPluginPluginInfo *(*GPluginNativePluginQueryFunc)(GError **error);
typedef gboolean (*GPluginNativePluginLoadFunc)(GPluginNativePlugin *plugin, GError **error);
typedef gboolean (*GPluginNativePluginUnloadFunc)(GPluginNativePlugin *plugin, GError **error);

G_END_DECLS

#endif /* GPLUGIN_PRIVATE_H */

