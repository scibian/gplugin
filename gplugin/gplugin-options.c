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

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>

#include <gplugin/gplugin-options.h>

#include <gplugin/gplugin-manager.h>

/******************************************************************************
 * Options
 *****************************************************************************/
static gboolean add_default_paths = TRUE;
static gchar **paths = NULL;

static gboolean
gplugin_options_no_default_paths_cb(GPLUGIN_UNUSED const gchar *n,
                                    GPLUGIN_UNUSED const gchar *v,
                                    GPLUGIN_UNUSED gpointer d,
                                    GPLUGIN_UNUSED GError **e)
{
	add_default_paths = FALSE;

	return TRUE;
}

static GOptionEntry entries[] = {
	{
		"no-default-paths", 'D', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		gplugin_options_no_default_paths_cb,
		N_("Do not search the default plugin paths"),
		NULL,
	}, {
		"path", 'p', 0, G_OPTION_ARG_STRING_ARRAY,
		&paths, N_("Additional path to look for plugins"),
		NULL,
	}, {
		NULL, 0, 0, 0, NULL, NULL, NULL
	},
};

static gboolean
gplugin_options_post_parse_cb(GPLUGIN_UNUSED GOptionContext *ctx,
                              GPLUGIN_UNUSED GOptionGroup *group,
                              GPLUGIN_UNUSED gpointer data,
                              GPLUGIN_UNUSED GError **error)
{
	gint i = 0;

	if(add_default_paths)
		gplugin_manager_add_default_paths();

	if(paths)
		for(i = 0; paths[i]; paths++)
			gplugin_manager_prepend_path(paths[i]);

	return TRUE;
}

/******************************************************************************
 * API
 *****************************************************************************/
/**
 * gplugin_get_option_group:
 *
 * Returns a #GOptionGroup for the commandline arguments recognized by
 * GPlugin.  You should add this option group to your #GOptionContext with
 * g_option_context_add_group(), if you are using g_option_context_parse() to
 * parse your commandline arguments.
 *
 * GPlugin must be initialized before you call this function.
 *
 * Return Value: (transfer full): a #GOptionGroup for the commandline arguments
 *                                recognized by GPlugin.
 */
GOptionGroup *
gplugin_get_option_group(void) {
	GOptionGroup *group = NULL;

	group = g_option_group_new("gplugin", _("GPlugin Options"),
	                           _("Show GPlugin Options"), NULL, NULL);

	g_option_group_set_parse_hooks(group, NULL,
	                               gplugin_options_post_parse_cb);
	g_option_group_add_entries(group, entries);
	g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);

	return group;
}

