-- Copyright (C) 2011-2014 Gary Kramlich <grim@reaperworld.com>
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, see <http://www.gnu.org/licenses/>.

lgi = require 'lgi'
GPlugin = lgi.GPlugin

export gplugin_query = ->
	return GPlugin.PluginInfo {
		id: "gplugin/moon-dependent-plugin",
		dependencies: {"dependency1", "dependency2"}
	}

export gplugin_load = ->
	return false


export gplugin_unload = ->
	return false
