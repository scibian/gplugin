include(CMakeParseArguments)
include(FindPkgConfig)

find_program(HELP2MAN
	NAMES help2man
	DOC "help2man executable"
)

if(NOT HELP2MAN)
	message(FATAL_ERROR "failed to find help2man")
endif(NOT HELP2MAN)

function(help2man _FIRST_ARG)
	set(oneValueArgs TARGET OUTPUT SECTION NAME HELP_OPTION VERSION_OPTION)

	CMAKE_PARSE_ARGUMENTS(_HELP2MAN "" "${oneValueArgs}" "" ${_FIRST_ARG} ${ARGN})

	if(ADD__HELP2MAN_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keys given to help2man(): \"${ADD__HELP2MAN_UNPARSED_ARGUMENTS}\"")
	endif(ADD__HELP2MAN_UNPARSED_ARGUMENTS)

	# validate arguments
	if(NOT _HELP2MAN_TARGET)
		message(FATAL_ERROR "No target specified")
	endif(NOT _HELP2MAN_TARGET)

	if(NOT _HELP2MAN_OUTPUT)
		message(FATAL_ERROR "No output specified")
	endif(NOT _HELP2MAN_OUTPUT)

	if(NOT _HELP2MAN_SECTION)
		set(_HELP2MAN_SECTION 1)
	endif(NOT _HELP2MAN_SECTION)

	# additional options
	set(_HELP2MAN_OPTIONS)
	if(_HELP2MAN_NAME)
		set(_HELP2MAN_OPTIONS ${_HELP2MAN_OPTIONS} -n "${_HELP2MAN_NAME}")
	endif(_HELP2MAN_NAME)

	if(_HELP2MAN_HELP_OPTION)
		set(_HELP2MAN_OPTIONS ${_HELP2MAN_OPTIONS} -h "${_HELP2MAN_HELP_OPTION}")
	endif(_HELP2MAN_HELP_OPTION)

	if(_HELP2MAN_VERSION_OPTION)
		set(_HELP2MAN_OPTIONS ${_HELP2MAN_OPTIONS} -v "${_HELP2MAN_VERSION_OPTION}")
	endif(_HELP2MAN_VERSION_OPTION)

	# now find the executable
	get_target_property(_HELP2MAN_EXEC ${_HELP2MAN_TARGET} RUNTIME_OUTPUT_NAME)
	if(NOT _HELP2MAN_EXEC)
		get_target_property(_HELP2MAN_EXEC ${_HELP2MAN_TARGET} LOCATION)
	endif(NOT _HELP2MAN_EXEC)

	add_custom_target(${_HELP2MAN_OUTPUT} ALL
		COMMAND ${HELP2MAN}
		        -s ${_HELP2MAN_SECTION}
		        -o ${_HELP2MAN_OUTPUT}
		        --no-info --no-discard-stderr
		        ${_HELP2MAN_OPTIONS}
		        ${_HELP2MAN_EXEC}
		DEPENDS ${_HELP2MAN_TARGET}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	set(_HELP2MAN_DIR share/man/)
	if(CMAKE_SYSTEM_NAME MATCHES .*BSD.*)
		set(_HELP2MAN_DIR man/)
	endif(CMAKE_SYSTEM_NAME MATCHES .*BSD.*)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_HELP2MAN_OUTPUT} DESTINATION ${_HELP2MAN_DIR}man${_HELP2MAN_SECTION})
endfunction(help2man)


