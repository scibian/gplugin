include(FindPkgConfig)

macro(pkg_config_variable MODULE VARIABLE OUTPUT_VARIABLE)
	set(_output)

	execute_process(
		COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=${VARIABLE} ${MODULE}
		OUTPUT_VARIABLE _output
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	set(${OUTPUT_VARIABLE} ${_output} CACHE FILEPATH "${MODULE} ${VARIABLE}")
endmacro(pkg_config_variable)

