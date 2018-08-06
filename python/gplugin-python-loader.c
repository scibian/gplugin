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

#include <stdlib.h>

#include <Python.h>

#include "gplugin-python-loader.h"

#include "gplugin-python-plugin.h"
#include "gplugin-python-utils.h"

#include <glib/gi18n.h>

#include <pygobject.h>

#define GPLUGIN_PYTHON_LOADER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_PYTHON_LOADER, GPluginPythonLoaderPrivate))

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct {
	PyThreadState *py_thread_state;
	guint gc_id;
} GPluginPythonLoaderPrivate;

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;
static GType type_real = 0;

/******************************************************************************
 * GPluginLoaderInterface API
 *****************************************************************************/
static GSList *
gplugin_python_loader_class_supported_extensions(GPLUGIN_UNUSED const GPluginLoaderClass *klass) {
	return g_slist_append(NULL, "py");
}

static GPluginPlugin *
gplugin_python_loader_query(GPLUGIN_UNUSED GPluginLoader *loader,
                            const gchar *filename,
                            GPLUGIN_UNUSED GError **error)
{
	GPluginPlugin *plugin = NULL;
	GObject *info = NULL;
	PyObject *pyinfo = NULL, *args = NULL;
	PyObject *module = NULL, *package_list = NULL, *module_dict = NULL;
	PyObject *query = NULL, *load = NULL, *unload = NULL;
	PyGILState_STATE state;
	gchar *module_name = NULL, *dir_name = NULL;

	/* lock the gil */
	state = pyg_gil_state_ensure();

	/* create package_list as a tuple to handle 'import foo.bar' */
	package_list = PyTuple_New(0);

	/* now figure out the module name from the filename */
	module_name = gplugin_python_filename_to_module(filename);

	/* grab the dirname since we need it on sys.path to import the module */
	dir_name = g_path_get_dirname(filename);
	gplugin_python_add_module_path(dir_name);
	g_free(dir_name);

	/* import the module */
	module = PyImport_ImportModuleEx(module_name, NULL, NULL, package_list);
	if(PyErr_Occurred()) {
		g_warning(_("Failed to query %s"), filename);
		PyErr_Print();

		pyg_gil_state_release(state);

		return NULL;
	}

	/* clean some stuff up */
	g_free(module_name);
	Py_DECREF(package_list);

	/* at this point we have the module, lets find the query, load, and unload
	 * functions.
	 */
	module_dict = PyModule_GetDict(module);

	query = PyDict_GetItemString(module_dict, "gplugin_query");
	if(query == NULL) {
		g_warning(_("Failed to find the gplugin_query function in %s"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}
	if(!PyCallable_Check(query)) {
		g_warning(_("Found gplugin_query in %s but it is not a "
		            "function"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}

	load = PyDict_GetItemString(module_dict, "gplugin_load");
	if(load == NULL) {
		g_warning(_("Failed to find the gplugin_load function in %s"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}
	if(!PyCallable_Check(load)) {
		g_warning(_("Found gplugin_load in %s but it is not a "
		            "function"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}

	unload = PyDict_GetItemString(module_dict, "gplugin_unload");
	if(unload == NULL) {
		g_warning(_("Failed to find the gplugin_unload function in %s"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}
	if(!PyCallable_Check(unload)) {
		g_warning(_("Found gplugin_unload in %s but it is not a "
		            "function"),
		          filename);

		pyg_gil_state_release(state);

		return NULL;
	}

	/* now that we have everything, call the query method and get the plugin's
	 * info.
	 */
	args = PyTuple_New(0);
	pyinfo = PyObject_Call(query, args, NULL);
	Py_DECREF(args);

	info = pygobject_get(pyinfo);

	/* now that we have everything, create the plugin */
	plugin = g_object_new(GPLUGIN_TYPE_PYTHON_PLUGIN,
	                      "filename", filename,
	                      "loader", loader,
	                      "module", module,
	                      "info", info,
	                      "load-func", load,
	                      "unload-func", unload,
	                      NULL);

	/* unlock the gil */
	pyg_gil_state_release(state);

	return plugin;
}

static gboolean
gplugin_python_loader_load(GPLUGIN_UNUSED GPluginLoader *loader,
                           GPluginPlugin *plugin,
                           GError **error)
{
	PyObject *load = NULL, *pyplugin = NULL, *result = NULL;
	gboolean ret = FALSE;

	g_object_get(G_OBJECT(plugin), "load-func", &load, NULL);

	pyplugin = pygobject_new(G_OBJECT(plugin));

	result = PyObject_CallFunctionObjArgs(load, pyplugin, NULL);
	Py_DECREF(pyplugin);

	if(error) {
		*error = gplugin_python_exception_to_gerror();
		if(*error)
			return FALSE;
	}

	ret = PyObject_IsTrue(result);
	Py_DECREF(result);

	if(!ret) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("Failed to load plugin"));
		}
	}

	return ret;
}

static gboolean
gplugin_python_loader_unload(GPLUGIN_UNUSED GPluginLoader *loader,
                             GPluginPlugin *plugin,
                             GError **error)
{
	PyObject *unload = NULL, *pyplugin = NULL, *result = NULL;
	gboolean ret = FALSE;

	g_object_get(G_OBJECT(plugin), "unload-func", &unload, NULL);

	pyplugin = pygobject_new(G_OBJECT(plugin));

	result = PyObject_CallFunctionObjArgs(unload, pyplugin, NULL);
	Py_DECREF(pyplugin);

	if(PyErr_Occurred()) {
		PyErr_Print();

		Py_DECREF(result);

		return FALSE;
	}

	ret = PyObject_IsTrue(result);
	Py_DECREF(result);

	if(!ret) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("Failed to unload plugin"));
		}
	}

	return ret;
}

/******************************************************************************
 * Python Stuff
 *****************************************************************************/
static gboolean
gplugin_python_loader_init_pygobject(void) {
	pygobject_init(3, 0, 0);
	if(PyErr_Occurred()) {
		PyObject *type = NULL, *value = NULL, *tb = NULL, *obj = NULL;

		PyErr_Fetch(&type, &value, &tb);
		Py_DECREF(type);

		obj = PyUnicode_AsUTF8String(value);
		Py_DECREF(value);

		g_warning("Failed to initialize PyGObject : %s", PyBytes_AsString(obj));
		Py_DECREF(obj);

		return FALSE;
	}

	/* enable threads */
	pyg_enable_threads();

	/* disable g_log redirections */
	pyg_disable_warning_redirections();

	return TRUE;
}

static gboolean
gplugin_python_loader_init_gettext(void) {
	PyObject *module_dict = NULL, *install = NULL;
	PyObject *gettext = NULL, *gettext_args = NULL;

	gettext = PyImport_ImportModule("gettext");
	if(gettext == NULL) {
		g_warning("Failed to import gettext");

		return FALSE;
	}

	module_dict = PyModule_GetDict(gettext);
	install = PyDict_GetItemString(module_dict, "install");
	gettext_args = Py_BuildValue("ss", GETTEXT_PACKAGE, LOCALEDIR);
	PyObject_CallObject(install, gettext_args);
	Py_DECREF(gettext_args);

	return TRUE;
}

static gboolean
gplugin_python_loader_init_python(void) {
	const gchar *program = NULL;
	wchar_t *argv[] = { NULL, NULL };
	size_t len;

	/* Initializes Python */
	if(!Py_IsInitialized())
		Py_InitializeEx(FALSE);

	program = g_get_prgname();
	program = program ? program : "";
	len = mbstowcs(NULL, program, 0);
	if(len == (size_t)-1) {
		g_warning("Could not convert program name to wchar_t string.");
		return FALSE;
	}

	argv[0] = g_new(wchar_t, len + 1);
	len = mbstowcs(argv[0], program, len + 1);
	if(len == (size_t)-1) {
		g_warning("Could not convert program name to wchar_t string.");
		return FALSE;
	}

	/* setup sys.path according to
	 * http://docs.python.org/3/c-api/init.html#PySys_SetArgvEx
	 */
#if PY_VERSION_HEX < 0x03010300
	PySys_SetArgv(1, argv);
	PyRun_SimpleString("import sys; sys.path.pop(0)\n");
	g_free(argv[0]);
#else
	PySys_SetArgvEx(1, argv, 0);
	g_free(argv[0]);
#endif

	/* initialize pygobject */
	if(gplugin_python_loader_init_pygobject()) {
		if(gplugin_python_loader_init_gettext()) {
			return TRUE;
		}
	}

	return FALSE;
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_python_loader_finalize(GObject *obj) {
#if 0
	GPluginPythonLoaderPrivate *priv =
		GPLUGIN_PYTHON_LOADER_GET_PRIVATE(obj);
#endif

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_python_loader_class_init(GPluginPythonLoaderClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GPluginLoaderClass *loader_class =
		GPLUGIN_LOADER_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginPythonLoaderPrivate));

	obj_class->finalize = gplugin_python_loader_finalize;

	loader_class->supported_extensions =
		gplugin_python_loader_class_supported_extensions;
	loader_class->query = gplugin_python_loader_query;
	loader_class->load = gplugin_python_loader_load;
	loader_class->unload = gplugin_python_loader_unload;
}

/******************************************************************************
 * API
 *****************************************************************************/
void
gplugin_python_loader_register(GPluginNativePlugin *plugin) {
	if(g_once_init_enter(&type_real)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginPythonLoaderClass),
			.class_init = (GClassInitFunc)gplugin_python_loader_class_init,
			.instance_size = sizeof(GPluginPythonLoader),
		};

		type = gplugin_native_plugin_register_type(plugin,
		                                           GPLUGIN_TYPE_LOADER,
		                                           "GPluginPythonLoader",
		                                           &info,
		                                           0);

		gplugin_python_loader_init_python();

		g_once_init_leave(&type_real, type);
	}
}

GType
gplugin_python_loader_get_type(void) {
	if(G_UNLIKELY(type_real == 0)) {
		g_warning("gplugin_python_loader_get_type was called before "
		          "the type was registered!\n");
	}

	return type_real;
}

