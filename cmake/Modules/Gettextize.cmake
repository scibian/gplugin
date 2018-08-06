include(CMakeParseArguments)

###############################################################################
# homebrew hackery
###############################################################################
# homebrew's gettext is keyonly, so we need to append it's path
if(APPLE)
	find_program(BREW brew)
	if(BREW)
		execute_process(COMMAND ${BREW} --prefix gettext
		                OUTPUT_VARIABLE GETTEXT_PREFIX
		                OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(GETTEXT_PREFIX)
			set(GETTEXT_PATH "${GETTEXT_PREFIX}/bin")
		endif(GETTEXT_PREFIX)
	endif(BREW)
endif(APPLE)

###############################################################################
# Dependencies
###############################################################################
find_program(XGETTEXT_EXECUTABLE xgettext ${GETTEXT_PATH})
if(NOT XGETTEXT_EXECUTABLE)
	message(FATAL_ERROR "Failed to find xgettext")
endif(NOT XGETTEXT_EXECUTABLE)

find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt ${GETTEXT_PATH})
if(NOT GETTEXT_MSGFMT_EXECUTABLE)
	message(FATAL_ERROR "Failed to find msgfmt")
endif(NOT GETTEXT_MSGFMT_EXECUTABLE)

find_program(GETTEXT_MSGMERGE_EXECUTABLE msgmerge ${GETTEXT_PATH})
if(NOT GETTEXT_MSGMERGE_EXECUTABLE)
	message(FATAL_ERROR "Failed to find msgmerge")
endif(NOT GETTEXT_MSGMERGE_EXECUTABLE)

set(DEFAULT_XGETTEXT_OPTIONS
	--package-name=${PROJECT_NAME}
	--package-version=${VERSION}
)

if(NOT DEFINED LOCALE_DIR)
	set(LOCALE_DIR ${CMAKE_INSTALL_PREFIX}/share/locale/)
	message(AUTHOR_WARNING "LOCALE_DIR is not set, using ${LOCALE_DIR}")
endif(NOT DEFINED LOCALE_DIR)

if(NOT DEFINED GETTEXT_PACKAGE)
	set(GETTEXT_PACKAGE ${PROJECT_NAME})
	message(AUTHOR_WARNING "GETTEXT_PACKAGE is not set, using ${GETTEXT_PACKAGE}")
endif(NOT DEFINED GETTEXT_PACKAGE)

###############################################################################
# Functions
###############################################################################
function(GETTEXTIZE_POT_FILE _FIRST_ARG)
	set(options SORT NO_DEFAULT_KEYWORDS NO_ESCAPE NO_COMMENTS)
	set(oneValueArgs FILENAME LANGUAGES)
	set(multiValueArgs SOURCES BUILT_SOURCES KEYWORDS)

	cmake_parse_arguments(
		POT "${options}" "${oneValueArgs}" "${multiValueArgs}"
		${_FIRST_ARG} ${ARGN}
	)

	if(POT_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keys given to pot_file(): "
		        "\"${POT_UNPARSED_ARGUMENTS}\"")
	endif(POT_UNPARSED_ARGUMENTS)

	if(NOT POT_FILENAME)
		set(POT_FILENAME ${GETTEXT_PACKAGE}.pot)
		message(AUTHOR_WARNING "No filename specified, using ${POT_FILENAME}")
	endif(NOT POT_FILENAME)

	if(NOT POT_SOURCES)
		message(FATAL_ERROR "No sources specified")
	endif(NOT POT_SOURCES)

	if(NOT POT_NO_ESCAPE)
		set(POT_ESCAPE --escape)
	endif(NOT POT_NO_ESCAPE)

	if(POT_COMMENTS)
		set(POT_COMMENTS "--add-comments=${POT_COMMENTS}")
	endif(POT_COMMENTS)

	if(POT_SORT)
		set(POT_SORT -s)
	endif(POT_SORT)

	# handle keywords
	if(NOT POT_NO_DEFAULT_KEYWORDS)
		list(INSERT POT_KEYWORDS 0 _ N_ C_:1c,2 NC_:1c,2)
	endif(NOT POT_NO_DEFAULT_KEYWORDS)

	if(POT_KEYWORDS)
		set(_keywords)

		foreach(_kw ${POT_KEYWORDS})
			list(APPEND _keywords "--keyword=${_kw}")
		endforeach(_kw)
		set(POT_KEYWORDS ${_keywords})
	endif(POT_KEYWORDS)

	# handle languages
	if(POT_LANGUAGES)
		set(_languages)

		foreach(_lang in ${POT_LANGUAGES})
			list(APPEND _languages "--language=${_lang}")
		endforeach(_lang)
		set(POT_LANGUAGES ${_languages})
	endif(POT_LANGUAGES)

	# now tweak the sources
	set(POT_REAL_BUILT_SOURCES)
	if(POT_BUILT_SOURCES)
		foreach(_item ${POT_BUILT_SOURCES})
			get_source_file_property(_location ${_item} LOCATION)
			list(APPEND POT_REAL_BUILT_SOURCES "${_location}")
		endforeach(_item)
	endif(POT_BUILT_SOURCES)

	add_custom_command(
		OUTPUT ${POT_FILENAME}
		COMMAND ${XGETTEXT_EXECUTABLE}
		        ${POT_SORT} ${POT_KEYWORDS} ${POT_LANGUAGES} ${POT_ESCAPE}
				${POT_COMMENTS}
		        -o ${CMAKE_CURRENT_BINARY_DIR}/${POT_FILENAME}
		        ${POT_SOURCES} ${POT_REAL_BUILT_SOURCES}
		DEPENDS ${POT_SOURCES} ${POT_BUILT_SOURCES}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
	)

	add_custom_target("${POT_FILENAME} potfile" ALL DEPENDS ${POT_FILENAME})
endfunction(GETTEXTIZE_POT_FILE)

function(GETTEXTIZE_TRANSLATIONS)
	set(options UPDATE)
	set(oneValueArgs)
	set(multiValueArgs)

	CMAKE_PARSE_ARGUMENTS(
		PO "${options}" "${oneValueArgs}" "${multiValueArgs}"
		${ARGV1} ${ARGN}
	)

	if(PO_UNPARSED_ARGUMENTS)
		message(
			FATAL_ERROR "Unknown keys given to GETTEXTIZE_TRANSLATIONS(): "
			"\"${PO_UNPARSED_ARGUMENTS}\""
		)
	endif(PO_UNPARSED_ARGUMENTS)

	file(GLOB _pos *.po)
	set(_gmos)

	foreach(_po ${_pos})
		set(_update)
		if(PO_UPDATE)
			set(_update COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_po} ${_po})
		endif(PO_UPDATE)

		get_filename_component(_base ${_po} NAME_WE)

		set(_install_dir ${LOCALE_DIR}/${_base}/LC_MESSAGES/)
		set(_install_file ${GETTEXT_DOMAIN}.mo)

		set(_gmo ${_base}.gmo)
		list(APPEND _gmos ${_gmo})

		add_custom_command(
			OUTPUT ${_gmo}
			${_update}
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmo} ${_po}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${_po}
		)

		install(
			FILES ${CMAKE_CURRENT_BINARY_DIR}/${_gmo}
			DESTINATION ${_install_dir}
			RENAME ${_install_file}
		)
	endforeach(_po)

	add_custom_target("translations" ALL DEPENDS ${_gmos})
endfunction(GETTEXTIZE_TRANSLATIONS)

