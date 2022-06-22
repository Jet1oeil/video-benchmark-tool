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
FindFFMPEG
-------

Finds the FFMPEG libraries.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``FFMPEG::Component``
  The specified FFMPEG component (avcodec, avfilter...)

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``FFMPEG_FOUND``
  True if the system has all specified FFMPEG components.
``FFMPEG_Component_VERSION``
  The version of the FFMPEG component which was found.
``FFMPEG_INCLUDE_DIRS``
  Include directories needed to use FFMPEG.
``FFMPEG_LIBRARIES``
  Libraries needed to link to FFMPEG.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``FFMPEG_INCLUDE_DIR``
  The directory containing headers of specified components.
``FFMPEG_LIBRARIES``
  The path to the FFMPEG libraries.

#]=======================================================================]


if (NOT FFMPEG_FIND_COMPONENTS)
	message(FATAL_ERROR "At least one FFMPEG module must be specified")
endif ()

# Try to use pkg-config
find_package(PkgConfig)

# Try to find all components
foreach (COMPONENT ${FFMPEG_FIND_COMPONENTS})
	pkg_check_modules(PC_FFMPEG_${COMPONENT} QUIET "lib${COMPONENT}")

	find_path(FFMPEG_${COMPONENT}_INCLUDE_DIR
		NAMES ${COMPONENT}.h
		PATHS ${PC_FFMPEG_${COMPONENT}_INCLUDE_DIRS}
		PATH_SUFFIXES lib${COMPONENT}
	)

	find_library(FFMPEG_${COMPONENT}_LIBRARY
		NAMES ${COMPONENT}
		PATHS ${PC_FFMPEG_${COMPONENT}_LIBRARY_DIRS}
	)

	if (FFMPEG_${COMPONENT}_INCLUDE_DIR AND FFMPEG_${COMPONENT}_LIBRARY)
		set (FFMPEG_${COMPONENT}_FOUND "True")

		list(APPEND FFMPEG_INCLUDE_DIRS "${FFMPEG_${COMPONENT}_INCLUDE_DIR}")
		list(APPEND FFMPEG_LIBRARIES "${FFMPEG_${COMPONENT}_LIBRARY}")
	endif ()

	# Export target
	add_library(FFMPEG::${COMPONENT} UNKNOWN IMPORTED)
	set_target_properties(FFMPEG::${COMPONENT} PROPERTIES
		IMPORTED_LOCATION "${FFMPEG_${COMPONENT}_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_${COMPONENT}_INCLUDE_DIR}/.."
	)
endforeach ()

# Handle standard variables (FFMPEG_FOUND...)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFMPEG
	REQUIRED_VARS
		FFMPEG_INCLUDE_DIRS
		FFMPEG_LIBRARIES
	HANDLE_COMPONENTS
)

mark_as_advanced(
	FFMPEG_FOUND
	FFMPEG_INCLUDE_DIRS
	FFMPEG_LIBRARIES
)
