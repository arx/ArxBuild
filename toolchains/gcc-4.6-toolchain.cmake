
find_program(GCC_46 NAMES
	gcc-4.6
	gcc-4.6.9
	gcc-4.6.8
	gcc-4.6.7
	gcc-4.6.6
	gcc-4.6.5
	gcc-4.6.4
	gcc-4.6.3
	gcc-4.6.2
	gcc-4.6.1
	gcc-4.6.0
)
mark_as_advanced(GCC_46)
find_program(GXX_46 NAMES
	g++-4.6
	g++-4.6.9
	g++-4.6.8
	g++-4.6.7
	g++-4.6.6
	g++-4.6.5
	g++-4.6.4
	g++-4.6.3
	g++-4.6.2
	g++-4.6.1
	g++-4.6.0
)
mark_as_advanced(GXX_46)

if((NOT GCC_46) OR (NOT GXX_46))
	message(FATAL_ERROR "GCC 4.6 not found (found gcc=\"${GCC_46}\", g++=\"${GXX_46}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_46}")
set(CMAKE_CXX_COMPILER "${GXX_46}")
