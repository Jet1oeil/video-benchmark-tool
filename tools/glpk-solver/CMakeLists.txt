find_package(GLPK REQUIRED)

add_executable(glpk-solver
	local/LinearSolver.cpp
	main.cpp
)

target_link_libraries(glpk-solver
	PRIVATE
		GLPK::GLPK
)

set_target_properties(glpk-solver
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
)

if (UNIX)
	target_compile_options(glpk-solver
		PRIVATE
			"-Wall" "-Wextra"
			"$<$<CONFIG:DEBUG>:-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>"
	)

	target_link_libraries(glpk-solver
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()
