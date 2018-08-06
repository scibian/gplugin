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

#include "gplugin-perl-plugin.h"

#include <EXTERN.h>
#include <perl.h>

/* perl define's _() to something completely different that we don't use.  So
 * we undef it so that we can use it for gettext.
 */
#undef _
#include <glib/gi18n.h>

#define GPLUGIN_PERL_PLUGIN_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_PERL_PLUGIN, GPluginPerlPluginPrivate))

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_INTERPRETER,
	PROP_LAST,
};

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct {
	PerlInterpreter *interpreter;
} GPluginPerlPluginPrivate;

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static volatile GType type_real = 0;

/******************************************************************************
 * Private Stuff
 *****************************************************************************/
static void
gplugin_perl_plugin_set_interpreter(GPluginPerlPlugin *plugin,
                                    PerlInterpreter *interpreter)
{
	GPluginPerlPluginPrivate *priv = GPLUGIN_PERL_PLUGIN_GET_PRIVATE(plugin);

	priv->interpreter = interpreter;
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_perl_plugin_get_property(GObject *obj, guint param_id, GValue *value,
                                 GParamSpec *pspec)
{
	GPluginPerlPlugin *plugin = GPLUGIN_PERL_PLUGIN(obj);

	switch(param_id) {
		case PROP_INTERPRETER:
			g_value_set_pointer(value,
			                    gplugin_perl_plugin_get_interpreter(plugin));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_perl_plugin_set_property(GObject *obj, guint param_id,
                                 const GValue *value, GParamSpec *pspec)
{
	GPluginPerlPlugin *plugin = GPLUGIN_PERL_PLUGIN(obj);

	switch(param_id) {
		case PROP_INTERPRETER:
			gplugin_perl_plugin_set_interpreter(plugin,
			                                    g_value_get_pointer(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
	}
}

static void
gplugin_perl_plugin_finalize(GObject *obj) {
	GPluginPerlPluginPrivate *priv = GPLUGIN_PERL_PLUGIN_GET_PRIVATE(obj);

	perl_destruct(priv->interpreter);
	perl_free(priv->interpreter);
	priv->interpreter = NULL;

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_perl_plugin_class_init(GPluginPerlPluginClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginPerlPluginPrivate));

	obj_class->get_property = gplugin_perl_plugin_get_property;
	obj_class->set_property = gplugin_perl_plugin_set_property;
	obj_class->finalize = gplugin_perl_plugin_finalize;

	g_object_class_install_property(obj_class, PROP_INTERPRETER,
		g_param_spec_pointer("interpreter", "interpreter",
		                     "The PERL interpreter for this plugin",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
		                     G_PARAM_STATIC_STRINGS));
}

/******************************************************************************
 * API
 *****************************************************************************/
void
gplugin_perl_plugin_register(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginPerlPluginClass),
			.class_init = (GClassInitFunc)gplugin_perl_plugin_class_init,
			.instance_size = sizeof(GPluginPerlPlugin),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           GPLUGIN_TYPE_PLUGIN,
		                                           "GPluginPerlPlugin",
		                                           &info,
		                                           0);

		g_once_init_leave(&type_real, type);
	}
}

GType
gplugin_perl_plugin_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("gplugin_perl_plugin_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

PerlInterpreter *
gplugin_perl_plugin_get_interpreter(const GPluginPerlPlugin *plugin) {
	GPluginPerlPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PERL_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PERL_PLUGIN_GET_PRIVATE(plugin);

	return priv->interpreter;
}

