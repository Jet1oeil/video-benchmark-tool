cmake_minimum_required(VERSION 3.13)

project(VideoBenchmarkTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if (UNIX)
	add_compile_options(
		"-Wall" "-Wextra"
		"$<$<CONFIG:DEBUG>:-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(video-benchmark-tool
	src/Controller/QBenchmarkConfigController.cpp
	src/View/QBenchmarkConfigView.cpp
	src/View/QMainView.cpp
	src/main.cpp
)

target_link_libraries(video-benchmark-tool
	PRIVATE
		Qt5::Widgets
)

if (UNIX)
	target_link_libraries(video-benchmark-tool
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()
