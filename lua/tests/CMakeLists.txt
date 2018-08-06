if(BUILD_LUA)

macro(add_lua_gtest target)
	add_executable(${target} ${target}.c)
	target_link_libraries(${target}
		${GLIB_LIBRARIES} ${LUA_LIBRARIES} gplugin
	)
	add_dependencies(${target} gplugin-lua)

	get_target_property(_output_name ${target} RUNTIME_OUTPUT_NAME)
	if(NOT ${_output_name})
		get_target_property(_output_name ${target} LOCATION)
	endif(NOT ${_output_name})

	list(APPEND LUA_TESTS ${_output_name})
endmacro(add_lua_gtest)

add_definitions(
	-DLUA_LOADER_DIR="${CMAKE_BINARY_DIR}/lua"
	-DLUA_PLUGIN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/lua-plugins"
	-DMOONSCRIPT_PLUGIN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/moonscript-plugins"
)

add_lua_gtest(test-lua-loader)
target_link_libraries(test-lua-loader gplugin-loader-tests)

if(MOONSCRIPT_TESTS)
	add_lua_gtest(test-lua-moon-loader)
	target_link_libraries(test-lua-moon-loader gplugin-loader-tests)
endif(MOONSCRIPT_TESTS)

set(GTESTER_LUA_TESTS "${LUA_TESTS}")
set(GTESTER_LUA_LOG "test-gplugin-lua.xml")
set(GTESTER_LUA_JUNIT "test-gplugin-lua-junit.xml")

add_custom_command(
	COMMAND ${GTESTER} -k --verbose -o ${GTESTER_LUA_LOG} ${LUA_TESTS}
	OUTPUT ${GTESTER_LUA_LOG}
	DEPENDS gplugin ${GPLUGIN_GIR_TARGETS} gplugin-lua
	        ${LUA_TESTS} ${CMAKE_CURRENT_SOURCE_DIR}/lua-plugins
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
	COMMAND ${XSLTPROC} -o ${GTESTER_LUA_JUNIT} --nonet
	        ${CMAKE_SOURCE_DIR}/xsl/gtester-junit.xsl
	        ${GTESTER_LUA_LOG}
	OUTPUT ${GTESTER_LUA_JUNIT}
	DEPENDS ${GTESTER_LUA_LOG}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(lua-tests ALL
	DEPENDS ${GTESTER_LUA_LOG} ${GTESTER_LUA_JUNIT} ${LUA_TESTS}
)

endif(BUILD_LUA)

