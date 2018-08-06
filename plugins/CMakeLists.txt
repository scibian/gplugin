macro(add_plugin plugin)
	add_library(${plugin} MODULE ${plugin}.c)
	set_target_properties(${plugin} PROPERTIES PREFIX "")
	target_link_libraries(${plugin} gplugin)
	install(TARGETS ${plugin} DESTINATION ${CMAKE_INSTALL_LIBDIR}/gplugin)
endmacro(add_plugin)

add_plugin(gplugin-license-check)

