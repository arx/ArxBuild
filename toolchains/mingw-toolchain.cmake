
# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Look for MinGW compilers
if(DEFINED MINGW_ROOT)
	set(MinGW_ROOT "${MINGW_ROOT}" CACHE INTERNAL)
else()
	find_path(MinGW_ROOT mingw i686-w64-mingw32
		PATH_SUFFIXES
		i686-pc-mingw32
		i586-pc-mingw32
		i486-pc-mingw32
		i386-pc-mingw32
		i686-mingw32
		i586-mingw32
		i486-mingw32
		i386-mingw32
		mingw32
		i686-w64-mingw32
		i586-w64-mingw32
		i486-w64-mingw32
		i386-w64-mingw32
		PATHS
		/usr
		/usr/local
	)
endif()
mark_as_advanced(MinGW_ROOT)

find_program(MinGW_GCC NAMES
	i686-pc-mingw32-gcc
	i586-pc-mingw32-gcc
	i486-pc-mingw32-gcc
	i386-pc-mingw32-gcc
	i686-mingw32-gcc
	i586-mingw32-gcc
	i486-mingw32-gcc
	i386-mingw32-gcc
	i686-w64-mingw32-gcc
	i586-w64-mingw32-gcc
	i486-w64-mingw32-gcc
	i386-w64-mingw32-gcc
)
mark_as_advanced(MinGW_GCC)
find_program(MinGW_GXX NAMES
	i686-pc-mingw32-g++
	i586-pc-mingw32-g++
	i486-pc-mingw32-g++
	i386-pc-mingw32-g++
	i686-mingw32-g++
	i586-mingw32-g++
	i486-mingw32-g++
	i386-mingw32-g++
	i686-w64-mingw32-g++
	i586-w64-mingw32-g++
	i486-w64-mingw32-g++
	i386-w64-mingw32-g++
)
mark_as_advanced(MinGW_GXX)
find_program(MinGW_RC NAMES
	i686-pc-mingw32-windres
	i586-pc-mingw32-windres
	i486-pc-mingw32-windres
	i386-pc-mingw32-windres
	i686-mingw32-windres
	i586-mingw32-windres
	i486-mingw32-windres
	i386-mingw32-windres
	i686-w64-mingw32-windres
	i586-w64-mingw32-windres
	i486-w64-mingw32-windres
	i386-w64-mingw32-windres
)
mark_as_advanced(MinGW_RC)

if((NOT MinGW_GCC) OR (NOT MinGW_GXX) OR (NOT MinGW_RC) OR (NOT MinGW_ROOT))
	message(FATAL_ERROR "MinGW not found (found gcc=\"${MinGW_GCC}\","
	                    " g++=\"${MinGW_GXX}\" rc=\"${MinGW_RC}\" root=\"${MinGW_ROOT}\")")
endif()

# Which compilers to use for C and C++
set(CMAKE_C_COMPILER "${MinGW_GCC}")
set(CMAKE_CXX_COMPILER "${MinGW_GXX}")
set(CMAKE_RC_COMPILER "${MinGW_RC}")

# Set where the target environment is located
set(CMAKE_FIND_ROOT_PATH "${MinGW_ROOT}")
# Also tell pkg-config because CMake doesn't
set(_MinGW_pkgconfig "${MinGW_ROOT}/usr/lib/pkgconfig")
set(_MinGW_pkgconfig "${MinGW_ROOT}/usr/share/pkgconfig:${_MinGW_pkgconfig}")
set(ENV{PKG_CONFIG_LIBDIR} "${_MinGW_pkgconfig}")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${MinGW_ROOT}")

# Adjust the default behaviour of the find_xxx() commands:
# Search headers and libraries in the target environment,
# search programs in the host environment.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/dxerr/")
