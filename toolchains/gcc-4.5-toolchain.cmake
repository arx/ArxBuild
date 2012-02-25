
find_program(GCC_45 NAMES
	gcc-4.5
	gcc45
	gcc-4.5.9
	gcc-4.5.8
	gcc-4.5.7
	gcc-4.5.6
	gcc-4.5.5
	gcc-4.5.4
	gcc-4.5.3
	gcc-4.5.2
	gcc-4.5.1
	gcc-4.5.0
)
mark_as_advanced(GCC_45)
find_program(GXX_45 NAMES
	g++-4.5
	g++45
	g++-4.5.9
	g++-4.5.8
	g++-4.5.7
	g++-4.5.6
	g++-4.5.5
	g++-4.5.4
	g++-4.5.3
	g++-4.5.2
	g++-4.5.1
	g++-4.5.0
)
mark_as_advanced(GXX_45)

if((NOT GCC_45) OR (NOT GXX_45))
	message(FATAL_ERROR "GCC 4.5 not found (found gcc=\"${GCC_45}\", g++=\"${GXX_45}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${GCC_45}")
set(CMAKE_CXX_COMPILER "${GXX_45}")
