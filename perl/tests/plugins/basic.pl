# vi:et:ts=4 sw=4 sts=4
# Copyright (C) 2011-2014 Gary Kramlich <grim@reaperworld.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

use strict;

use Glib::Object::Introspection;

Glib::Object::Introspection->setup(basename => "GPlugin", version => "0.0", package=> "GPlugin");

sub gplugin_plugin_query() {
	return GPlugin::PluginInfo->new(
		id => "gplugin-perl/basic-plugin",
		abi_version => 0x01000001,
		name => "basic plugin",
		authors => ("Gary Kramlich <grim\@reaperworld.com>"),
		category => "test",
		version => "version",
		license_id => "license id",
		summary => "summary",
		website => "website",
		description => "description",
	);
}

sub gplugin_plugin_load() {
	my $plugin = shift;

	return 0;
}

sub gplugin_plugin_unload() {
	my $plugin = shift;

	return 0;
}

