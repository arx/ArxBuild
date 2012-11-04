
# Look for icc compilers
find_program(icc icc)
mark_as_advanced(icc)
find_program(icpc icpc)
mark_as_advanced(icpc)

if((NOT icc) OR (NOT icpc))
	message(FATAL_ERROR "icc not found (found: c compiler \"${icc}\", c++ compiler \"${icpc}\")")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "${icc}")
set(CMAKE_CXX_COMPILER "${icpc}")

# tell cmake that icc supports -isystem
set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
