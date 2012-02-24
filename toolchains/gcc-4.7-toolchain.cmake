
find_program(GCC_47 NAMES
	gcc-4.7
	gcc-4.7.9
	gcc-4.7.8
	gcc-4.7.7
	gcc-4.7.6
	gcc-4.7.5
	gcc-4.7.4
	gcc-4.7.3
	gcc-4.7.2
	gcc-4.7.1
	gcc-4.7.0
	gcc-4.7.0-pre9999
)
mark_as_advanced(GCC_47)
find_program(GXX_47 NAMES
	g++-4.7
	g++-4.7.9
	g++-4.7.8
	g++-4.7.7
	g++-4.7.6
	g++-4.7.5
	g++-4.7.4
	g++-4.7.3
	g++-4.7.2
	g++-4.7.1
	g++-4.7.0
	g++-4.7.0-pre9999
)
mark_as_advanced(GXX_47)

if((NOT GCC_47) OR (NOT GXX_47))
	message(FATAL_ERROR "GCC 4.7 not found (found gcc=\"${GCC_47}\", g++=\"${GXX_47}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_47}")
set(CMAKE_CXX_COMPILER "${GXX_47}")
