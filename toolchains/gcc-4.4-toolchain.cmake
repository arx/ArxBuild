
find_program(GCC_44 NAMES
	gcc-4.4.0
	gcc-4.4.1
	gcc-4.4.2
	gcc-4.4.3
	gcc-4.4.4
	gcc-4.4.5
	gcc-4.4.6
	gcc-4.4.7
	gcc-4.4.8
	gcc-4.4.9
)
mark_as_advanced(GCC_44)
find_program(GXX_44 NAMES
	g++-4.4.0
	g++-4.4.1
	g++-4.4.2
	g++-4.4.3
	g++-4.4.4
	g++-4.4.5
	g++-4.4.6
	g++-4.4.7
	g++-4.4.8
	g++-4.4.9
)
mark_as_advanced(GXX_44)

if((NOT GCC_44) OR (NOT GXX_44))
	message(FATAL_ERROR "GCC 4.4 not found (found gcc=\"${GCC_44}\", g++=\"${GXX_44}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_44}")
set(CMAKE_CXX_COMPILER "${GXX_44}")
