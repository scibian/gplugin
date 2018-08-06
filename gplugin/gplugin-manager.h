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

#ifndef GPLUGIN_MANAGER_H
#define GPLUGIN_MANAGER_H

#include <glib.h>
#include <glib-object.h>

#include <gplugin/gplugin-plugin.h>

G_BEGIN_DECLS

void gplugin_manager_append_path(const gchar *path);
void gplugin_manager_prepend_path(const gchar *path);
void gplugin_manager_remove_path(const gchar *path);
void gplugin_manager_remove_paths(void);

void gplugin_manager_add_default_paths(void);
void gplugin_manager_add_app_paths(const gchar *prefix, const gchar *appname);

GList *gplugin_manager_get_paths(void);

void gplugin_manager_register_loader(GType type);
void gplugin_manager_unregister_loader(GType type);

void gplugin_manager_refresh(void);

GSList *gplugin_manager_find_plugins(const gchar *id);
void gplugin_manager_free_plugin_list(GSList *plugins_list);

GPluginPlugin *gplugin_manager_find_plugin(const gchar *id);

gboolean gplugin_manager_load_plugin(GPluginPlugin *plugin, GError **error);
gboolean gplugin_manager_unload_plugin(GPluginPlugin *plugin, GError **error);

GList *gplugin_manager_list_plugins(void);

GObject *gplugin_manager_get_instance(void);

G_END_DECLS

#endif /* GPLUGIN_MANAGER_H */

