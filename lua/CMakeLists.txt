option(
	BUILD_LUA
	"Whether or not to build the Lua plugin loader"
	"On"
)

if(BUILD_LUA)
	if(NOT BUILD_GIR)
		message(FATAL_ERROR "Lua plugin requires GObject Introspection.")
	endif(NOT BUILD_GIR)

	set(GPLUGIN_LUA_SOURCES
		gplugin-lua-core.c
		gplugin-lua-loader.c
		gplugin-lua-plugin.c
	)

	set(GPLUGIN_LUA_HEADERS
		gplugin-lua-loader.h
		gplugin-lua-plugin.h
	)

	set(_LUAS "lua>=5.1.0;lua5.1>=5.1.0;luajit>=2.0.0;lua5.2>=5.2.0")
	foreach(_LUA ${_LUAS})
		if(NOT LUA_FOUND)
			pkg_check_modules(LUA ${_LUA})
		endif(NOT LUA_FOUND)
	endforeach(_LUA)

	if(NOT LUA_FOUND)
		message(FATAL_ERROR "No usable Lua library was found")
	endif(NOT LUA_FOUND)

	include_directories(${LUA_INCLUDE_DIRS})

	message(STATUS "checking for lua module 'lgi'")

	# compile our lua-lgi test program
	try_compile(LUA_LGI_COMPILE
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/gplugin-lua-test-lgi.c
		CMAKE_FLAGS
			-DINCLUDE_DIRECTORIES:STRING=${LUA_INCLUDE_DIRS}
			-DLINK_LIBRARIES:STRING=${LUA_LIBRARIES}
		OUTPUT_VARIABLE OUTPUT
		COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/gplugin-lua-test-lgi
	)
	if(NOT ${LUA_LGI_COMPILE})
		message(STATUS ${OUTPUT})
		message(FATAL_ERROR "Failed to compile the lua-lgi test")
	endif(NOT ${LUA_LGI_COMPILE})

	# run our lua-lgi test program
	execute_process(
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gplugin-lua-test-lgi
		RESULT_VARIABLE LUA_LGI_FOUND
	)

	if(${LUA_LGI_FOUND} EQUAL 0)
		message(STATUS "  found lgi")
	else(${LUA_LGI_FOUND} EQUAL 0)
		message(FATAL_ERROR "  failed to find the 'lgi' lua module")
	endif(${LUA_LGI_FOUND} EQUAL 0)

	option(
		MOONSCRIPT_TESTS
		"Whether or not to run the moonscript tests"
		"Off"
	)

	# now add the library
	add_library(gplugin-lua MODULE
		${GPLUGIN_LUA_SOURCES}
		${GPLUGIN_LUA_HEADERS}
	)

	set_target_properties(gplugin-lua PROPERTIES PREFIX "")

	target_link_libraries(gplugin-lua
		${LUA_LIBRARIES}
		gplugin
	)

	install(TARGETS gplugin-lua DESTINATION ${CMAKE_INSTALL_LIBDIR}/gplugin)
endif(BUILD_LUA)

if(TESTING_ENABLED)
	add_subdirectory(tests)
endif(TESTING_ENABLED)

