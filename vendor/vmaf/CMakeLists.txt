# create custom task to build vmaf lib

find_program(MESON_EXE meson REQUIRED)
if (NOT MESON_EXE)
	message(FATAL_ERROR "meson not found")
else ()
	message(STATUS "meson found: ${MESON_EXE}")
endif ()

find_program(NINJA_EXE ninja REQUIRED)
if (NOT NINJA_EXE)
	message(FATAL_ERROR "ninja not found")
else ()
	message(STATUS "ninja found: ${NINJA_EXE}")
endif ()

find_program(XXD_EXE xxd REQUIRED)
if (NOT XXD_EXE)
	message(FATAL_ERROR "xxd not found")
else ()
	message(STATUS "xxd found: ${XXD_EXE}")
endif ()

find_package(Threads REQUIRED)

# Add target to compile VMAF
add_custom_target(vmaf-compile ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vmaf-compiled
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vmaf-compiled
	COMMAND
		${MESON_EXE}
		--buildtype $<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>
		-D enable_float=true
		${CMAKE_CURRENT_BINARY_DIR}/meson-build
	COMMAND ${NINJA_EXE} -vC ${CMAKE_CURRENT_BINARY_DIR}/meson-build
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/vmaf-compiled
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/repo/libvmaf
)

# Export vmaf target
add_library(vmaf::vmaf UNKNOWN IMPORTED GLOBAL)
add_dependencies(vmaf::vmaf vmaf-compile)
set_target_properties(vmaf::vmaf PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/meson-build/src/libvmaf.a
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/repo/libvmaf/include
	INTERFACE_LINK_LIBRARIES Threads::Threads
)
