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

#include <gplugin/gplugin-plugin-info.h>
#include <gplugin/gplugin-core.h>

#include <gplugin/gplugin-enums.h>
#include <gplugin/gplugin-private.h>

#define GPLUGIN_PLUGIN_INFO_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GPLUGIN_TYPE_PLUGIN_INFO, GPluginPluginInfoPrivate))

/******************************************************************************
 * Structs
 *****************************************************************************/
typedef struct {
	gchar *id;

	gchar *name;

	gchar *version;
	GPluginVersionCompareFunc version_func;

	gchar *license_id;
	gchar *license_text;
	gchar *license_url;

	gchar *icon;

	gchar *summary;
	gchar *description;
	gchar *category;
	gchar **authors;
	gchar *help;
	gchar *website;

	gchar **dependencies;

	guint32 abi_version;
	gboolean internal;
	gboolean load_on_query;

	gboolean bind_local;
} GPluginPluginInfoPrivate;

/******************************************************************************
 * Enums
 *****************************************************************************/
enum {
	PROP_ZERO = 0,
	PROP_ID,
	PROP_ABI_VERSION,
	PROP_INTERNAL,
	PROP_LOQ,
	PROP_BIND_LOCAL,
	PROP_NAME,
	PROP_VERSION,
	PROP_VERSION_FUNC,
	PROP_LICENSE_ID,
	PROP_LICENSE_TEXT,
	PROP_LICENSE_URL,
	PROP_ICON,
	PROP_SUMMARY,
	PROP_DESCRIPTION,
	PROP_CATEGORY,
	PROP_AUTHORS,
	PROP_HELP,
	PROP_WEBSITE,
	PROP_DEPENDENCIES,
	N_PROPERTIES,
};

/******************************************************************************
 * Globals
 *****************************************************************************/
static GObjectClass *parent_class = NULL;

/******************************************************************************
 * Private API
 *****************************************************************************/
static void
gplugin_plugin_info_set_id(GPluginPluginInfo *info, const gchar *id) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->id);
	priv->id = (id) ? g_strdup(id) : NULL;
}

static void
gplugin_plugin_info_set_abi_version(GPluginPluginInfo *info,
                                    guint32 abi_version)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	priv->abi_version = abi_version;
}

static void
gplugin_plugin_info_set_internal(GPluginPluginInfo *info, gboolean internal) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	priv->internal = internal;
}

static void
gplugin_plugin_info_set_load_on_query(GPluginPluginInfo *info, gboolean loq) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	priv->load_on_query = loq;
}

static void
gplugin_plugin_info_set_bind_local(GPluginPluginInfo *info,
                                   gboolean bind_local)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	priv->bind_local = bind_local;
}

static void
gplugin_plugin_info_set_name(GPluginPluginInfo *info, const gchar *name) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->name);
	priv->name = (name) ? g_strdup(name) : NULL;
}

static void
gplugin_plugin_info_set_version(GPluginPluginInfo *info,
                                const gchar *version)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->version);
	priv->version = (version) ? g_strdup(version) : NULL;
}

static void
gplugin_plugin_info_set_version_func(GPluginPluginInfo *info,
                                     GPluginVersionCompareFunc func)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	priv->version_func = func;
}

static void
gplugin_plugin_info_set_license_id(GPluginPluginInfo *info,
                                   const gchar *license_id)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->license_id);
	priv->license_id = (license_id) ? g_strdup(license_id) : NULL;
}


static void
gplugin_plugin_info_set_license_text(GPluginPluginInfo *info,
                                     const gchar *license_text)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->license_text);
	priv->license_text = (license_text) ? g_strdup(license_text) : NULL;
}


static void
gplugin_plugin_info_set_license_url(GPluginPluginInfo *info,
                                    const gchar *license_url)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->license_url);
	priv->license_url = (license_url) ? g_strdup(license_url) : NULL;
}

static void
gplugin_plugin_info_set_icon(GPluginPluginInfo *info, const gchar *icon) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->icon);
	priv->icon = (icon) ? g_strdup(icon) : NULL;
}

static void
gplugin_plugin_info_set_summary(GPluginPluginInfo *info,
                                const gchar *summary)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->summary);
	priv->summary = (summary) ? g_strdup(summary) : NULL;
}

static void
gplugin_plugin_info_set_description(GPluginPluginInfo *info,
                                    const gchar *description)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->description);
	priv->description = (description) ? g_strdup(description) : NULL;
}

static void
gplugin_plugin_info_set_category(GPluginPluginInfo *info,
                                 const gchar *category)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->category);
	priv->category = (category) ? g_strdup(category) : NULL;
}

static void
gplugin_plugin_info_set_authors(GPluginPluginInfo *info,
                                const gchar * const *authors) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_strfreev(priv->authors);

	priv->authors = g_strdupv((gchar **)authors);
}

static void
gplugin_plugin_info_set_help(GPluginPluginInfo *info, const gchar *help)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->help);
	priv->help = (help) ? g_strdup(help) : NULL;
}

static void
gplugin_plugin_info_set_website(GPluginPluginInfo *info,
                                const gchar *website)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_free(priv->website);
	priv->website = (website) ? g_strdup(website) : NULL;
}

static void
gplugin_plugin_info_set_dependencies(GPluginPluginInfo *info,
                                     const gchar * const *dependencies)
{
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	g_strfreev(priv->dependencies);

	priv->dependencies = g_strdupv((gchar **)dependencies);
}

/******************************************************************************
 * Object Stuff
 *****************************************************************************/
static void
gplugin_plugin_info_get_property(GObject *obj, guint param_id, GValue *value,
                                 GParamSpec *pspec)
{
	GPluginPluginInfo *info = GPLUGIN_PLUGIN_INFO(obj);

	switch(param_id) {
		case PROP_ID:
			g_value_set_string(value, gplugin_plugin_info_get_id(info));
			break;
		case PROP_ABI_VERSION:
			g_value_set_uint(value, gplugin_plugin_info_get_abi_version(info));
			break;
		case PROP_INTERNAL:
			g_value_set_boolean(value, gplugin_plugin_info_get_internal(info));
			break;
		case PROP_LOQ:
			g_value_set_boolean(value,
			                    gplugin_plugin_info_get_load_on_query(info));
			break;
		case PROP_BIND_LOCAL:
			g_value_set_boolean(value,
			                    gplugin_plugin_info_get_bind_local(info));
			break;
		case PROP_NAME:
			g_value_set_string(value, gplugin_plugin_info_get_name(info));
			break;
		case PROP_VERSION:
			g_value_set_string(value, gplugin_plugin_info_get_version(info));
			break;
		case PROP_VERSION_FUNC:
			g_value_set_pointer(value,
			                    gplugin_plugin_info_get_version_func(info));
			break;
		case PROP_LICENSE_ID:
			g_value_set_string(value,
			                   gplugin_plugin_info_get_license_id(info));
			break;
		case PROP_LICENSE_TEXT:
			g_value_set_string(value,
			                   gplugin_plugin_info_get_license_text(info));
			break;
		case PROP_LICENSE_URL:
			g_value_set_string(value,
			                   gplugin_plugin_info_get_license_url(info));
			break;
		case PROP_ICON:
			g_value_set_string(value, gplugin_plugin_info_get_icon(info));
			break;
		case PROP_SUMMARY:
			g_value_set_string(value, gplugin_plugin_info_get_summary(info));
			break;
		case PROP_DESCRIPTION:
			g_value_set_string(value,
			                   gplugin_plugin_info_get_description(info));
			break;
		case PROP_CATEGORY:
			g_value_set_string(value, gplugin_plugin_info_get_category(info));
			break;
		case PROP_AUTHORS:
			g_value_set_boxed(value, gplugin_plugin_info_get_authors(info));
			break;
		case PROP_HELP:
			g_value_set_string(value, gplugin_plugin_info_get_help(info));
			break;
		case PROP_WEBSITE:
			g_value_set_string(value, gplugin_plugin_info_get_website(info));
			break;
		case PROP_DEPENDENCIES:
			g_value_set_boxed(value,
			                  gplugin_plugin_info_get_dependencies(info));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_plugin_info_set_property(GObject *obj, guint param_id,
                                 const GValue *value, GParamSpec *pspec)
{
	GPluginPluginInfo *info = GPLUGIN_PLUGIN_INFO(obj);

	switch(param_id) {
		case PROP_ID:
			gplugin_plugin_info_set_id(info, g_value_get_string(value));
			break;
		case PROP_ABI_VERSION:
			gplugin_plugin_info_set_abi_version(info, g_value_get_uint(value));
			break;
		case PROP_INTERNAL:
			gplugin_plugin_info_set_internal(info, g_value_get_boolean(value));
			break;
		case PROP_LOQ:
			gplugin_plugin_info_set_load_on_query(info,
			                                      g_value_get_boolean(value));
			break;
		case PROP_BIND_LOCAL:
			gplugin_plugin_info_set_bind_local(info,
			                                   g_value_get_boolean(value));
			break;
		case PROP_NAME:
			gplugin_plugin_info_set_name(info, g_value_get_string(value));
			break;
		case PROP_VERSION:
			gplugin_plugin_info_set_version(info, g_value_get_string(value));
			break;
		case PROP_VERSION_FUNC:
			gplugin_plugin_info_set_version_func(info,
			                                     g_value_get_pointer(value));
			break;
		case PROP_LICENSE_ID:
			gplugin_plugin_info_set_license_id(info,
			                                   g_value_get_string(value));
			break;
		case PROP_LICENSE_TEXT:
			gplugin_plugin_info_set_license_text(info,
			                                     g_value_get_string(value));
			break;
		case PROP_LICENSE_URL:
			gplugin_plugin_info_set_license_url(info,
			                                    g_value_get_string(value));
			break;
		case PROP_ICON:
			gplugin_plugin_info_set_icon(info, g_value_get_string(value));
			break;
		case PROP_SUMMARY:
			gplugin_plugin_info_set_summary(info, g_value_get_string(value));
			break;
		case PROP_DESCRIPTION:
			gplugin_plugin_info_set_description(info,
			                                    g_value_get_string(value));
			break;
		case PROP_CATEGORY:
			gplugin_plugin_info_set_category(info, g_value_get_string(value));
			break;
		case PROP_AUTHORS:
			gplugin_plugin_info_set_authors(info, g_value_get_boxed(value));
			break;
		case PROP_HELP:
			gplugin_plugin_info_set_help(info, g_value_get_string(value));
			break;
		case PROP_WEBSITE:
			gplugin_plugin_info_set_website(info, g_value_get_string(value));
			break;
		case PROP_DEPENDENCIES:
			gplugin_plugin_info_set_dependencies(info,
			                                     g_value_get_boxed(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, pspec);
			break;
	}
}

static void
gplugin_plugin_info_finalize(GObject *obj) {
	GPluginPluginInfoPrivate *priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(obj);

	g_free(priv->id);
	g_free(priv->name);
	g_free(priv->version);
	g_free(priv->license_id);
	g_free(priv->license_text);
	g_free(priv->license_url);
	g_free(priv->icon);
	g_free(priv->summary);
	g_free(priv->description);
	g_strfreev(priv->authors);
	g_free(priv->help);
	g_free(priv->website);
	g_strfreev(priv->dependencies);

	G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gplugin_plugin_info_class_init(GPluginPluginInfoClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);

	g_type_class_add_private(klass, sizeof(GPluginPluginInfoPrivate));

	obj_class->get_property = gplugin_plugin_info_get_property;
	obj_class->set_property = gplugin_plugin_info_set_property;
	obj_class->finalize = gplugin_plugin_info_finalize;

	/* properties */
	/**
	 * GPluginPluginInfo:id:
	 *
	 * The id of the plugin.
	 *
	 * While not required, the recommended convention is to use the following
	 * format: <application or library>/<name of the plugin>.
	 *
	 * For example, the python loader in GPlugin has an id of
	 * "gplugin/python-plugin-loader".
	 */
	g_object_class_install_property(obj_class, PROP_ID,
		g_param_spec_string("id", "id",
		                    "The ID of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:abi-version:
	 *
	 * The GPlugin ABI version that the plugin was compiled against.
	 *
	 * GPlugin only uses the first byte (0xff000000) of this value.  The
	 * remaining 3 bytes are available for the application to use.
	 *
	 * Take the following example from an application:
	 *
	 *  #define ABI_VERSION (GPLUGIN_NATIVE_ABI_VERSION |
	 *                       (APPLICATION_MAJOR_VERSION << 8) |
	 *                       (APPLICATION_MINOR_VERSION))
	 *
	 * The application here uses the thrid and fourth bytes, but could use
	 * the second as well.
	 */
	g_object_class_install_property(obj_class, PROP_ABI_VERSION,
		g_param_spec_uint("abi-version", "abi_version",
		                  "The ABI version of the plugin",
		                  0, G_MAXUINT32, 0,
		                  G_PARAM_READWRITE |
		                  G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:internal:
	 *
	 * Whether or not the plugin is considered an "internal" plugin.
	 *
	 * Defaults to FALSE.
	 */
	g_object_class_install_property(obj_class, PROP_INTERNAL,
		g_param_spec_boolean("internal", "internal",
		                     "Whether or not the plugin is an internal plugin",
		                     FALSE,
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
		                     G_PARAM_STATIC_STRINGS));

	/**
	 * GPluginPluginInfo:load-on-query:
	 *
	 * Whether or not the plugin should be loaded when it's queried.
	 *
	 * This is used by the loaders and may be useful to your application as
	 * well.
	 *
	 * Defaults to FALSE.
	 */
	g_object_class_install_property(obj_class, PROP_LOQ,
		g_param_spec_boolean("load-on-query", "load-on-query",
		                     "Whether or not the plugin should be loaded when "
		                     "queried",
		                     FALSE,
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
		                     G_PARAM_STATIC_STRINGS));

	/**
	 * GPluginPluginInfo:bind-local:
	 *
	 * Determines whether the plugin should be have it's symbols bound locally.
	 *
	 * Note: This should only be used by the native plugin loader.
	 */
	g_object_class_install_property(obj_class, PROP_BIND_LOCAL,
		g_param_spec_boolean("bind-local", "bind-local",
		                     "Whether symbols should be bound locally",
		                     FALSE,
		                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
		                     G_PARAM_STATIC_STRINGS));

	/**
	 * GPluginPluginInfo:name:
	 *
	 * The display name of the plugin.  This should be a translated string.
	 */
	g_object_class_install_property(obj_class, PROP_NAME,
		g_param_spec_string("name", "name",
		                    "The name of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:version:
	 *
	 * The version of the plugin.
	 */
	g_object_class_install_property(obj_class, PROP_VERSION,
		g_param_spec_string("version", "version",
		                    "The version of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:version-func:
	 *
	 * The function to use to compare versions of this plugin.
	 */
	g_object_class_install_property(obj_class, PROP_VERSION_FUNC,
		g_param_spec_pointer("version-func", "version-func",
		                     "The function that can compare versions of " \
		                     "this plugin",
		                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                     G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:license-id:
	 *
	 * The short name of the license.
	 *
	 * It is recommended to use the identifier of the license from
	 * http://dep.debian.net/deps/dep5/#license-specification and should be
	 * "Other" for licenses that are not mentioned in DEP5.
	 *
	 * If a plugin has multiple license, they should be separated by a pipe
	 * (|). In the odd case that you have multiple licenses that are used at
	 * the same time, they should be separated by an ampersand (&).
	 */
	g_object_class_install_property(obj_class, PROP_LICENSE_ID,
		g_param_spec_string("license-id", "license-id",
		                    "The license id of the plugin according to SPDX",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:license-text:
	 *
	 * The text of the license for this plugin.  This should only be used when
	 * the plugin is licensed under a license that is not listed in DEP5.
	 */
	g_object_class_install_property(obj_class, PROP_LICENSE_TEXT,
		g_param_spec_string("license-text", "license text",
		                    "The text of the license for the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:license-url:
	 *
	 * The url to the text of the license.  This should primarily only be used
	 * for licenses not listed in DEP5.
	 */
	g_object_class_install_property(obj_class, PROP_LICENSE_URL,
		g_param_spec_string("license-url", "license url",
		                    "The url to the license of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:icon:
	 *
	 * A string representing an icon for the plugin.  The actual use of this
	 * is determined by the application/library using GPlugin.
	 */
	g_object_class_install_property(obj_class, PROP_ICON,
		g_param_spec_string("icon", "icon",
		                    "The file path of the icon for the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:summary:
	 *
	 * A short description of the plugin that can be listed with the name in a
	 * user interface.
	 */
	g_object_class_install_property(obj_class, PROP_SUMMARY,
		g_param_spec_string("summary", "summary",
		                    "The summary of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:description:
	 *
	 * The full description of the plugin that will be used in a "more
	 * information" section in a user interface.
	 */
	g_object_class_install_property(obj_class, PROP_DESCRIPTION,
		g_param_spec_string("description", "description",
		                    "The description of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:category:
	 *
	 * The category of this plugin.
	 *
	 * This property is used to organize plugins into categories in a user
	 * interface.  It is recommended that an application has a well defined
	 * set of categories that plugin authors should use, and put all plugins
	 * that don't match this category into an "Other" category.
	 */
	g_object_class_install_property(obj_class, PROP_CATEGORY,
		g_param_spec_string("category", "category",
		                    "The category of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:authors:
	 *
	 * A gchar ** of the names and email addresses of the authors.
	 *
	 * It is recommended to use the RFC 822, 2822 format of:
	 * "First Last <user@domain.com>" with additional authors separated by a
	 * comma.
	 */
	g_object_class_install_property(obj_class, PROP_AUTHORS,
		g_param_spec_boxed("authors", "authors",
		                   "The authors of the plugin",
		                   G_TYPE_STRV,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                   G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:help:
	 *
	 * The url of the plugin that can be represented in a user interface.
	 */
	g_object_class_install_property(obj_class, PROP_HELP,
		g_param_spec_string("help", "help",
		                    "The help string for the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:website:
	 *
	 * The url of the plugin that can be represented in a user interface.
	 */
	g_object_class_install_property(obj_class, PROP_WEBSITE,
		g_param_spec_string("website", "website",
		                    "The website of the plugin",
		                    NULL,
		                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                    G_PARAM_CONSTRUCT_ONLY));

	/**
	 * GPluginPluginInfo:dependencies:
	 *
	 * A comma separated list of plugin id's that this plugin depends on.
	 */
	g_object_class_install_property(obj_class, PROP_DEPENDENCIES,
		g_param_spec_boxed("dependencies", "dependencies",
		                   "The dependencies of the plugin",
		                   G_TYPE_STRV,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
		                   G_PARAM_CONSTRUCT_ONLY));
}

/******************************************************************************
 * GPluginPlugin API
 *****************************************************************************/
GType
gplugin_plugin_info_get_type(void) {
	static volatile gsize type_volatile = 0;

	if(g_once_init_enter(&type_volatile)) {
		GType type = 0;

		static const GTypeInfo info = {
			.class_size = sizeof(GPluginPluginInfoClass),
			.class_init = (GClassInitFunc)gplugin_plugin_info_class_init,
			.instance_size = sizeof(GPluginPluginInfo),
		};

		type = g_type_register_static(G_TYPE_OBJECT,
		                              "GPluginPluginInfo",
		                              &info, 0);

		g_once_init_leave(&type_volatile, type);
	}

	return type_volatile;
}

/**
 * gplugin_plugin_info_new: (skip)
 * @id: The id of the plugin
 * @abi_version: The GPlugin ABI version that the plugin uses
 * @...: name/value pairs of properties to set, followed by %NULL.
 *
 * Creates a new GPluginPluginInfo instance.
 *
 * Returns: The new GPluginPluginInfo instance.
 */

/**
 * gplugin_plugin_info_get_id:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The id from @info.
 */
const gchar *
gplugin_plugin_info_get_id(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->id;
}

/**
 * gplugin_plugin_info_get_abi_version:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The abi_version from @info.
 */
guint32
gplugin_plugin_info_get_abi_version(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), 0);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->abi_version;
}

/**
 * gplugin_plugin_info_get_internal:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: Whether or not this plugin is considered an internal plugin
 */
gboolean
gplugin_plugin_info_get_internal(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), FALSE);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->internal;
}

/**
 * gplugin_plugin_info_get_load_on_query:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: Whether or not this plugin should be loaded when queried
 */
gboolean
gplugin_plugin_info_get_load_on_query(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), FALSE);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->load_on_query;
}

/**
 * gplugin_plugin_info_get_name:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The name from @info.
 */
const gchar *
gplugin_plugin_info_get_name(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->name;
}

/**
 * gplugin_plugin_info_get_version:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The version from @info.
 */
const gchar *
gplugin_plugin_info_get_version(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->version;
}

/**
 * gplugin_plugin_info_get_version_func: (skip)
 * @info: #GPluginPluginInfo instance
 *
 * Returns: The #GPluginVersionCompareFunc that can compare versions of this
 *          plugins.
 */
GPluginVersionCompareFunc
gplugin_plugin_info_get_version_func(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->version_func;
}

/**
 * gplugin_plugin_info_get_license_id:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The license-id from @info.
 */
const gchar *
gplugin_plugin_info_get_license_id(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->license_id;
}

/**
 * gplugin_plugin_info_get_license_text:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The text of the license from @info.
 */
const gchar *
gplugin_plugin_info_get_license_text(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->license_text;
}

/**
 * gplugin_plugin_info_get_license_url:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The url of the license from @info.
 */
const gchar *
gplugin_plugin_info_get_license_url(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->license_url;
}

/**
 * gplugin_plugin_info_get_icon:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The icon from @info.
 */
const gchar *
gplugin_plugin_info_get_icon(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->icon;
}

/**
 * gplugin_plugin_info_get_summary:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The summary from @info.
 */
const gchar *
gplugin_plugin_info_get_summary(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->summary;
}

/**
 * gplugin_plugin_info_get_description:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The description from @info.
 */
const gchar *
gplugin_plugin_info_get_description(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->description;
}

/**
 * gplugin_plugin_info_get_category:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The category from @info.
 */
const gchar *
gplugin_plugin_info_get_category(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->category;
}

/**
 * gplugin_plugin_info_get_authors:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: (array zero-terminated=1) (transfer none): The authors from
 *  @info.
 */
const gchar * const *
gplugin_plugin_info_get_authors(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return (const gchar * const *)priv->authors;
}

/**
 * gplugin_plugin_info_get_help:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The help from @info.
 */
const gchar *
gplugin_plugin_info_get_help(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->help;
}

/**
 * gplugin_plugin_info_get_website:
 * @info: #GPluginPluginInfo instance
 *
 * Return value: The website from @info.
 */
const gchar *
gplugin_plugin_info_get_website(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->website;
}

/**
 * gplugin_plugin_info_get_dependencies:
 * @info: #GPluginPluginInfo instance
 *
 * Return value:  (array zero-terminated=1) (transfer none): The list of
 *  dependencies from @info.
 */
const gchar * const *
gplugin_plugin_info_get_dependencies(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), NULL);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return (const gchar * const *)priv->dependencies;
}

/**
 * gplugin_plugin_info_get_bind_local:
 * @info: #GPluginPluginInfo instance
 *
 * This function is only used by the native plugin loader.
 *
 * Return value: TRUE if the plugin has requested to be loaded with it's
 *               symbols bound locally, FALSE if they should bind be bound
 *               globally.
 */
gboolean
gplugin_plugin_info_get_bind_local(const GPluginPluginInfo *info) {
	GPluginPluginInfoPrivate *priv = NULL;

	g_return_val_if_fail(GPLUGIN_IS_PLUGIN_INFO(info), FALSE);

	priv = GPLUGIN_PLUGIN_INFO_GET_PRIVATE(info);

	return priv->bind_local;
}

