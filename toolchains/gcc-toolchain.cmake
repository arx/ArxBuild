
# This toolchain file determines the compiler version used from its own filename.
# It should be named gcc-<major>.<minor>-toolchain.cmake

if("${CMAKE_CURRENT_LIST_FILE}" MATCHES "^.*/gcc\\-([^/]+)\\-toolchain\\.cmake$")
	# versioned toolchain file
	set(_gcc_suffix "-${CMAKE_MATCH_1}")
else()
	# generic gcc toolchain
	set(_gcc_suffix "")
endif()
string(REGEX REPLACE "[^0-9]" "" _gcc_short_suffix "${_gcc_suffix}")

foreach(lang IN ITEMS C CXX)
	
	if(lang STREQUAL C)
		set(_gcc_command "gcc")
	else()
		set(_gcc_command "g++")
	endif()
	
	find_program(GCC${_gcc_short_suffix}_${lang}_COMPILER NAMES
		"${_gcc_command}${_gcc_suffix}"
		"${_gcc_command}${_gcc_short_suffix}"
		"${_gcc_command}${_gcc_suffix}.9"
		"${_gcc_command}${_gcc_suffix}.8"
		"${_gcc_command}${_gcc_suffix}.7"
		"${_gcc_command}${_gcc_suffix}.6"
		"${_gcc_command}${_gcc_suffix}.5"
		"${_gcc_command}${_gcc_suffix}.4"
		"${_gcc_command}${_gcc_suffix}.3"
		"${_gcc_command}${_gcc_suffix}.2"
		"${_gcc_command}${_gcc_suffix}.1"
		"${_gcc_command}${_gcc_suffix}.0"
		"${_gcc_command}${_gcc_suffix}.0-pre9999"
	)
	mark_as_advanced(GCC${_gcc_short_suffix}_${lang}_COMPILER)
	
	if(NOT GCC${_gcc_short_suffix}_${lang}_COMPILER)
		message(FATAL_ERROR "${_gcc_command}${_gcc_suffix} not found")
	endif()
	
	set(CMAKE_${lang}_COMPILER "${GCC${_gcc_short_suffix}_${lang}_COMPILER}")
	
	unset(_gcc_command)
	
endforeach()

unset(_gcc_suffix)
unset(_gcc_short_suffix)
