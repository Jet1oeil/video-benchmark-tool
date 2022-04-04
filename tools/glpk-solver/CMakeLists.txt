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

find_package(GLPK REQUIRED)

add_executable(glpk-solver
	local/Data.cpp
	local/LinearSolver.cpp
	main.cpp
)

target_link_libraries(glpk-solver
	PRIVATE
		GLPK::GLPK
		stdc++fs
)

set_target_properties(glpk-solver
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
)

target_include_directories(glpk-solver
	PRIVATE
		${CMAKE_SOURCE_DIR}/vendor/json/single_include/nlohmann/
)

if (UNIX)
	target_compile_options(glpk-solver
		PRIVATE
			"-Wall" "-Wextra"
			"$<$<CONFIG:DEBUG>:-O0;-g;-fno-omit-frame-pointer;-fsanitize=address>;-DGLPK_DEBUG"
	)

	target_link_libraries(glpk-solver
		PRIVATE
			"$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>"
	)
endif ()
