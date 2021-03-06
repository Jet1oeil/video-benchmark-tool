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

#[=======================================================================[.rst:
FindGLPK
-------

Finds the GLPK libraries.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``GLPK::GLPK``
  The linear/mixed-integer program

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``GLPK_FOUND``
  True if the system has all specified GLPK components.
``GLPK_VERSION``
  The version of the GLPK component which was found.
``GLPK_INCLUDE_DIRS``
  Include directories needed to use GLPK.
``GLPK_LIBRARIES``
  Libraries needed to link to GLPK.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``GLPK_INCLUDE_DIR``
  The directory containing headers of specified components.
``GLPK_LIBRARIES``
  The path to the GLPK libraries.

#]=======================================================================]

# Try to use pkg-config
find_package(PkgConfig)

pkg_check_modules(PC_GLPK QUIET "libglpk")

find_path(GLPK_INCLUDE_DIR
	NAMES glpk.h
	PATHS ${PC_GLPK_INCLUDE_DIRS}
)

find_library(GLPK_LIBRARY
	NAMES glpk
	PATHS ${PC_GLPK_LIBRARY_DIRS}
)

if (GLPK_INCLUDE_DIR AND GLPK_LIBRARY)
	set (GLPK_FOUND "True")

	list(APPEND GLPK_INCLUDE_DIRS "${GLPK_INCLUDE_DIR}")
	list(APPEND GLPK_LIBRARIES "${GLPK_LIBRARY}")
endif ()

# Export target
add_library(GLPK::GLPK UNKNOWN IMPORTED)
set_target_properties(GLPK::GLPK PROPERTIES
	IMPORTED_LOCATION "${GLPK_LIBRARY}"
)

# Handle standard variables (GLPK_FOUND...)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLPK
	REQUIRED_VARS
		GLPK_INCLUDE_DIRS
		GLPK_LIBRARIES
)

mark_as_advanced(
	GLPK_FOUND
	GLPK_INCLUDE_DIRS
	GLPK_LIBRARIES
)
