cmake_minimum_required(VERSION 3.13)

project(VMAF VERSION 2.3.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(repo/libvmaf/include repo/libvmaf/src ${CMAKE_CURRENT_BINARY_DIR})

execute_process(
	COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	OUTPUT_VARIABLE VCS_TAG
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set config variables
# Detect arch family
set(ARCH_X86 0)
set(ARCH_X86_32 0)
set(ARCH_X86_64 0)

if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^x86")
	set(ARCH_X86 1)

	if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
		set (ARCH_X86_64 1)
	elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86")
		set (ARCH_X86_32 1)
	endif ()
endif ()

# Mandatory for 32 bits builds
set(HAVE_ASM 0)
set(HAVE_AVX512 0)

# Use built-in models
set(VMAF_BUILT_IN_MODELS 1)
set(VMAF_FLOAT_FEATURES 1)

# Set version variables
set(VMAF_API_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VMAF_API_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VMAF_API_VERSION_PATCH ${PROJECT_VERSION_PATCH})

configure_file(repo/libvmaf/include/vcs_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/vcs_version.h)
configure_file(repo/libvmaf/include/libvmaf/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_library(vmaf
	repo/libvmaf/src/libvmaf.c
	repo/libvmaf/src/compute_vmaf.c
	repo/libvmaf/src/predict.c
	repo/libvmaf/src/model.c
	repo/libvmaf/src/svm.cpp
	repo/libvmaf/src/picture.c
	repo/libvmaf/src/mem.c
	repo/libvmaf/src/picture.c
	repo/libvmaf/src/output.c
	repo/libvmaf/src/fex_ctx_vector.c
	repo/libvmaf/src/thread_pool.c
	repo/libvmaf/src/dict.c
	repo/libvmaf/src/opt.c
	repo/libvmaf/src/ref.c
	repo/libvmaf/src/read_json_model.c
	repo/libvmaf/src/pdjson.c
	repo/libvmaf/src/log.c
)
