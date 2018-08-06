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

#include <Python.h>

#include "gplugin-python-plugin.h"

#include <pygobject.h>

#define GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_PYTHON_PLUGIN, GPluginPythonPluginPrivate))

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct {
	PyObject *module;
	PyObject *query;
	PyObject *load;
	PyObject *unload;
} GPluginPythonPluginPrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO,
	PROP_MODULE,
	PROP_LOAD_FUNC,
	PROP_UNLOAD_FUNC,
	PROP_LAST,
};

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static GType type_real = 0;

/******************************************************************************
 * Private Stuff
 *****************************************************************************/
static PyObject *
gplugin_python_plugin_get_module(const GPluginPythonPlugin *plugin) {
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PYTHON_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	return priv->module;
}

static void
gplugin_python_plugin_set_module(GPluginPythonPlugin *plugin,
                                 PyObject *module)
{
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_if_fail(GPLUGIN_IS_PLUGIN(plugin));
	g_return_if_fail(module);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	if(priv->module)
		Py_DECREF(priv->module);

	priv->module = module;

	if(priv->module)
		Py_INCREF(priv->module);
}

static gpointer
gplugin_python_plugin_get_load_func(const GPluginPythonPlugin *plugin) {
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PYTHON_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	return priv->load;
}

static void
gplugin_python_plugin_set_load_func(GPluginPythonPlugin *plugin,
                                     PyObject *func)
{
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_if_fail(GPLUGIN_IS_PYTHON_PLUGIN(plugin));
	g_return_if_fail(func != NULL);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	if(priv->load)
		Py_DECREF(priv->load);

	priv->load = func;
	if(priv->load)
		Py_INCREF(priv->load);
}

static gpointer
gplugin_python_plugin_get_unload_func(const GPluginPythonPlugin *plugin) {
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PYTHON_PLUGIN(plugin), NULL);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	return priv->unload;
}

static void
gplugin_python_plugin_set_unload_func(GPluginPythonPlugin *plugin,
                                     PyObject *func)
{
	GPluginPythonPluginPrivate *priv = NULL;

	g_return_if_fail(GPLUGIN_IS_PYTHON_PLUGIN(plugin));
	g_return_if_fail(func != NULL);

	priv = GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(plugin);

	if(priv->unload)
		Py_DECREF(priv->unload);

	priv->unload = func;
	if(priv->unload)
		Py_INCREF(priv->unload);
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_python_plugin_get_property(GObject *obj, guint param_id, GValue *value,
                                   GParamSpec *pspec)
{
	GPluginPythonPlugin *plugin = GPLUGIN_PYTHON_PLUGIN(obj);

	switch(param_id) {
		case PROP_MODULE:
			g_value_set_pointer(value,
			                    gplugin_python_plugin_get_module(plugin));
			break;
		case PROP_LOAD_FUNC:
			g_value_set_pointer(value,
			                    gplugin_python_plugin_get_load_func(plugin));
			break;
		case PROP_UNLOAD_FUNC:
			g_value_set_pointer(value,
			                    gplugin_python_plugin_get_unload_func(plugin));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_python_plugin_set_property(GObject *obj, guint param_id,
                                   const GValue *value, GParamSpec *pspec)
{
	GPluginPythonPlugin *plugin = GPLUGIN_PYTHON_PLUGIN(obj);

	switch(param_id) {
		case PROP_MODULE:
			gplugin_python_plugin_set_module(plugin,
			                                 g_value_get_pointer(value));
			break;
		case PROP_LOAD_FUNC:
			gplugin_python_plugin_set_load_func(plugin,
			                                    g_value_get_pointer(value));
			break;
		case PROP_UNLOAD_FUNC:
			gplugin_python_plugin_set_unload_func(plugin,
			                                      g_value_get_pointer(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_python_plugin_finalize(GObject *obj) {
	GPluginPythonPluginPrivate *priv =
		GPLUGIN_PYTHON_PLUGIN_GET_PRIVATE(obj);

	if(priv->module)
		Py_DECREF(priv->module);
	if(priv->load)
		Py_DECREF(priv->load);
	if(priv->unload)
		Py_DECREF(priv->unload);

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_python_plugin_class_init(GPluginPythonPluginClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginPythonPluginPrivate));

	obj_class->get_property = gplugin_python_plugin_get_property;
	obj_class->set_property = gplugin_python_plugin_set_property;
	obj_class->finalize = gplugin_python_plugin_finalize;

	g_object_class_install_property(obj_class, PROP_MODULE,
		g_param_spec_pointer("module", "module",
		                     "The python module object",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(obj_class, PROP_LOAD_FUNC,
		g_param_spec_pointer("load-func", "load-func",
		                     "The python load function",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(obj_class, PROP_UNLOAD_FUNC,
		g_param_spec_pointer("unload-func", "unload-func",
		                     "The python unload function",
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

/******************************************************************************
 * API
 *****************************************************************************/
void
gplugin_python_plugin_register(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginPythonPluginClass),
			.class_init = (GClassInitFunc)gplugin_python_plugin_class_init,
			.instance_size = sizeof(GPluginPythonPlugin),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           GPLUGIN_TYPE_PLUGIN,
		                                           "GPluginPythonPlugin",
		                                           &info,
		                                           0);

		g_once_init_leave(&type_real, type);
	}
}

GType
gplugin_python_plugin_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("gplugin_python_plugin_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

