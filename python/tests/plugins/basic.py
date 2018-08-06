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

from gi.repository import GPlugin

def gplugin_query():
    return GPlugin.PluginInfo(
        id='gplugin/python-basic-plugin',
        abi_version=0x01020304,
        name='basic plugin',
        authors=['author1'],
        category='test',
        version='version',
        license_id='license',
        summary='summary',
        website='website',
        description='description',
    )

def gplugin_load(plugin):
    return True


def gplugin_unload(plugin):
    return True

