
find_program(GCC_49 NAMES
	gcc-4.9
	gcc49
	gcc-4.9.9
	gcc-4.9.8
	gcc-4.9.7
	gcc-4.9.6
	gcc-4.9.5
	gcc-4.9.4
	gcc-4.9.3
	gcc-4.9.2
	gcc-4.9.1
	gcc-4.9.0
	gcc-4.9.0-pre9999
)
mark_as_advanced(GCC_49)
find_program(GXX_49 NAMES
	g++-4.9
	g++49
	g++-4.9.9
	g++-4.9.8
	g++-4.9.7
	g++-4.9.6
	g++-4.9.5
	g++-4.9.4
	g++-4.9.3
	g++-4.9.2
	g++-4.9.1
	g++-4.9.0
	g++-4.9.0-pre9999
)
mark_as_advanced(GXX_49)

if((NOT GCC_49) OR (NOT GXX_49))
	message(FATAL_ERROR "GCC 4.9 not found (found gcc=\"${GCC_49}\", g++=\"${GXX_49}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_49}")
set(CMAKE_CXX_COMPILER "${GXX_49}")
