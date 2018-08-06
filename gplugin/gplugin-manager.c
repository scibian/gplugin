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

#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>

#include <gplugin/gplugin-manager.h>
#include <gplugin/gplugin-core.h>
#include <gplugin/gplugin-marshallers.h>

#include <gplugin/gplugin-private.h>

#include <gplugin/gplugin-native.h>

#include <gplugin/gplugin-file-tree.h>

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	SIG_LOADING,
	SIG_LOADED,
	SIG_LOAD_FAILED,
	SIG_UNLOADING,
	SIG_UNLOADED,
	N_SIGNALS,
};

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	GObject parent;

	GQueue *paths;
	GHashTable *plugins;
	GHashTable *plugins_filename_view;

	GHashTable *loaders;

	gboolean refresh_needed;
} GPluginManager;

typedef struct {
	GObjectClass parent;

	void (*append_path)(GPluginManager *manager, const gchar *path);
	void (*prepend_path)(GPluginManager *manager, const gchar *path);
	void (*remove_path)(GPluginManager *manager, const gchar *path);
	void (*remove_paths)(GPluginManager *manager);

	GList *(*get_paths)(GPluginManager *manager);

	void (*register_loader)(GPluginManager *manager, GType type);
	void (*unregister_loader)(GPluginManager *manager, GType type);

	void (*refresh)(GPluginManager *manager);

	GSList *(*find_plugins)(GPluginManager *manager, const gchar *id);

	GList *(*list_plugins)(GPluginManager *manager);

	gboolean (*load_plugin)(GPluginManager *manager,
	                        GPluginPlugin *plugin,
	                        GError **error);
	gboolean (*unload_plugin)(GPluginManager *manager,
	                          GPluginPlugin *plugin,
	                          GError **error);

	/* signals */
	gboolean (*loading_plugin)(GObject *manager,
	                           GPluginPlugin *plugin,
	                           GError **error);
	void (*loaded_plugin)(GObject *manager,
	                      GPluginPlugin *plugin);
	void (*load_failed)(GObject *manager, GPluginPlugin *plugin);
	gboolean (*unloading_plugin)(GObject *manager,
	                             GPluginPlugin *plugin,
	                             GError **error);
	void (*unloaded_plugin)(GObject *manager,
	                        GPluginPlugin *plugin);

} GPluginManagerClass;

#define GPLUGIN_TYPE_MANAGER            (gplugin_manager_get_type())
#define GPLUGIN_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_MANAGER, GPluginManager))
#define GPLUGIN_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_TYPE_MANAGER, GPluginManagerClass))
#define GPLUGIN_IS_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_MANAGER))
#define GPLUGIN_IS_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_TYPE_MANAGER))
#define GPLUGIN_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_MANAGER, GPluginManagerClass))

#define GPLUGIN_MANAGER_INSTANCE        (GPLUGIN_MANAGER(gplugin_manager_get_instance()))

G_DEFINE_TYPE(GPluginManager, gplugin_manager, G_TYPE_OBJECT);

/******************************************************************************
 * Globals
 *****************************************************************************/
GPluginManager *instance = NULL;
static guint signals[N_SIGNALS] = {0, };
const gchar *dependency_pattern = "^(?P<id>.+?)((?P<op>\\<=|\\<|==|=|\\>=|\\>)(?P<version>.+))?$";
GRegex *dependency_regex = NULL;

/******************************************************************************
 * Helpers
 *****************************************************************************/
static guint
gplugin_manager_str_hash(gconstpointer v) {
	if(v == NULL)
		return g_str_hash("");

	return g_str_hash(v);
}

static gboolean
gplugin_manager_remove_list_value(GPLUGIN_UNUSED gpointer k,
                                  gpointer v,
                                  GPLUGIN_UNUSED gpointer d)
{
	GSList *l = NULL;

	for(l = (GSList *)v; l; l = l->next) {
		if(l->data && G_IS_OBJECT(l->data))
			g_object_unref(G_OBJECT(l->data));
	}

	g_slist_free((GSList *)v);

	return TRUE;
}

/******************************************************************************
 * Manager implementation
 *****************************************************************************/
static void
gplugin_manager_real_append_path(GPluginManager *manager,
                                 const gchar *path)
{
	GList *l = NULL;

	if(!path)
		return;

	for(l = manager->paths->head; l; l = l->next)
		if(strcmp(l->data, path) == 0)
			return;

	g_queue_push_tail(manager->paths, g_strdup(path));
}

static void
gplugin_manager_real_prepend_path(GPluginManager *manager,
                                  const gchar *path)
{
	GList *l = NULL;

	if(!path)
		return;

	for(l = manager->paths->head; l; l = l->next)
		if(strcmp(l->data, path) == 0)
			return;

	g_queue_push_head(manager->paths, g_strdup(path));
}

static void
gplugin_manager_real_remove_path(GPluginManager *manager,
                                 const gchar *path)
{
	GList *l = NULL, *link = NULL;

	g_return_if_fail(path != NULL);

	for(l = manager->paths->head; l; l = l->next)
		if(strcmp(l->data, path) == 0) {
			g_free(l->data);
			link = l;
		}

	if(link)
		g_queue_delete_link(manager->paths, link);
}

static void
gplugin_manager_real_remove_paths(GPluginManager *manager) {
	g_queue_clear(manager->paths);
}

static GList *
gplugin_manager_real_get_paths(GPluginManager *manager) {
	return manager->paths->head;
}

static void
gplugin_manager_real_register_loader(GPluginManager *manager,
                                     GType type)
{
	GPluginLoader *loader = NULL;
	GPluginLoaderClass *lo_class = NULL;
	GSList *l = NULL;

	g_return_if_fail(g_type_is_a(type, GPLUGIN_TYPE_LOADER));

	/* Create the loader instance first.  If we can't create it, we bail */
	loader = g_object_new(type, NULL);
	if(!GPLUGIN_IS_LOADER(loader)) {
		g_warning(_("failed to create loader instance for %s"),
		          g_type_name(type));

		return;
	}

	/* grab the class of the loader */
	lo_class = GPLUGIN_LOADER_GET_CLASS(loader);
	if(!lo_class) {
		g_warning(_("failed to get the loader class for %s"), g_type_name(type));
		g_object_unref(G_OBJECT(loader));

		return;
	}

	for(l = gplugin_loader_class_get_supported_extensions(lo_class);
	    l; l = l->next)
	{
		GSList *existing = NULL, *ll = NULL;
		const gchar *ext = (const gchar *)l->data;

		/* grab any existing loaders that are registered for this type so that
		 * we can prepend our loader.  But before we add ours, we remove any
		 * old copies we might have of ours.
		 */
		existing = g_hash_table_lookup(manager->loaders, ext);
		for(ll = existing; ll; ll = ll->next) {
			if(G_OBJECT_TYPE(ll->data) == type) {
				GPluginLoader *old = GPLUGIN_LOADER(ll->data);

				existing = g_slist_remove(existing, old);

				g_object_unref(G_OBJECT(old));

				break;
			}
		}

		existing = g_slist_prepend(existing, g_object_ref(G_OBJECT(loader)));

		/* Now insert the updated slist back into the hash table */
		g_hash_table_insert(manager->loaders, g_strdup(ext), existing);
	}

	/* make a note that we need to refresh */
	manager->refresh_needed = TRUE;

	/* we remove our initial reference from the loader now to avoid a leak */
	g_object_unref(G_OBJECT(loader));
}

static void
gplugin_manager_real_unregister_loader(GPluginManager *manager,
                                       GType type)
{
	GPluginLoaderClass *klass = NULL;
	GSList *exts = NULL;

	g_return_if_fail(g_type_is_a(type, GPLUGIN_TYPE_LOADER));

	klass = g_type_class_ref(type);
	if(!klass)
		return;

	for(exts = gplugin_loader_class_get_supported_extensions(klass);
	    exts; exts = exts->next) {
		GSList *los = NULL;
		GSList *l = NULL;
		const gchar *ext = NULL;

		ext = (const gchar *)exts->data;
		los = g_hash_table_lookup(manager->loaders, ext);

		for(l = los; l; l = l->next) {
			GPluginLoader *lo = GPLUGIN_LOADER(l->data);

			if(!GPLUGIN_IS_LOADER(lo))
				continue;

			if(G_OBJECT_TYPE(lo) != type)
				continue;

			/* at this point, the loader we're at is of the type we're
			 * removing.  So we'll remove it from the los SList.  Then if the
			 * SList is empty, we remove it from the hash table, otherwise we
			 * just update it.
			 */
			los = g_slist_remove(los, lo);
			if(los)
				g_hash_table_insert(manager->loaders, g_strdup(ext), los);
			else
				g_hash_table_remove(manager->loaders, ext);

			/* kill our ref to the loader */
			g_object_unref(G_OBJECT(lo));

			/* now move to the next extension to check */
			break;
		}
	}

	g_type_class_unref(klass);
}

static void
gplugin_manager_real_refresh(GPluginManager *manager) {
	GNode *root = NULL;
	GList *error_messages = NULL, *l = NULL;
	gchar *error_message = NULL;
	guint errors = 0;

	/* build a tree of all possible plugins */
	root = gplugin_file_tree_new(manager->paths->head);

	manager->refresh_needed = TRUE;

	while(manager->refresh_needed) {
		GNode *dir = NULL;

		if(error_messages) {
			for(l = error_messages; l; l = l->next)
				g_free(l->data);
			g_list_free(error_messages);
			error_messages = NULL;
		}

		manager->refresh_needed = FALSE;

		for(dir = root->children; dir; dir = dir->next) {
			GNode *file = NULL;
			const gchar *path = (const gchar *)dir->data;

			for(file = dir->children; file; file = file->next) {
				GPluginPlugin *plugin = NULL;
				GPluginLoader *loader = NULL;
				GPluginFileTreeEntry *e = NULL;
				GError *error = NULL;
				GSList *l = NULL;
				gchar *filename = NULL;

				e = (GPluginFileTreeEntry *)file->data;

				/* Build the path and see if we need to probe it! */
				filename = g_build_filename(path, e->filename, NULL);
				plugin = g_hash_table_lookup(manager->plugins_filename_view, filename);

				if(plugin && GPLUGIN_IS_PLUGIN(plugin)) {
					GPluginPluginState state =
						gplugin_plugin_get_state(plugin);

					/* The plugin is in our "view", check it's state.  If it's
					 * queried or loaded, move on to the next one.
					 */
					if(state == GPLUGIN_PLUGIN_STATE_QUERIED ||
					   state == GPLUGIN_PLUGIN_STATE_LOADED)
					{
						g_free(filename);
						continue;
					}
				}

				/* grab the list of loaders for this extension */
				for(l = g_hash_table_lookup(manager->loaders, e->extension); l;
				    l = l->next)
				{
					if(!l->data)
						continue;

					loader = GPLUGIN_LOADER(l->data);
					if(!GPLUGIN_IS_LOADER(loader)) {
						loader = NULL;
						continue;
					}

					/* Try to probe the plugin with the current loader */
					plugin = gplugin_loader_query_plugin(loader,
					                                     filename,
				                                         &error);

					/* Check the GError, if it's set, output it's message and
					 * try the next loader.
					 */
					if(plugin == NULL || error) {
						errors++;

						error_message =
							g_strdup_printf(_("failed to query '%s' with " \
							                  "loader '%s': %s"), filename,
							                  G_OBJECT_TYPE_NAME(loader),
							                  (error) ? error->message : _("Unknown"));
						error_messages = g_list_prepend(error_messages, error_message);

						if(error)
							g_error_free(error);

						error = NULL;

						loader = NULL;

						continue;
					}

					/* if the plugin instance is good, then break out of this
					 * loop.
					 */
					if(plugin != NULL && GPLUGIN_IS_PLUGIN(plugin)) {
						break;
					}

					g_object_unref(G_OBJECT(plugin));

					loader = NULL;
				}

				/* check if our plugin instance is good.  If it's not good we
				 * don't need to do anything but free the filename which we'll
				 * do later.
				 */
				if(plugin != NULL && GPLUGIN_IS_PLUGIN(plugin)) {
					/* we have a good plugin, huzzah!  We need to add it to our
					 * "view" as well as the main plugin hash table.
					 */

					/* we want the internal filename from the plugin to avoid
					 * duplicate memory, so we need to grab it for the "view".
					 */
					gchar *real_filename =
						gplugin_plugin_get_internal_filename(plugin);

					/* we also need the GPluginPluginInfo to get the plugin's
					 * ID for the key in our main hash table.
					 */
					const GPluginPluginInfo *info =
						gplugin_plugin_get_info(plugin);

					const gchar *id = gplugin_plugin_info_get_id(info);
					GSList *l = NULL, *ll = NULL;
					gboolean seen = FALSE;

					/* throw a warning if the info->id is NULL */
					if(id == NULL) {
						error_message =
							g_strdup_printf(_("Plugin %s has a NULL id."),
							                real_filename);
						error_messages = g_list_prepend(error_messages, error_message);
					}

					/* now insert into our view */
					g_hash_table_insert(manager->plugins_filename_view,
					                    real_filename,
					                    g_object_ref(G_OBJECT(plugin)));

					/* Grab the list of plugins with our id and prepend the new
					 * plugin to it before updating it.
					 */
					l = g_hash_table_lookup(manager->plugins, id);
					for(ll = l; ll; ll = ll->next) {
						GPluginPlugin *splugin = GPLUGIN_PLUGIN(ll->data);
						const gchar *sfilename = gplugin_plugin_get_filename(splugin);

						if(!g_strcmp0(real_filename, sfilename))
							seen = TRUE;
					}
					if(!seen) {
						l = g_slist_prepend(l, g_object_ref(plugin));
						g_hash_table_insert(manager->plugins, g_strdup(id), l);
					}

					/* check if the plugin is supposed to be loaded on query,
					 * and if so, load it.
					 */
					if(gplugin_plugin_info_get_load_on_query(info)) {
						GError *error = NULL;
						gboolean loaded;

						loaded = gplugin_loader_load_plugin(loader,
						                                    plugin,
						                                    &error);

						if(!loaded) {
							error_message =
								g_strdup_printf(_("failed to load %s during query: %s"),
								                filename,
								                (error) ? error->message : _("Unknown"));
							error_messages = g_list_prepend(error_messages, error_message);

							errors++;

							g_error_free(error);
						} else {
							gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_LOADED);
						}
					} else {
						/* finally set the plugin state queried */
						gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_QUERIED);

						/* if errors is greater than 0 set
						 * manager->refresh_needed to TRUE.
						 */
						if(errors > 0) {
							errors = 0;
							manager->refresh_needed = TRUE;
						}
					}

					g_object_unref(G_OBJECT(info));

				}

				g_free(filename);
			}
		}
	}

	if(error_messages) {
		error_messages = g_list_reverse(error_messages);
		for(l = error_messages; l; l = l->next) {
			g_warning("%s", (gchar *)l->data);
			g_free(l->data);
		}

		g_list_free(error_messages);
	}

	/* free the file tree */
	gplugin_file_tree_free(root);
}

static GSList *
gplugin_manager_real_find_plugins(GPluginManager *manager,
                                  const gchar *id)
{
	GSList *plugins_list = NULL, *l;

	g_return_val_if_fail(id != NULL, NULL);

	for(l = g_hash_table_lookup(manager->plugins, id); l; l = l->next) {
		GPluginPlugin *plugin = NULL;

		if(l->data == NULL)
			continue;

		plugin = GPLUGIN_PLUGIN(l->data);

		plugins_list = g_slist_prepend(plugins_list,
		                               g_object_ref(G_OBJECT(plugin)));
	}

	return plugins_list;
}

static GList *
gplugin_manager_real_list_plugins(GPluginManager *manager) {
	GQueue *queue = g_queue_new();
	GList *ret = NULL;
	GHashTableIter iter;
	gpointer key = NULL;

	g_hash_table_iter_init(&iter, manager->plugins);
	while(g_hash_table_iter_next(&iter, &key, NULL)) {
		g_queue_push_tail(queue, (gchar *)key);
	}

	ret = g_list_copy(queue->head);

	g_queue_free(queue);

	return ret;
}

static gboolean
gplugin_manager_load_dependencies(const GPluginPluginInfo *info,
                                  GError **error)
{
	const gchar * const *dependencies = NULL;
	gint i = 0;

	/* now walk through any dependencies the plugin has and load them.  If they
	 * fail to load we need to fail as well.
	 */
	dependencies = gplugin_plugin_info_get_dependencies(info);
	if(dependencies != NULL) {
		gboolean all_found = TRUE;

		for(i = 0; dependencies[i]; i++) {
			gboolean found = FALSE;
			gchar **ors = NULL;
			gint o = 0;

			ors = g_strsplit(dependencies[i], "|", 0);
			for(o = 0; ors[o]; o++) {
				GMatchInfo *match = NULL;
				GSList *matches = NULL, *m = NULL;
				gchar *oid = NULL, *oop = NULL, *over = NULL;

				if(!g_regex_match(dependency_regex, ors[o], 0, &match)) {
					continue;
				}

				/* grab the or'd id, op, and version */
				oid = g_match_info_fetch_named(match, "id");
				oop = g_match_info_fetch_named(match, "op");
				over = g_match_info_fetch_named(match, "version");

				/* free the match info */
				g_match_info_free(match);

				/* now look for a plugin matching the id */
				matches = gplugin_manager_find_plugins(oid);
				if(matches == NULL)
					continue;

				/* now iterate the matches and check if we need to check their
				 * version.
				 */
				for(m = matches; m; m = m->next) {
					GPluginPlugin *dplugin = g_object_ref(G_OBJECT(m->data));
					gboolean ret = FALSE;

					if(oop && over) {
						/* we need to check the version, so grab the info to
						 * get the version and check it.
						 */
						GPluginPluginInfo *dinfo = NULL;
						const gchar *dver = NULL;
						gboolean satisfied = FALSE;
						gint res = 0;

						dinfo = gplugin_plugin_get_info(dplugin);
						dver = gplugin_plugin_info_get_version(dinfo);

						res = gplugin_version_compare(dver, over, error);
						g_object_unref(G_OBJECT(dinfo));

						if(res < 0) {
							/* dver is greather than over */
							if(g_strcmp0(oop, ">") == 0)
								satisfied = TRUE;
						} else if(res == 0) {
							/* dver is equal to over */
							if(g_strcmp0(oop, ">=") == 0 ||
							   g_strcmp0(oop, "<=") == 0 ||
							   g_strcmp0(oop, "=") == 0 ||
							   g_strcmp0(oop, "==") == 0)
							{
								satisfied = TRUE;
							}
						} else if(res > 0) {
							if(g_strcmp0(oop, "<") == 0)
								satisfied = TRUE;
						}

						if(satisfied)
							found = TRUE;
					}

					ret = gplugin_manager_load_plugin(dplugin, error);

					g_object_unref(G_OBJECT(dplugin));

					if(ret) {
						found = TRUE;
						break;
					}
				}

				if(found)
					break;
			}
			g_strfreev(ors);

			if(!found)
				all_found = FALSE;
		}

		if(!all_found) {
			return FALSE;
		}
	}

	return TRUE;
}

static gboolean
gplugin_manager_real_load_plugin(GPluginManager *manager,
                                 GPluginPlugin *plugin,
                                 GError **error)
{
	const GPluginPluginInfo *info = NULL;
	GPluginLoader *loader = NULL;
	gboolean ret = TRUE;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);

	/* if the plugin is already loaded there's nothing for us to do */
	if(gplugin_plugin_get_state(plugin) == GPLUGIN_PLUGIN_STATE_LOADED)
		return TRUE;

	/* now try to get the plugin info from the plugin */
	info = gplugin_plugin_get_info(plugin);
	if(info == NULL) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("Plugin %s did not return value plugin info"),
			                     gplugin_plugin_get_filename(plugin));
		}

		gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

		return FALSE;
	}

	if(!gplugin_manager_load_dependencies(info, error)) {
		g_object_unref(G_OBJECT(info));

		return FALSE;
	}

	g_object_unref(G_OBJECT(info));

	/* now load the actual plugin */
	loader = gplugin_plugin_get_loader(plugin);

	if(!GPLUGIN_IS_LOADER(loader)) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("The loader for %s is not a loader.  This "
			                     "should not happend!"),
			                     gplugin_plugin_get_filename(plugin));
		}

		gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

		return FALSE;
	}

	g_signal_emit(manager, signals[SIG_LOADING], 0, plugin, error, &ret);
	if(!ret) {
		gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

		return ret;
	}

	ret = gplugin_loader_load_plugin(loader, plugin, error);
	gplugin_plugin_set_state(plugin, (ret) ? GPLUGIN_PLUGIN_STATE_LOADED :
	                                         GPLUGIN_PLUGIN_STATE_LOAD_FAILED);

	if(ret)
		g_signal_emit(manager, signals[SIG_LOADED], 0, plugin);
	else
		g_signal_emit(manager, signals[SIG_LOAD_FAILED], 0, plugin);

	return ret;
}

static gboolean
gplugin_manager_real_unload_plugin(GPluginManager *manager,
                                   GPluginPlugin *plugin,
                                   GError **error)
{
	GPluginLoader *loader = NULL;
	gboolean ret = TRUE;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);

	if(gplugin_plugin_get_state(plugin) != GPLUGIN_PLUGIN_STATE_LOADED)
		return TRUE;

	loader = gplugin_plugin_get_loader(plugin);
	if(!GPLUGIN_IS_LOADER(loader)) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("Plugin loader is not a loader"));
		}

		return FALSE;
	}

	g_signal_emit(manager, signals[SIG_UNLOADING], 0, plugin, error, &ret);
	if(!ret)
		return ret;

	ret = gplugin_loader_unload_plugin(loader, plugin, error);
	if(ret) {
		gplugin_plugin_set_state(plugin, GPLUGIN_PLUGIN_STATE_QUERIED);

		g_signal_emit(manager, signals[SIG_UNLOADED], 0, plugin, error);
	}

	return ret;
}

static gboolean
gplugin_manager_loading_cb(GPLUGIN_UNUSED GObject *manager,
                           GPLUGIN_UNUSED GPluginPlugin *plugin,
                           GPLUGIN_UNUSED GError **error)
{
	return TRUE;
}

static gboolean
gplugin_manager_unloading_cb(GPLUGIN_UNUSED GObject *manager,
                             GPLUGIN_UNUSED GPluginPlugin *plugin,
                             GPLUGIN_UNUSED GError **error)
{
	return TRUE;
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_manager_finalize(GObject *obj) {
	GPluginManager *manager = GPLUGIN_MANAGER(obj);

	g_queue_free_full(manager->paths, g_free);

	/* free all the data in the plugins hash table and destroy it */
	g_hash_table_foreach_remove(manager->plugins,
	                            gplugin_manager_remove_list_value,
	                            NULL);
	g_hash_table_destroy(manager->plugins);

	/* destroy the filename view */
	g_hash_table_destroy(manager->plugins_filename_view);

	/* free all the data in the loaders hash table and destroy it */
	g_hash_table_foreach_remove(manager->loaders,
	                            gplugin_manager_remove_list_value,
	                            NULL);
	g_hash_table_destroy(manager->loaders);

	/* call the base class's destructor */
	G_OBJECT_CLASS(gplugin_manager_parent_class)->finalize(obj);
}

static void
gplugin_manager_class_init(GPluginManagerClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GPluginManagerClass *manager_class =
		GPLUGIN_MANAGER_CLASS(klass);

	obj_class->finalize = gplugin_manager_finalize;

	manager_class->append_path = gplugin_manager_real_append_path;
	manager_class->prepend_path = gplugin_manager_real_prepend_path;
	manager_class->remove_path = gplugin_manager_real_remove_path;
	manager_class->remove_paths = gplugin_manager_real_remove_paths;
	manager_class->get_paths = gplugin_manager_real_get_paths;

	manager_class->register_loader =
		gplugin_manager_real_register_loader;
	manager_class->unregister_loader =
		gplugin_manager_real_unregister_loader;

	manager_class->refresh = gplugin_manager_real_refresh;

	manager_class->find_plugins = gplugin_manager_real_find_plugins;
	manager_class->list_plugins = gplugin_manager_real_list_plugins;

	manager_class->load_plugin = gplugin_manager_real_load_plugin;
	manager_class->unload_plugin = gplugin_manager_real_unload_plugin;

	manager_class->loading_plugin = gplugin_manager_loading_cb;
	manager_class->unloading_plugin = gplugin_manager_unloading_cb;

	/* signals */

	/**
	 * GPluginManager::loading-plugin:
	 * @manager: The #GPluginManager instance.  Treat as a #GObject.
	 * @plugin: The #GPluginPlugin that's about to be loaded.
	 * @error: Return address for a #GError.
	 *
	 * Emitted before @plugin is loaded.
	 *
	 * Return FALSE to stop loading
	 */
	signals[SIG_LOADING] =
		g_signal_new("loading-plugin",
		             G_OBJECT_CLASS_TYPE(manager_class),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginManagerClass,
		                             loading_plugin),
		             gplugin_boolean_accumulator, NULL,
		             gplugin_marshal_BOOLEAN__OBJECT_POINTER,
		             G_TYPE_BOOLEAN,
		             2,
		             GPLUGIN_TYPE_PLUGIN, G_TYPE_POINTER);

	/**
	 * GPluginManager::loaded-plugin:
	 * @manager: the #gpluginpluginmanager instance.  treat as a #gobject.
	 * @plugin: the #gpluginplugin that's about to be loaded.
	 * @error: return address for a #gerror.
	 *
	 * emitted after a plugin is loaded.
	 */
	signals[SIG_LOADED] =
		g_signal_new("loaded-plugin",
		             G_OBJECT_CLASS_TYPE(manager_class),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginManagerClass,
		                             loaded_plugin),
		             NULL, NULL,
		             gplugin_marshal_VOID__OBJECT,
		             G_TYPE_NONE,
		             1,
		             GPLUGIN_TYPE_PLUGIN);

	/**
	 * GPluginManager::load-failed:
	 * @manager: The #GPluginPluginManager instance.
	 * @plugin: The #GPluginPlugin that failed to load.
	 *
	 * emitted after a plugin fails to load.
	 */
	signals[SIG_LOAD_FAILED] =
		g_signal_new("load-failed",
		             G_OBJECT_CLASS_TYPE(manager_class),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginManagerClass, load_failed),
		             NULL,
		             NULL,
		             gplugin_marshal_VOID__OBJECT,
		             G_TYPE_NONE,
		             1,
		             GPLUGIN_TYPE_PLUGIN);

	/**
	 * GPluginManager::unloading-plugin:
	 * @manager: the #GPluginPluginManager instance.  treat as a #GObject.
	 * @plugin: the #GPluginPlugin that's about to be loaded.
	 *
	 * emitted before a plugin is unloaded.
	 *
	 * Return FALSE to stop unloading
	 */
	signals[SIG_UNLOADING] =
		g_signal_new("unloading-plugin",
		             G_OBJECT_CLASS_TYPE(manager_class),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginManagerClass,
		                             unloading_plugin),
		             gplugin_boolean_accumulator, NULL,
		             gplugin_marshal_BOOLEAN__OBJECT_POINTER,
		             G_TYPE_BOOLEAN,
		             2,
		             GPLUGIN_TYPE_PLUGIN, G_TYPE_POINTER);

	/**
	 * GPluginManager::unloaded-plugin:
	 * @manager: the #gpluginpluginmanager instance.  treat as a #gobject.
	 * @plugin: the #gpluginplugin that's about to be loaded.
	 * @error: return address for a #gerror.
	 *
	 * emitted after a plugin is unloaded.
	 */
	signals[SIG_UNLOADED] =
		g_signal_new("unloaded-plugin",
		             G_OBJECT_CLASS_TYPE(manager_class),
		             G_SIGNAL_RUN_LAST,
		             G_STRUCT_OFFSET(GPluginManagerClass,
		                             unloaded_plugin),
		             NULL, NULL,
		             gplugin_marshal_VOID__OBJECT,
		             G_TYPE_NONE,
		             1,
		             GPLUGIN_TYPE_PLUGIN);
}

static void
gplugin_manager_init(GPluginManager *manager) {
	manager->paths = g_queue_new();

	/* the plugins hashtable is keyed on a plugin id and holds a GSList of all
	 * plugins that share that id.
	 */
	manager->plugins =
		g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	/* the filename view is hash table keyed on the filename of the plugin with
	 * a value of the plugin itself.
	 */
	manager->plugins_filename_view =
		g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_object_unref);

	/* The loaders hash table is keyed on the supported extensions of the
	 * loader.  Which means that a loader that supports multiple extensions
	 * will be in the table multiple times.
	 *
	 * We deal with collisions by using a GSList for the value which will hold
	 * references to instances of the actual loaders.
	 *
	 * Storing this in this method allows up to quickly figure out which loader
	 * to use by the filename and helps us to avoid iterating the loaders table
	 * again and again.
	 */
	manager->loaders =
		g_hash_table_new_full(gplugin_manager_str_hash, g_str_equal,
		g_free, NULL);
}

/******************************************************************************
 * Private API
 *****************************************************************************/
void
gplugin_manager_private_init(void) {
	instance = g_object_new(GPLUGIN_TYPE_MANAGER, NULL);

	gplugin_manager_register_loader(GPLUGIN_TYPE_NATIVE_LOADER);

	dependency_regex = g_regex_new(dependency_pattern, 0, 0, NULL);
}

void
gplugin_manager_private_uninit(void) {
	g_regex_unref(dependency_regex);

	g_object_unref(G_OBJECT(instance));
}

/******************************************************************************
 * API
 *****************************************************************************/

/**
 * gplugin_manager_append_path:
 * @path: A path to add to the end of the plugin search paths
 *
 * Adds @path to the end of the list of paths to search for plugins.
 */
void
gplugin_manager_append_path(const gchar *path) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);

	if(klass && klass->append_path)
		klass->append_path(manager, path);
}

/**
 * gplugin_manager_prepend_path:
 * @path: A path to add to the beginning of the plugin search paths
 *
 * Adds @path to the beginning of the list of paths to search for plugins.
 */
void
gplugin_manager_prepend_path(const gchar *path) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);

	if(klass && klass->prepend_path)
		klass->prepend_path(manager, path);
}

/**
 * gplugin_manager_remove_path:
 * @path: A path to remove from the plugin search paths
 *
 * Removes @path from the list of paths to search for plugins.
 */
void
gplugin_manager_remove_path(const gchar *path) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);

	if(klass && klass->remove_path)
		klass->remove_path(manager, path);
}

/**
 * gplugin_manager_remove_paths:
 *
 * Clears all paths that are set to search for plugins.
 */
void
gplugin_manager_remove_paths(void) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);

	if(klass && klass->remove_paths)
		klass->remove_paths(manager);
}

/**
 * gplugin_manager_add_default_paths:
 *
 * Adds the path that GPlugin was installed to to the plugin search path, as
 * well as ${XDG_CONFIG_HOME}/gplugin.
 */
void
gplugin_manager_add_default_paths(void) {
	gchar *path;

	path = g_build_filename(PREFIX, LIBDIR, "gplugin", NULL);
	gplugin_manager_prepend_path(path);
	g_free(path);

	path = g_build_filename(g_get_user_config_dir(), "gplugin", NULL);
	gplugin_manager_prepend_path(path);
	g_free(path);
}


/**
 * gplugin_manager_add_app_paths:
 * @prefix: The installation prefix for the application.
 * @appname: The name of the application whose paths to add.
 *
 * Adds the application installation path for @appname.  This will add
 * $prefix/@appname/plugins to the list as well as
 * ${XDG_CONFIG_HOME}/@appname/plugins.
 */
void
gplugin_manager_add_app_paths(const gchar *prefix,
                              const gchar *appname)
{
	gchar *path;

	g_return_if_fail(appname != NULL);

	path = g_build_filename(prefix, LIBDIR, appname, NULL);
	gplugin_manager_prepend_path(path);
	g_free(path);

	path = g_build_filename(g_get_user_config_dir(), appname, "plugins", NULL);
	gplugin_manager_prepend_path(path);
	g_free(path);
}

/**
 * gplugin_manager_get_paths:
 *
 * Gets the list of paths which will be search for plugins.
 *
 * Return value: (element-type utf8) (transfer none): list of paths which will
 *               be searched for plugins.  free the list with g_list_free when
 *               done.
 */
GList *
gplugin_manager_get_paths(void) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_val_if_fail(GPLUGIN_IS_MANAGER(manager), NULL);

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->get_paths)
		return klass->get_paths(manager);

	return NULL;
}

/**
 * gplugin_manager_register_loader:
 * @type: #GType of a #GPluginLoader
 *
 * Registers @type as an available loader.
 */
void
gplugin_manager_register_loader(GType type) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->register_loader)
		klass->register_loader(manager, type);
}

/**
 * gplugin_manager_unregister_loader:
 * @type: #GType of a #GPluginLoader
 *
 * Unregisters @type as an available loader.
 */
void
gplugin_manager_unregister_loader(GType type) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->unregister_loader)
		klass->unregister_loader(manager, type);
}

/**
 * gplugin_manager_refresh:
 *
 * Forces a refresh of all plugins found in the search paths.
 */
void
gplugin_manager_refresh(void) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_if_fail(GPLUGIN_IS_MANAGER(manager));

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->refresh)
		klass->refresh(manager);
}

/**
 * gplugin_manager_find_plugins:
 * @id: id string of the plugin to find
 *
 * Finds all plugins matching @id.
 *
 * Return value: (element-type GPlugin.Plugin) (transfer full): A #GSList of
 *               referenced #GPluginPlugin's matching @id.  Call
 *               #gplugin_manager_free_plugin_list on the returned value
 *               when you're done with it.
 */
GSList *
gplugin_manager_find_plugins(const gchar *id) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_val_if_fail(GPLUGIN_IS_MANAGER(manager), NULL);

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->find_plugins)
		return klass->find_plugins(manager, id);

	return NULL;
}

/**
 * gplugin_manager_free_plugin_list:
 * @plugins_list: (element-type GPlugin.Plugin): Returned value from
 *                #gplugin_manager_find_plugins
 *
 * Frees the return value of #gplugin_manager_find_plugins.
 */
void
gplugin_manager_free_plugin_list(GSList *plugins_list) {
	GSList *l = NULL;

	g_return_if_fail(plugins_list != NULL);

	for(l = plugins_list; l; l = l->next) {
		GPluginPlugin *plugin = NULL;

		if(l->data == NULL)
			continue;

		plugin = GPLUGIN_PLUGIN(l->data);

		g_object_unref(G_OBJECT(plugin));
	}

	g_slist_free(plugins_list);
}

/**
 * gplugin_manager_find_plugin:
 * @id: The id of the plugin to find.
 *
 * Finds the first plugin matching @id.  This function uses
 * #gplugin_manager_find_plugins and returns the first plugin in the
 * list.
 *
 * Return value: (transfer full): A referenced #GPluginPlugin instance or NULL
 *               if no plugin matching @id was found.
 */
GPluginPlugin *
gplugin_manager_find_plugin(const gchar *id) {
	GSList *plugins_list = NULL;
	GPluginPlugin *plugin = NULL;

	g_return_val_if_fail(id != NULL, NULL);

	plugins_list = gplugin_manager_find_plugins(id);
	if(plugins_list == NULL)
		return NULL;

	plugin = g_object_ref(G_OBJECT(plugins_list->data));

	gplugin_manager_free_plugin_list(plugins_list);

	return plugin;
}


/**
 * gplugin_manager_load_plugin:
 * @plugin: #GPluginPlugin instance
 * @error: (out): return location for a #GError or null
 *
 * Loads @plugin and all of it's dependencies.  If a dependency can not be
 * loaded, @plugin will not be loaded either.  However, any other plugins that
 * @plugin depends on that were loaded from this call, will not be unloaded.
 *
 * Return value: TRUE if @plugin was loaded successfully or already loaded,
 *               FALSE otherwise.
 */
gboolean
gplugin_manager_load_plugin(GPluginPlugin *plugin, GError **error) {
	GPluginManager *manager = NULL;
	GPluginManagerClass *klass = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);

	manager = GPLUGIN_MANAGER_INSTANCE;
	g_return_val_if_fail(GPLUGIN_IS_MANAGER(manager), FALSE);

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->load_plugin)
		return klass->load_plugin(manager, plugin, error);

	return FALSE;
}

/**
 * gplugin_manager_unload_plugin:
 * @plugin: #GPluginPlugin instance
 * @error: (out): return location for a #GError or null
 *
 * Unloads @plugin.  If @plugin has dependencies, they are not unloaded.
 *
 * Return value: TRUE if @plugin was unloaded successfully or not loaded,
 *               FALSE otherwise.
 */
gboolean
gplugin_manager_unload_plugin(GPluginPlugin *plugin, GError **error) {
	GPluginManager *manager = NULL;
	GPluginManagerClass *klass = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN(plugin), FALSE);

	manager = GPLUGIN_MANAGER_INSTANCE;
	g_return_val_if_fail(GPLUGIN_IS_MANAGER(manager), FALSE);

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->unload_plugin)
		return klass->unload_plugin(manager, plugin, error);

	return FALSE;
}

/**
 * gplugin_manager_list_plugins:
 *
 * Returns a #GList of all plugin id's.  Each id should be queried directly
 * for more information.
 *
 * Return value: (element-type utf8) (transfer full): A #GList of each unique
 *               plugin id.
 */
GList *
gplugin_manager_list_plugins(void) {
	GPluginManager *manager = GPLUGIN_MANAGER_INSTANCE;
	GPluginManagerClass *klass = NULL;

	g_return_val_if_fail(GPLUGIN_IS_MANAGER(manager), NULL);

	klass = GPLUGIN_MANAGER_GET_CLASS(manager);
	if(klass && klass->list_plugins)
		return klass->list_plugins(manager);

	return NULL;
}


/**
 * gplugin_manager_get_instance:
 *
 * Returns a #GObject that is the instance of the plugin manager that is being
 * used.
 *
 * This is provided so that signals can be connected and should not be tinkered
 * with in any way.
 *
 * Return Value: (transfer none): The #GObject that is the instance of the
 *                                plugin manager.
 */
GObject *
gplugin_manager_get_instance(void) {
	if(instance)
		return G_OBJECT(instance);

	return NULL;
}

