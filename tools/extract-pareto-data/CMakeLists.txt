add_executable(extract-pareto-data
	local/Data.cpp
	main.cpp
)

target_link_libraries(extract-pareto-data
	PRIVATE
		stdc++fs
)

set_target_properties(extract-pareto-data
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
)

target_include_directories(extract-pareto-data
	PRIVATE
		${CMAKE_SOURCE_DIR}/vendor/json/single_include/nlohmann/
)

if (UNIX)
	target_compile_options(extract-pareto-data
		PRIVATE
			"-Wall" "-Wextra"
			"$<$<CONFIG:DEBUG>:-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>"
	)

	target_link_libraries(extract-pareto-data
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()
