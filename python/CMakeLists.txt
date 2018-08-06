option(
	BUILD_PYTHON
	"Whether or not to build the Python 3.x plugin loader"
	"On"
)

if(BUILD_PYTHON)
	if(NOT BUILD_GIR)
		message(FATAL_ERROR "Python plugin requires GObject Introspection.")
	endif(NOT BUILD_GIR)

	set(GPLUGIN_PYTHON_SOURCES
		gplugin-python-core.c
		gplugin-python-loader.c
		gplugin-python-plugin.c
		gplugin-python-utils.c
	)

	set(GPLUGIN_PYTHON_HEADERS
		gplugin-python-loader.h
		gplugin-python-plugin.h
		gplugin-python-utils.h
	)

	pkg_check_modules(PYTHON3 REQUIRED python3)

	pkg_check_modules(PYGOBJECT REQUIRED pygobject-3.0>=3.0.0)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}
		${PYTHON3_INCLUDE_DIRS}
		${PYGOBJECT_INCLUDE_DIRS}
	)

	message(STATUS "checking if pygobject works with python3")

	set(GPLUGIN_PYTHON_TEST_PYGOBJECT_INCLUDES)
	foreach(INCLUDE_PATH ${GLIB_INCLUDE_DIRS} ${PYTHON3_INCLUDE_DIRS} ${PYGOBJECT_INCLUDE_DIRS})
		set(GPLUGIN_PYTHON_TEST_PYGOBJECT_INCLUDES "${GPLUGIN_PYTHON_TEST_PYGOBJECT_INCLUDES}\;${INCLUDE_PATH}")
	endforeach(INCLUDE_PATH)

	set(GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARIES)
	foreach(LIBRARY ${GLIB_LDFLAGS} ${PYTHON3_LDFLAGS} ${PYGOBJECT_LDFLAGS})
		set(GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARIES "${GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARIES}\;${LIBRARY}")
	endforeach(LIBRARY)

	set(GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARY_DIRS)
	foreach(LIBRARY_DIR ${GLIB_LIBRARY_DIRS} ${PYTHON3_LIBRARY_DIRS} ${PYGOBJECT_LIBRARY_DIRS})
		set(GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARY_DIRS "${GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARY_DIRS}\;${LIBRARY_DIR}")
	endforeach(LIBRARY_DIR)

	# compile our python-gi test program
	try_compile(PYTHON_GI_COMPILE
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/gplugin-python-test-pygobject.c
		CMAKE_FLAGS
			-DINCLUDE_DIRECTORIES:STRING=${GPLUGIN_PYTHON_TEST_PYGOBJECT_INCLUDES}
			-DLINK_LIBRARIES:STRING=${GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARIES}
			-DLINK_DIRECTORIES:STRING=${GPLUGIN_PYTHON_TEST_PYGOBJECT_LIBRARY_DIRS}
		OUTPUT_VARIABLE OUTPUT
		COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/gplugin-python-test-pygobject
	)
	if(NOT ${PYTHON_GI_COMPILE})
		message(STATUS ${OUTPUT})
		message(FATAL_ERROR "Failed to compile the pygobject test")
	endif(NOT ${PYTHON_GI_COMPILE})

	# run the test program now
	execute_process(
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gplugin-python-test-pygobject
		RESULT_VARIABLE PYTHON_GI_FOUND
	)
	if(${PYTHON_GI_FOUND} EQUAL 0)
		message(STATUS "  pygobject works with python3")
	else(${PYTHON_GI_FOUND} EQUAL 0)
		message(FATAL_ERROR "  pygobject does not work with python3")
	endif(${PYTHON_GI_FOUND} EQUAL 0)

	# Now add our libraries
	add_library(gplugin-python MODULE
		${GPLUGIN_PYTHON_SOURCES}
		${GPLUGIN_PYTHON_HEADERS}
	)

	add_library(gplugin-python-static STATIC
		${GPLUGIN_PYTHON_SOURCES}
		${GPLUGIN_PYTHON_HEADERS}
	)

	set_target_properties(gplugin-python PROPERTIES PREFIX "")

	target_link_libraries(gplugin-python
		${PYTHON3_LIBRARIES}
		${PYGOBJECT_LIBRARIES}
		gplugin
	)

	target_link_libraries(gplugin-python-static
		${PYTHON3_LIBRARIES}
		${PYGOBJECT_LIBRARIES}
		gplugin
	)

	install(TARGETS gplugin-python DESTINATION ${CMAKE_INSTALL_LIBDIR}/gplugin)
endif(BUILD_PYTHON)

if(TESTING_ENABLED)
	add_subdirectory(tests)
endif(TESTING_ENABLED)

