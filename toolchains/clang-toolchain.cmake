
# This toolchain file determines the compiler version used from its own filename.
# It should be named clang-<ver>-toolchain.cmake

if("${CMAKE_CURRENT_LIST_FILE}" MATCHES "^.*/clang\\-([^/]+)\\-toolchain\\.cmake$")
	# versioned toolchain file
	set(_clang_suffix "-${CMAKE_MATCH_1}")
else()
	# generic clang toolchain
	set(_clang_suffix "")
endif()
string(REGEX REPLACE "[^0-9]" "" _clang_short_suffix "${_clang_suffix}")

foreach(lang IN ITEMS C CXX)
	
	if(lang STREQUAL C)
		set(_clang_command "clang${_clang_suffix}")
	else()
		set(_clang_command "clang++${_clang_suffix}")
	endif()
	
	find_program(Clang${_clang_short_suffix}_${lang}_COMPILER "${_clang_command}")
	mark_as_advanced(Clang${_clang_short_suffix}_${lang}_COMPILER)
	
	if(NOT Clang${_clang_short_suffix}_${lang}_COMPILER)
		message(FATAL_ERROR "${_clang_command} not found")
	endif()
	
	set(CMAKE_${lang}_COMPILER "${Clang${_clang_short_suffix}_${lang}_COMPILER}")
	
	unset(_clang_command)
	
endforeach()

unset(_clang_suffix)
unset(_clang_short_suffix)
