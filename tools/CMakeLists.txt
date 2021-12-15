add_executable(generate-graph
	generate-graph.cpp
)
target_include_directories(generate-graph
	PRIVATE
		vendor/json/single_include/nlohmann/
)

set_target_properties(generate-graph
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
)

if (UNIX)
	target_compile_options(generate-graph
		PRIVATE
			"-Wall" "-Wextra"
			"$<$<CONFIG:DEBUG>:-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>"
	)

	target_link_libraries(generate-graph
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()
