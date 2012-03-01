
find_program(GCC_43 NAMES
	gcc-4.3
	gcc43
	gcc-4.3.9
	gcc-4.3.8
	gcc-4.3.7
	gcc-4.3.6
	gcc-4.3.5
	gcc-4.3.4
	gcc-4.3.3
	gcc-4.3.2
	gcc-4.3.1
	gcc-4.3.0
)
mark_as_advanced(GCC_43)
find_program(GXX_43 NAMES
	g++-4.3
	g++43
	g++-4.3.9
	g++-4.3.8
	g++-4.3.7
	g++-4.3.6
	g++-4.3.5
	g++-4.3.4
	g++-4.3.3
	g++-4.3.2
	g++-4.3.1
	g++-4.3.0
)
mark_as_advanced(GXX_43)

if((NOT GCC_43) OR (NOT GXX_43))
	message(FATAL_ERROR "GCC 4.3 not found (found gcc=\"${GCC_43}\", g++=\"${GXX_43}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_43}")
set(CMAKE_CXX_COMPILER "${GXX_43}")
