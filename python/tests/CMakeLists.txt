if(BUILD_PYTHON)

macro(add_python_gtest target)
	add_executable(${target} ${target}.c)
	target_link_libraries(${target}
		${GLIB_LIBRARIES} ${PYTHON3_LIBRARIES} ${PYGOBJECT_LIBRARIES} gplugin)
	add_dependencies(${target} gplugin-python)

	get_target_property(_output_name ${target} RUNTIME_OUTPUT_NAME)
	if(NOT ${_output_name})
		get_target_property(_output_name ${target} LOCATION)
	endif(NOT ${_output_name})

	list(APPEND PYTHON_TESTS ${_output_name})

	set_property(TARGET ${target} APPEND PROPERTY COMPILE_DEFINITIONS
		PYTHON_LOADER_DIR="${CMAKE_BINARY_DIR}/python"
		PYTHON_PLUGIN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/plugins"
	)

	set_property(TARGET ${target} APPEND PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/python"
		${PYTHON3_INCLUDE_DIRS}
		${PYGOBJECT_INCLUDE_DIRS}
	)
endmacro(add_python_gtest)

list(APPEND GPLUGIN_PYTHON_LOADER gplugin-python)

add_python_gtest(test-python-loader)
target_link_libraries(test-python-loader gplugin-loader-tests)

# we can't see the symbols in gplugin-python externally, so use the static
# version for testing
add_python_gtest(test-python-utils)
add_dependencies(test-python-utils gplugin-python-static)
target_link_libraries(test-python-utils gplugin-python-static)

set(GTESTER_PYTHON_TESTS "${PYTHON_TESTS}")
set(GTESTER_PYTHON_LOG "test-gplugin-python.xml")
set(GTESTER_PYTHON_JUNIT "test-gplugin-python-junit.xml")

add_custom_command(
	COMMAND ${GTESTER} -k --verbose -o ${GTESTER_PYTHON_LOG} ${PYTHON_TESTS}
	OUTPUT ${GTESTER_PYTHON_LOG}
	DEPENDS gplugin ${GPLUGIN_GIR_TARGETS} ${GPLUGIN_PYTHON_LOADER}
	        ${PYTHON_TESTS} ${CMAKE_CURRENT_SOURCE_DIR}/plugins
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
	COMMAND ${XSLTPROC} -o ${GTESTER_PYTHON_JUNIT} --nonet
	        ${CMAKE_SOURCE_DIR}/xsl/gtester-junit.xsl
	        ${GTESTER_PYTHON_LOG}
	OUTPUT ${GTESTER_PYTHON_JUNIT}
	DEPENDS ${GTESTER_PYTHON_LOG}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(python-tests ALL
	DEPENDS ${GTESTER_PYTHON_LOG} ${GTESTER_PYTHON_JUNIT} ${PYTHON_TESTS}
)

endif(BUILD_PYTHON)

