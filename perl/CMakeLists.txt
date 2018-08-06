option(
	BUILD_PERL
	"Whether or not to build the Perl plugin loader"
	"Off"
)

if(BUILD_PERL)
	if(NOT BUILD_GIR)
		message(FATAL_ERROR "Perl plugin requires GObject Introspection.")
	endif(NOT BUILD_GIR)

	find_package(PerlLibs)

	if(PERLLIBS_FOUND)
		set(GPLUGIN_PERL_SOURCES
			gplugin-perl-core.c
			gplugin-perl-loader.c
			gplugin-perl-plugin.c
		)

		set(GPLUGIN_PERL_HEADERS
			gplugin-perl-loader.h
			gplugin-perl-plugin.h
		)

		add_library(gplugin-perl MODULE
			${GPLUGIN_PERL_SOURCES}
			${GPLUGIN_PERL_HEADERS}
		)

		execute_process(COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ccopts
						OUTPUT_VARIABLE PERL_CFLAGS)
		add_definitions(${PERL_EXTRA_C_FLAGS} ${PERL_CFLAGS})

		execute_process(COMMAND ${PERL_EXECUTABLE} -V:lddlflags
		                OUTPUT_VARIABLE PERL_LDFLAGS)
		string(REGEX REPLACE "lddlflags='(.+)'" "\\1" PERL_LDFLAGS ${PERL_LDFLAGS})
		string(STRIP ${PERL_LDFLAGS} PERL_LDFLAGS)
		set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PERL_LDFLAGS}")

		message(STATUS "PERL_CFLAGS " ${PERL_CFLAGS})
		message(STATUS "PERL_LDFLAGS " ${PERL_LDFLAGS})

		set_target_properties(gplugin-perl PROPERTIES PREFIX "")

		include_directories(${PERL_INCLUDE_PATH})
		target_link_libraries(gplugin-perl ${PERL_LIBRARY})

		install(TARGETS gplugin-perl DESTINATION ${CMAKE_INSTALL_LIBDIR}/gplugin)
	else(PERLLIBS_FOUND)
		message(FATAL_ERROR "Failed to find the Perl libraries")
	endif(PERLLIBS_FOUND)
endif(BUILD_PERL)

