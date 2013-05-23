
find_program(GCC_41 NAMES
	gcc-4.1
	gcc41
	gcc-4.1.9
	gcc-4.1.8
	gcc-4.1.7
	gcc-4.1.6
	gcc-4.1.5
	gcc-4.1.4
	gcc-4.1.3
	gcc-4.1.2
	gcc-4.1.1
	gcc-4.1.0
)
mark_as_advanced(GCC_41)
find_program(GXX_41 NAMES
	g++-4.1
	g++41
	g++-4.1.9
	g++-4.1.8
	g++-4.1.7
	g++-4.1.6
	g++-4.1.5
	g++-4.1.4
	g++-4.1.3
	g++-4.1.2
	g++-4.1.1
	g++-4.1.0
)
mark_as_advanced(GXX_41)

if((NOT GCC_41) OR (NOT GXX_41))
	message(FATAL_ERROR "GCC 4.1 not found (found gcc=\"${GCC_41}\", g++=\"${GXX_41}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_41}")
set(CMAKE_CXX_COMPILER "${GXX_41}")
