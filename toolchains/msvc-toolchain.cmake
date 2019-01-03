
# This toolchain file determines the compiler version and architecture used from its
# own filename.
# It must be used as msvc-<ver>-<arch>-toolchain.cmake

# The name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

string(REGEX REPLACE "-toolchain.cmake$" "" _msvc_dir "${CMAKE_CURRENT_LIST_FILE}")
string(REGEX REPLACE "^.*/" "" _msvc_ver "${_msvc_dir}")
string(REGEX REPLACE "^.*\\-" "" _msvc_arch "${_msvc_ver}")
string(REGEX REPLACE "\\-[^\\-]+$" "" _msvc_ver_noarch "${_msvc_ver}")
string(REGEX REPLACE "\\.[0-9]+$" "" _msvc_ver_major_noarch "${_msvc_ver_noarch}")
set(_msvc_ver_major "${_msvc_ver_major_noarch}-${_msvc_arch}")

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

set(CMAKE_LIBRARY_ARCHITECTURE "${_msvc_arch}")
set(CMAKE_C_LIBRARY_ARCHTECTURE ${CMAKE_LIBRARY_ARCHITECTURE})
set(CMAKE_CXX_LIBRARY_ARCHTECTURE ${CMAKE_LIBRARY_ARCHITECTURE})

if(NOT _msvc_included)
set(_msvc_included 1)

set(_msvc_wineroot "${CMAKE_CURRENT_LIST_DIR}/../deps/msvc/drive_c")
get_filename_component(_msvc_wineroot ${_msvc_wineroot} REALPATH)
set(CMAKE_FIND_ROOT_PATH "${_msvc_wineroot}")

# Prevent pkg-config from finding stuff it shouldn't
set(ENV{PKG_CONFIG_LIBDIR} "/dev/null")

# Adjust the default behaviour of the find_xxx() commands:
# Search headers and libraries in the target environment,
# search programs in the host environment.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

file(GLOB _msvc_depdirs RELATIVE ${_msvc_wineroot} "${_msvc_wineroot}/Code/*")
list(SORT _msvc_depdirs)
foreach(_msvc_depdir IN LISTS _msvc_depdirs)
	foreach(_ver IN ITEMS ${_msvc_ver} ${_msvc_ver_noarch} ${_msvc_ver_major} ${_msvc_ver_major_noarch})
		if(EXISTS ${_msvc_wineroot}/${_msvc_depdir}/${_ver})
			list(APPEND CMAKE_SYSTEM_PREFIX_PATH /${_msvc_depdir}/${_ver})
		endif()
	endforeach()
	if(EXISTS ${_msvc_wineroot}/${_msvc_depdir}/lib OR
	   EXISTS ${_msvc_wineroot}/${_msvc_depdir}/bin OR
	   EXISTS ${_msvc_wineroot}/${_msvc_depdir}/include)
		list(APPEND CMAKE_SYSTEM_PREFIX_PATH /${_msvc_depdir})
	endif()
endforeach()

foreach(_ver IN ITEMS ${_msvc_ver} ${_msvc_ver_noarch} ${_msvc_ver_major} ${_msvc_ver_major_noarch})
	set(_msvc_paths "${CMAKE_CURRENT_LIST_DIR}/../deps/msvc/${_ver}-paths.cmake")
	if(EXISTS ${_msvc_paths})
		get_filename_component(_msvc_paths ${_msvc_paths} REALPATH)
		break()
	endif()
endforeach()
include(${_msvc_paths})
if(DEFINED msvc_redist)
	file(GLOB _msvc_redist_dirs RELATIVE ${_msvc_wineroot} "${_msvc_wineroot}/${msvc_redist}/*")
	list(SORT _msvc_redist_dirs)
	foreach(_msvc_redist_dir IN LISTS _msvc_redist_dirs)
		if(IS_DIRECTORY "${_msvc_wineroot}/${_msvc_redist_dir}")
			list(APPEND CMAKE_SYSTEM_LIBRARY_PATH "/${_msvc_redist_dir}")
		endif()
	endforeach()
	unset(_msvc_redist_dirs)
	unset(msvc_redist)
endif()

endif(NOT _msvc_included)
