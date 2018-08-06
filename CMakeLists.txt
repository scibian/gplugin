###############################################################################
# CMake Initialization
###############################################################################
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

cmake_minimum_required(VERSION 2.8.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

if(${CMAKE_VERSION} VERSION_LESS "2.8.3")
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Compatibility)
endif(${CMAKE_VERSION} VERSION_LESS "2.8.3")

if(${CMAKE_VERSION} VERSION_GREATER "3.0.0")
	# allow reading of the LOCATION property
	cmake_policy(SET CMP0026 OLD)

	# don't force target names to match a pattern
	cmake_policy(SET CMP0037 OLD)
endif(${CMAKE_VERSION} VERSION_GREATER "3.0.0")

###############################################################################
# Project Info
###############################################################################
project(gplugin C)

set(GPLUGIN_MAJOR_VERSION 0)
set(GPLUGIN_MINOR_VERSION 27)
set(GPLUGIN_MICRO_VERSION 0)
set(GPLUGIN_EXTRA_VERSION)

set(GPLUGIN_VERSION ${GPLUGIN_MAJOR_VERSION}.${GPLUGIN_MINOR_VERSION}.${GPLUGIN_MICRO_VERSION}${GPLUGIN_EXTRA_VERSION})

set(LOCALE_DIR ${CMAKE_INSTALL_PREFIX}/locale)
add_definitions(-DLOCALEDIR="${LOCALE_DIR}")

###############################################################################
# Options
###############################################################################
option(
	BUILD_GIR
	"Whether or not to build a GObject Introspection type library"
	"On"
)

option(
	NLS
	"Install translation files"
	"On"
)

option(
	TESTING_ENABLED
	"Whether or not to run unit tests while compiling"
	"On"
)

option(
	BUILD_HELP2MAN
	"Whether or not to build man pages from --help output"
	"on"
)

###############################################################################
# Dependencies
###############################################################################
if(APPLE)
	# homebrew stuff
	set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/opt/X11/lib/pkgconfig")

	# look for homebrew, if it's installed theres so more mucking we need to do
	find_program(BREW
		NAMES brew
		DOC "brew executable"
	)
	if(BREW)
		execute_process(
			COMMAND ${BREW} --prefix libffi
			OUTPUT_VARIABLE FFI_PREFIX
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(FFI_PREFIX)
			set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${FFI_PREFIX}/lib/pkgconfig")
		endif(FFI_PREFIX)

		execute_process(
			COMMAND ${BREW} --prefix cairo
			OUTPUT_VARIABLE CAIRO_PREFIX
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		if(CAIRO_PREFIX)
			set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CAIRO_PREFIX}/lib/pkgconfig")
		endif(CAIRO_PREFIX)
	endif(BREW)

	# fink and macports
	set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/sw/lib/pkgconfig:/opt/local/lib/pkgconfig")
endif(APPLE)

include(GNUInstallDirs)
include(FindPkgConfig)
include(PkgConfigVariable)

if(BUILD_HELP2MAN)
	include(help2man)
endif(BUILD_HELP2MAN)

pkg_check_modules(GLIB REQUIRED glib-2.0>=2.34.0 gobject-2.0)

# we separate gmodule out so our test aren't linked to it
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)

pkg_config_variable(glib-2.0 glib_genmarshal GLIB_GENMARSHAL)
pkg_config_variable(glib-2.0 glib_mkenums GLIB_MKENUMS)

if(TESTING_ENABLED)
	find_program(GTESTER
		NAMES gtester
		DOC	"gtester executable"
	)
	mark_as_advanced(GTESTER)

	find_program(XSLTPROC
		NAMES xsltproc
		DOC "xsltproc executable"
	)
	mark_as_advanced(XSLTPROC)
endif(TESTING_ENABLED)

###############################################################################
# NLS
###############################################################################
set(GETTEXT_PACKAGE gplugin)

add_definitions(-DGETTEXT_PACKAGE="${GETTEXT_PACKAGE}")

if(NLS)
	include(Gettextize)
endif(NLS)

###############################################################################
# Build Info
###############################################################################
add_definitions(
	-std=c99
	-g -g3
	-O2
	-DPREFIX="${CMAKE_INSTALL_PREFIX}"
	-DLIBDIR="${CMAKE_INSTALL_LIBDIR}"
	-DGPLUGIN_WEBSITE="http://bitbucket.org/gplugin/main"
	-Wall
	-Werror=format-security
	-Wformat
	-Wextra
)

# check if we're using gcc
if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions(
		-DGPLUGIN_UNUSED=__attribute__\(\(unused\)\)
		-ggdb
	)
else(CMAKE_COMPILER_IS_GNUCC)
	add_definitions(
		-DGPLUGIN_UNUSED=
	)
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}                # for built headers/sources
	${CMAKE_BINARY_DIR}/gplugin        # for gplugin.h
	${GLIB_INCLUDE_DIRS}
	${GMODULE_INCLUDE_DIRS}
)

link_directories(
	${GLIB_LIBRARY_DIRS}
	${GMODULE_LIBRARY_DIRS}
)

###############################################################################
# Subdirectories
###############################################################################
add_subdirectory(gplugin)
add_subdirectory(gplugin-gtk)
add_subdirectory(plugins)
add_subdirectory(po)

add_subdirectory(lua)
add_subdirectory(perl)
add_subdirectory(python)

###############################################################################
# Install stuff
###############################################################################
# documentation
install(FILES ChangeLog INSTALL README HACKING DESTINATION ${CMAKE_INSTALL_DOCDIR})

###############################################################################
# make dist
###############################################################################
set(ARCHIVES
	gplugin-${GPLUGIN_VERSION}.tar.bz2
	gplugin-${GPLUGIN_VERSION}.tar.gz
	gplugin-${GPLUGIN_VERSION}.zip
)

set(SIGNATURES)

foreach(ARCHIVE ${ARCHIVES})
	add_custom_command(
		COMMAND hg archive ${ARCHIVE}
		OUTPUT ${ARCHIVE}
		DEPENDS .
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

	add_custom_command(
		COMMAND gpg --yes -abs ${ARCHIVE}
		OUTPUT ${ARCHIVE}.asc
		DEPENDS . ${ARCHIVE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

	list(APPEND SIGNATURES ${ARCHIVE}.asc)
endforeach(ARCHIVE)

add_custom_target(dist DEPENDS ${ARCHIVES} ${SIGNATURES})

