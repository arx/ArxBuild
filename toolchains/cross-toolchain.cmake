
# This toolchain file determines the target triple used from its own filename.
# It should be named clang-<target>-toolchain.cmake
#
# Compilers should be installed as <target>-gcc and <target>-g++
# Target environment should be installed at /usr/<target>

if("${CMAKE_CURRENT_LIST_FILE}" MATCHES "^.*/([^/]+)\\-toolchain\\.cmake$")
	set(_cross_target "${CMAKE_MATCH_1}")
else()
	message(FATAL_ERROR "could not determine target triple")
endif()
string(REGEX REPLACE "[^0-9]" "_" _cross_id "${_cross_target}")

# Set the name of the target operating system
if("${_cross_target}" MATCHES "linux")
	set(CMAKE_SYSTEM_NAME "Linux")
elseif("${_cross_target}" MATCHES "freebsd")
	set(CMAKE_SYSTEM_NAME "FreeBSD")
else()
	message(FATAL_ERROR "unsupported os for target ${_cross_target}")
endif()

# Set the compiler binaries to use
foreach(lang IN ITEMS C CXX)
	
	if(lang STREQUAL C)
		set(_cross_command "${_cross_target}-gcc")
	else()
		set(_cross_command "${_cross_target}-g++")
	endif()
	
	find_program(${_cross_id}_${lang}_COMPILER "${_cross_command}")
	mark_as_advanced(${_cross_id}_${lang}_COMPILER)
	
	if(NOT ${_cross_id}_${lang}_COMPILER)
		message(FATAL_ERROR "${_cross_command} not found")
	endif()
	
	set(CMAKE_${lang}_COMPILER "${${_cross_id}_${lang}_COMPILER}")
	
	unset(_cross_command)
	
endforeach()

# Set where the target environment is located
set(CMAKE_FIND_ROOT_PATH "/usr/${_cross_target}")

# Adjust the default behaviour of the find_xxx() commands:
# Search headers and libraries in the target environment,
# search programs in the host environment.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set the FPU type for ARM
if("${_cross_target}" MATCHES "^arm.*-hardfloat-")
	add_definitions(-mfpu=vfp -mfloat-abi=hard)
endif()

unset(_cross_target)
unset(_cross_id)
