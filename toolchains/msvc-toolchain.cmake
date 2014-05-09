
# This toolchain file determines the compiler version and architecture used from its
# own filename.
# It must be used as msvc-<ver>-<arch>-toolchain.cmake

# The name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

string(REGEX REPLACE "-toolchain.cmake$" "" _msvc_dir "${CMAKE_CURRENT_LIST_FILE}")
string(REGEX REPLACE "^.*/" "" _msvc_ver "${_msvc_dir}")

# Which compilers to use for C and C++
set(CMAKE_C_COMPILER "${_msvc_dir}/cl")
set(CMAKE_CXX_COMPILER "${_msvc_dir}/cl")
set(CMAKE_RC_COMPILER "${_msvc_dir}/rc")
set(CMAKE_MT_COMPILER "${_msvc_dir}/mt")
# CMake uses 'link' to link static and dynamic libraries - make sure it can be found
set(ENV{PATH} "${_msvc_dir}/:$ENV{PATH}")
# TODO A better solution should be to set CMAKE_LINKER,
# but for some reason CMake ends up clearing that var and using the empty string.
#set(CMAKE_LINKER "${_msvc_dir}/link")

if("${_msvc_dir}" MATCHES "-64$")
	set(CMAKE_LIBRARY_ARCHITECTURE x64)
else()
	set(CMAKE_LIBRARY_ARCHITECTURE x86)
endif()

if(NOT _msvc_included)
set(_msvc_included 1)

set(_msvc_wineroot "${CMAKE_CURRENT_LIST_DIR}/../deps/msvc/drive_c")
get_filename_component(_msvc_wineroot ${_msvc_wineroot} REALPATH)
set(CMAKE_FIND_ROOT_PATH "${_msvc_wineroot}")

# Adjust the default behaviour of the find_xxx() commands:
# Search headers and libraries in the target environment,
# search programs in the host environment.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

file(GLOB _msvc_dirs RELATIVE ${_msvc_wineroot} ${_msvc_wineroot}/Code/*)
foreach(_msvc_dir IN LISTS _msvc_dirs)
	if(EXISTS ${_msvc_wineroot}/${_msvc_dir}/lib OR
	   EXISTS ${_msvc_wineroot}/${_msvc_dir}/bin OR
	   EXISTS ${_msvc_wineroot}/${_msvc_dir}/include)
		list(APPEND CMAKE_SYSTEM_PREFIX_PATH /${_msvc_dir})
	endif()
	if(EXISTS ${_msvc_wineroot}/${_msvc_dir}/${_msvc_ver})
		list(APPEND CMAKE_SYSTEM_PREFIX_PATH /${_msvc_dir}/${_msvc_ver})
	endif()
endforeach()

set(_msvc_paths "${CMAKE_CURRENT_LIST_DIR}/../deps/msvc/${_msvc_ver}-paths.cmake")
get_filename_component(_msvc_paths ${_msvc_paths} REALPATH)
include(${_msvc_paths})

endif(NOT _msvc_included)
