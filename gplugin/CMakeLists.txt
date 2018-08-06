###############################################################################
# Library
###############################################################################

set(GPLUGIN_LD_CURRENT 0)
set(GPLUGIN_LD_REVISION 1)
set(GPLUGIN_LD_AGE 0)
set(GPLUGIN_LD_VERSION ${GPLUGIN_LD_CURRENT}.${GPLUGIN_LD_REVISION}.${GPLUGIN_LD_AGE})

add_definitions(
	-DGPLUGIN_COMPILATION
	-DG_LOG_DOMAIN="GPlugin"
	-DGI_TYPELIB_PATH="${CMAKE_CURRENT_BINARY_DIR}"
)

set(GPLUGIN_HEADERS
	gplugin-core.h
	gplugin-loader.h
	gplugin-manager.h
	gplugin-options.h
	gplugin-plugin.h
	gplugin-plugin-info.h
)

set(GPLUGIN_SOURCES
	gplugin-core.c
	gplugin-plugin.c
	gplugin-loader.c
	gplugin-manager.c
	gplugin-options.c
	gplugin-plugin-info.c
	gplugin-private.c
	gplugin-version.c
)

set(GPLUGIN_PUBLIC_BUILT_HEADERS
	gplugin-enums.h
	gplugin-version.h
)

set(GPLUGIN_PUBLIC_BUILT_SOURCES
	gplugin-enums.c
)

set(GPLUGIN_PRIVATE_HEADERS
	gplugin-file-tree.h
)

set(GPLUGIN_PRIVATE_SOURCES
	gplugin-file-tree.c
)

set(GPLUGIN_PRIVATE_BUILT_HEADERS
	gplugin-marshallers.h
)

set(GPLUGIN_PRIVATE_BUILT_SOURCES
	gplugin-marshallers.c
)

set(GPLUGIN_NATIVE_HEADERS
	gplugin-native-plugin.h
	gplugin-native-loader.h
)

set(GPLUGIN_NATIVE_SOURCES
	gplugin-native-plugin.c
	gplugin-native-loader.c
)

##############################################################################
# Helper Variables
###############################################################################
# don't add gplugin.h to this, EVER!
list(APPEND PUBLIC_HEADERS
	${GPLUGIN_HEADERS} ${GPLUGIN_PUBLIC_BUILT_HEADERS}
)
list(SORT PUBLIC_HEADERS)

list(APPEND PRIVATE_HEADERS
	${GPLUGIN_PRIVATE_HEADERS}
	${GPLUGIN_PRIVATE_BUILT_HEADERS}
	gplugin-private.h
	gplugin-native-private.h
)

###############################################################################
# Configure Files
###############################################################################
configure_file(gplugin-version.h.in gplugin-version.h @ONLY)
configure_file(gplugin.pc.in ${CMAKE_CURRENT_BINARY_DIR}/gplugin.pc @ONLY)
configure_file(gplugin-uninstalled.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/gplugin-uninstalled.pc
               @ONLY)


# build gplugin.h
set(GPLUGIN_H_INCLUDES "")

foreach(HEADER ${PUBLIC_HEADERS})
	set(GPLUGIN_H_INCLUDES "${GPLUGIN_H_INCLUDES}\n#include <gplugin/${HEADER}>")
endforeach(HEADER)

configure_file(gplugin.h.in gplugin.h @ONLY)

# build gplugin-native.h
set(GPLUGIN_NATIVE_H_INCLUDES "")
foreach(HEADER ${GPLUGIN_NATIVE_HEADERS})
	set(GPLUGIN_NATIVE_H_INCLUDES "${GPLUGIN_NATIVE_H_INCLUDES}\n#include <gplugin/${HEADER}>")
endforeach(HEADER)

configure_file(gplugin-native.h.in gplugin-native.h @ONLY)

###############################################################################
# gplugin-enum.[ch] generation
###############################################################################
set(ENUM_HEADERS
	gplugin-plugin.h
)

add_custom_command(
	OUTPUT gplugin-enums.h
	DEPENDS ${ENUM_HEADERS} gplugin-enums.h.tmpl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_MKENUMS} --template gplugin-enums.h.tmpl
		--identifier-prefix GPlugin ${ENUM_HEADERS} >
		${CMAKE_CURRENT_BINARY_DIR}/gplugin-enums.h
)

add_custom_command(
	OUTPUT gplugin-enums.c
	DEPENDS ${ENUM_HEADERS} gplugin-enums.c.tmpl gplugin-enums.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_MKENUMS} --template gplugin-enums.c.tmpl
		--identifier-prefix GPlugin ${ENUM_HEADERS} >
		${CMAKE_CURRENT_BINARY_DIR}/gplugin-enums.c
)

###############################################################################
# gplugin-marshallers.[ch] generation
###############################################################################
add_custom_command(
	OUTPUT gplugin-marshallers.h
	DEPENDS gplugin-marshallers.list
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_GENMARSHAL} --header --prefix=gplugin_marshal gplugin-marshallers.list
		> ${CMAKE_CURRENT_BINARY_DIR}/gplugin-marshallers.h
)

add_custom_command(
	OUTPUT gplugin-marshallers.c
	DEPENDS gplugin-marshallers.list
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_GENMARSHAL} --body --prefix=gplugin_marshal gplugin-marshallers.list
		> ${CMAKE_CURRENT_BINARY_DIR}/gplugin-marshallers.c
)

###############################################################################
# Library target
###############################################################################
add_library(gplugin SHARED
	${GPLUGIN_SOURCES}
	${GPLUGIN_NATIVE_SOURCES}
	${GPLUGIN_PUBLIC_BUILT_SOURCES}
	${GPLUGIN_PRIVATE_SOURCES}
	${GPLUGIN_PRIVATE_BUILT_SOURCES}
	${GPLUGIN_NATIVE_HEADERS}
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
	gplugin.h
	gplugin-native.h
)

set_target_properties(gplugin PROPERTIES
	SOVERSION ${GPLUGIN_LD_CURRENT}
	VERSION ${GPLUGIN_LD_VERSION}
)

target_link_libraries(gplugin
	${GLIB_LIBRARIES}
	${GMODULE_LIBRARIES}
)

# nls
if(NLS)
	gettextize_pot_file(
		SORT
		LANGUAGES C
		SOURCES ${GPLUGIN_SOURCES} ${GPLUGIN_NATIVE_SOURCES} gplugin-query.c
		BUILT_SOURCES ${GPLUGIN_PUBLIC_BUILT_SOURCES}
		FILENAME gplugin.pot
	)
endif(NLS)

###############################################################################
# gplugin-query executable
###############################################################################
add_executable(gplugin-query gplugin-query.c)
target_link_libraries(gplugin-query ${GLIB_LIBRARIES} gplugin)

if(BUILD_HELP2MAN)
	help2man(
		TARGET gplugin-query
		OUTPUT gplugin-query.1
		SECTION 1
		NAME "Query installed plugins"
		HELP_OPTION --help-all
	)
endif(BUILD_HELP2MAN)

###############################################################################
# loader-tests static library
###############################################################################
add_library(gplugin-loader-tests STATIC
	gplugin-loader-tests.c
	gplugin-loader-tests.h
)

target_link_libraries(gplugin-loader-tests gplugin)

###############################################################################
# GObject Introspection
###############################################################################
if(BUILD_GIR)
	if(APPLE)
		message("The GObject Introspection build always fails on OS X, "
		        "skipping")
	else(APPLE)
		include(GObjectIntrospection)

		set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_BINARY_DIR}")

		gobject_introspection(
			FILENAME GPlugin-${GPLUGIN_MAJOR_VERSION}.0.gir
			PACKAGES glib-2.0 gmodule-2.0 gobject-2.0
			LIBRARY gplugin
			QUIET
			SCANNER_ARGS --add-include-path=${CMAKE_CURRENT_SOURCE_DIR}
			             --include=GModule-2.0 --include=GObject-2.0
			COMPILER_ARGS --includedir=${CMAKE_CURRENT_SOURCE_DIR}
			SYMBOL_PREFIXES gplugin
			SOURCES ${GPLUGIN_SOURCES} ${GPLUGIN_HEADERS}
			BUILT_SOURCES ${GPLUGIN_PUBLIC_BUILT_SOURCES}
			              ${GPLUGIN_PUBLIC_BUILT_HEADERS}
		)
		# For loader plugin tests dependencies
		set(GPLUGIN_GIR_TARGETS ${GPLUGIN_GIR_TARGETS} PARENT_SCOPE)
	endif(APPLE)
endif(BUILD_GIR)

###############################################################################
# Install Stuff
###############################################################################
# install the main library
install(TARGETS gplugin DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install gplugin-query
install(TARGETS gplugin-query RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install the single includes into the main directory
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/gplugin.h
	      ${CMAKE_CURRENT_BINARY_DIR}/gplugin-native.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gplugin-1.0
)

# install the normal includes into the gplugin subdirectory
foreach(HEADER ${GPLUGIN_HEADERS} ${GPLUGIN_NATIVE_HEADERS})
	install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gplugin-1.0/gplugin
	)
endforeach(HEADER)

# install the built include into the gplugin subdirectory
foreach(HEADER ${GPLUGIN_PUBLIC_BUILT_HEADERS})
	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/${HEADER}
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gplugin-1.0/gplugin
	)
endforeach(HEADER)

# pkg-config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gplugin.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

###############################################################################
# subdirectories
###############################################################################
if(TESTING_ENABLED)
	add_subdirectory(tests)
endif(TESTING_ENABLED)

