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

#ifndef GPLUGIN_PERL_PLUGIN_H
#define GPLUGIN_PERL_PLUGIN_H

#define GPLUGIN_TYPE_PERL_PLUGIN            (gplugin_perl_plugin_get_type())
#define GPLUGIN_PERL_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_PERL_PLUGIN, GPluginPerlPlugin))
#define GPLUGIN_PERL_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((obj), GPLUGIN_TYPE_PERL_PLUGIN, GPluginPerlPluginClass))
#define GPLUGIN_IS_PERL_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_PERL_PLUGIN))
#define GPLUGIN_IS_PERL_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GPLUGIN_TYPE_PERL_PLUGIN))
#define GPLUGIN_PERL_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_PERL_PLUGIN, GPluginPerlPluginClass))

typedef struct _GPluginPerlPlugin            GPluginPerlPlugin;
typedef struct _GPluginPerlPluginClass       GPluginPerlPluginClass;

#include <gplugin.h>
#include <gplugin-native.h>

#include <EXTERN.h>
#include <perl.h>
#undef _

struct _GPluginPerlPlugin {
	GPluginPlugin parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

struct _GPluginPerlPluginClass {
	GPluginPluginClass parent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

void gplugin_perl_plugin_register(GPluginNativePlugin *plugin);
GType gplugin_perl_plugin_get_type(void);

PerlInterpreter *gplugin_perl_plugin_get_interpreter(const GPluginPerlPlugin *plugin);

G_END_DECLS

#endif /* GPLUGIN_PERL_PLUGIN_H */
