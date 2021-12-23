cmake_minimum_required(VERSION 3.13)

project(VideoBenchmarkTool VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(FFMPEG COMPONENTS avcodec avformat avutil REQUIRED)

add_subdirectory(vendor/vmaf)

add_executable(video-benchmark-tool
	src/Controller/QBenchmarkConfigController.cpp
	src/Helper/AVCodecHelper.cpp
	src/Helper/AVFormatHelper.cpp
	src/Helper/Log.cpp
	src/Helper/VMAFWrapper.cpp
	src/Process/vmaf/Benchmark.cpp
	src/Process/vmaf/Configuration.cpp
	src/Process/vmaf/Experiment.cpp
	src/View/QBenchmarkConfigView.cpp
	src/View/QMainView.cpp
	src/Types/Codec.cpp
	src/main.cpp
)

target_link_libraries(video-benchmark-tool
	PRIVATE
		Qt5::Widgets
		FFMPEG::avcodec
		FFMPEG::avformat
		FFMPEG::avutil
		vmaf::vmaf
)

target_include_directories(video-benchmark-tool
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		vendor/json/single_include/nlohmann/
)

set_target_properties(video-benchmark-tool
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		AUTOMOC ON
		AUTORCC ON
		AUTOUIC ON
)

if (UNIX)
	target_compile_options(video-benchmark-tool
		PRIVATE
			"-Wall" "-Wextra"
			"$<$<CONFIG:DEBUG>:-DDEBUG;-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>"
	)

	target_link_libraries(video-benchmark-tool
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()

add_subdirectory("tools/")
