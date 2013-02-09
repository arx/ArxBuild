
# This toolchain file determines the compiler version and architecture used from its
# own filename.
# It must be used as msvc-<ver>-<arch>.toolchain

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

string(REGEX REPLACE "-toolchain.cmake$" "" MSVCDIR "${CMAKE_CURRENT_LIST_FILE}")

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${MSVCDIR}/cl")
set(CMAKE_CXX_COMPILER "${MSVCDIR}/cl")
set(CMAKE_RC_COMPILER "${MSVCDIR}/rc")
set(CMAKE_MT_COMPILER "${MSVCDIR}/mt")
# CMake uses 'link' to link static and dynamic libraries - make sure it can be found
set(ENV{PATH} "${MSVCDIR}/:$ENV{PATH}")
# TODO A better solution should be to set CMAKE_LINKER,
# but for some reason CMake ends up clearing that var and using the empty string.
#set(CMAKE_LINKER "${MSVCDIR}/link")

string(REGEX REPLACE "^.*-" "" MSVCARCH "${MSVCDIR}")
if(MSVCARCH STREQUAL "64")
	set(CMAKE_LIBRARY_ARCHITECTURE x64)
else()
	set(CMAKE_LIBRARY_ARCHITECTURE x86)
endif()

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/../deps/windows/")
get_filename_component(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} REALPATH)

# adjust the default behaviour of the find_xxx() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
