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

#include <gplugin/gplugin-core.h>
#include <gplugin/gplugin-version.h>

#include <stdlib.h>

#include <glib/gi18n.h>

/******************************************************************************
 * Globals
 *****************************************************************************/
GRegex *regex = NULL;

static const gchar *version_pattern = "^(?P<major>\\d+)(\\." \
                                      "(?P<minor>\\d+)" \
                                      "(\\.(?P<micro>\\d+)(?P<extra>.*))?)?$";


/******************************************************************************
 * Helpers
 *****************************************************************************/
static void
gplugin_version_lazy_init(void) {
	static volatile gsize init_volatile = 0;

	if(g_once_init_enter(&init_volatile)) {
		GError *error = NULL;

		regex = g_regex_new(version_pattern, G_REGEX_NO_AUTO_CAPTURE,
		                    0, &error);

		if(error) {
			g_warning(_("Failed to initialize the version regex: %s"),
			          (error->message) ? error->message :
			                             _("unknown"));
			g_error_free(error);
		}

		g_once_init_leave(&init_volatile, 1);
	}
}

static gboolean
gplugin_version_parser(const gchar *v, gint *major, gint *minor, gint *micro,
                       gchar **extra, GError **error)
{
	GMatchInfo *info = NULL;
	gboolean matches = FALSE;
	gchar *temp = NULL;

	matches = g_regex_match(regex, v, 0, &info);
	if(!matches) {
		if(error) {
			*error = g_error_new(GPLUGIN_DOMAIN, 0,
			                     _("%s does not match the version regex"), v);
		}

		return FALSE;
	}

	/* grab the major version */
	if(major) {
		temp = g_match_info_fetch_named(info, "major");
		*major = (temp) ? atoi(temp) : 0;
		g_free(temp);
	}

	/* grab the minor version */
	if(minor) {
		temp = g_match_info_fetch_named(info, "minor");
		*minor = (temp) ? atoi(temp) : 0;
		g_free(temp);
	}

	/* grab the micro version */
	if(micro) {
		temp = g_match_info_fetch_named(info, "micro");
		*micro = (temp) ? atoi(temp) : 0;
		g_free(temp);
	}

	/* grab the extra version */
	if(extra)
		*extra = g_match_info_fetch_named(info, "extra");

	return TRUE;
}

/******************************************************************************
 * GPluginVersion API
 *****************************************************************************/
/**
 * GPluginVersionCompareFunc:
 * @v1: The first version to compare
 * @v2: The second version to compare
 * @error: A #GError return address if there are any errors.
 *
 * #GPluginVersionCompareFunc is used to compare two versions of a plugin.  It
 * should return -1 if @v1 is greater than @v2, 0 if @v1 is equal to @v2, and
 * 1 if @v1 is less than @v2.
 *
 * Returns: -1 if @v1 is greater than @v2, 0 if @v1 is equal to @v1, and 1 if
 *          @v1 is less than @v2.
 */

/**
 * gplugin_version_compare:
 * @v1: The first version to compare
 * @v2: The second version to compare
 * @error: A #GError return address if there are any errors.
 *
 * The default #GPluginVersionCompareFunc.  It handles the typical
 * MAJOR.MINOR.MICRO format and ignore any characters after the micro version.
 *
 * Returns: -1 if @v1 is greater than @v2, 0 if @v1 is equal to @v1, and 1 if
 *          @v1 is less than @v2.
 */
gint
gplugin_version_compare(const gchar *v1, const gchar *v2, GError **error) {
	gint v1_maj = 0, v1_min = 0, v1_mic = 0;
	gint v2_maj = 0, v2_min = 0, v2_mic = 0;
	gint t = 0;

	g_return_val_if_fail(v1 != NULL, 1);
	g_return_val_if_fail(v2 != NULL, 2);

	if(regex == NULL)
		gplugin_version_lazy_init();

	/* make sure v1 matches the regex */
	if(!gplugin_version_parser(v1, &v1_maj, &v1_min, &v1_mic, NULL, error))
		return 1;

	/* make sure v2 matches the regex */
	if(!gplugin_version_parser(v2, &v2_maj, &v2_min, &v2_mic, NULL, error))
		return -1;

	/* now figure out if they match */
	t = v2_maj - v1_maj;
	if(t != 0)
		return t;

	t = v2_min - v1_min;
	if(t != 0)
		return t;

	return v2_mic - v1_mic;
}

