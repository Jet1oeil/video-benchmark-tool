# Copyright (c) 2022 Jet1oeil
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.13)

project(VideoBenchmarkTool VERSION 1.1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(FFMPEG COMPONENTS avcodec avformat avutil REQUIRED)

add_subdirectory(vendor/vmaf)

add_executable(video-benchmark-tool
	src/CLI/OptionParser.cpp
	src/Controller/QBenchmarkController.cpp
	src/Helper/AVCodecHelper.cpp
	src/Helper/AVFormatHelper.cpp
	src/Helper/Log.cpp
	src/Helper/VMAFWrapper.cpp
	src/Process/vmaf/Benchmark.cpp
	src/Process/vmaf/Configuration.cpp
	src/Process/vmaf/Experiment.cpp
	src/View/QBenchmarkConfigView.cpp
	src/View/QBenchmarkProgressView.cpp
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
		stdc++fs
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
