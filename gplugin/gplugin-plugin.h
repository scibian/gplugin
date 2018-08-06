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

#ifndef GPLUGIN_PLUGIN_H
#define GPLUGIN_PLUGIN_H

#define GPLUGIN_TYPE_PLUGIN            (gplugin_plugin_get_type())
#define GPLUGIN_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_PLUGIN, GPluginPlugin))
#define GPLUGIN_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_TYPE_PLUGIN, GPluginPluginClass))
#define GPLUGIN_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_PLUGIN))
#define GPLUGIN_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_TYPE_PLUGIN))
#define GPLUGIN_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_PLUGIN, GPluginPluginClass))

typedef struct _GPluginPlugin          GPluginPlugin;
typedef struct _GPluginPluginClass     GPluginPluginClass;

typedef enum /*< prefix=GPLUGIN_PLUGIN_STATE,underscore_name=GPLUGIN_PLUGIN_STATE >*/ {
	GPLUGIN_PLUGIN_STATE_UNKNOWN = -1,
	GPLUGIN_PLUGIN_STATE_ERROR = 0,
	GPLUGIN_PLUGIN_STATE_QUERIED,
	GPLUGIN_PLUGIN_STATE_REQUERY,
	GPLUGIN_PLUGIN_STATE_LOADED,
	GPLUGIN_PLUGIN_STATE_LOAD_FAILED,
	GPLUGIN_PLUGIN_STATES, /*< skip >*/
} GPluginPluginState;

#include <glib.h>
#include <glib-object.h>

#include <gplugin/gplugin-plugin-info.h>
#include <gplugin/gplugin-loader.h>

struct _GPluginPlugin {
	GObject gparent;
};

struct _GPluginPluginClass {
	GObjectClass gparent;

	void (*state_changed)(GPluginPlugin *plugin, GPluginPluginState oldstate, GPluginPluginState newstate);

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

GType gplugin_plugin_get_type(void);

const gchar *gplugin_plugin_get_filename(const GPluginPlugin *plugin);
GPluginLoader *gplugin_plugin_get_loader(const GPluginPlugin *plugin);
GPluginPluginInfo *gplugin_plugin_get_info(const GPluginPlugin *plugin);

GPluginPluginState gplugin_plugin_get_state(const GPluginPlugin *plugin);
void gplugin_plugin_set_state(GPluginPlugin *plugin, GPluginPluginState state);

G_END_DECLS

#endif /* GPLUGIN_PLUGIN_H */

