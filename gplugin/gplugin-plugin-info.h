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

#ifndef GPLUGIN_PLUGIN_INFO_H
#define GPLUGIN_PLUGIN_INFO_H

#define GPLUGIN_TYPE_PLUGIN_INFO            (gplugin_plugin_info_get_type())
#define GPLUGIN_PLUGIN_INFO(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GPLUGIN_TYPE_PLUGIN_INFO, GPluginPluginInfo))
#define GPLUGIN_PLUGIN_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GPLUGIN_TYPE_PLUGIN_INFO, GPluginPluginInfoClass))
#define GPLUGIN_IS_PLUGIN_INFO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GPLUGIN_TYPE_PLUGIN_INFO))
#define GPLUGIN_IS_PLUGIN_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GPLUGIN_TYPE_PLUGIN_INFO))
#define GPLUGIN_PLUGIN_INFO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GPLUGIN_TYPE_PLUGIN_INFO, GPluginPluginInfoClass))

typedef struct _GPluginPluginInfo           GPluginPluginInfo;
typedef struct _GPluginPluginInfoClass      GPluginPluginInfoClass;

#include <glib.h>
#include <glib-object.h>

#include <gplugin/gplugin-loader.h>
#include <gplugin/gplugin-version.h>

struct _GPluginPluginInfo {
	GInitiallyUnowned gparent;
};

struct _GPluginPluginInfoClass {
	GInitiallyUnownedClass gparent;

	void (*_gplugin_reserved_1)(void);
	void (*_gplugin_reserved_2)(void);
	void (*_gplugin_reserved_3)(void);
	void (*_gplugin_reserved_4)(void);
};

G_BEGIN_DECLS

GType gplugin_plugin_info_get_type(void);

#define gplugin_plugin_info_new(id, abi_version, ...) \
	GPLUGIN_PLUGIN_INFO( \
		g_object_new(GPLUGIN_TYPE_PLUGIN_INFO, \
		             "id", (id), \
		             "abi-version", (abi_version), \
		             __VA_ARGS__) \
	)

const gchar *gplugin_plugin_info_get_id(const GPluginPluginInfo *info);
guint32 gplugin_plugin_info_get_abi_version(const GPluginPluginInfo *info);
gboolean gplugin_plugin_info_get_internal(const GPluginPluginInfo *info);
gboolean gplugin_plugin_info_get_load_on_query(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_name(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_version(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_license_id(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_license_text(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_license_url(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_icon(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_summary(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_description(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_category(const GPluginPluginInfo *info);
const gchar * const *gplugin_plugin_info_get_authors(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_website(const GPluginPluginInfo *info);
const gchar * const *gplugin_plugin_info_get_dependencies(const GPluginPluginInfo *info);
GPluginVersionCompareFunc gplugin_plugin_info_get_version_func(const GPluginPluginInfo *info);
const gchar *gplugin_plugin_info_get_help(const GPluginPluginInfo *info);

G_END_DECLS

#endif /* GPLUGIN_PLUGIN_INFO_H */

