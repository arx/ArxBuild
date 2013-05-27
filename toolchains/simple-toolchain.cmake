
foreach(lang IN ITEMS C CXX)
	
	find_program(TOOLCHAIN_${_toolchain_${lang}_command}_BINARY
	             "${_toolchain_${lang}_command}")
	mark_as_advanced(TOOLCHAIN_${_toolchain_${lang}_command}_BINARY)
	
	if(NOT TOOLCHAIN_${_toolchain_${lang}_command}_BINARY)
		message(FATAL_ERROR "${_toolchain_${lang}_command} not found")
	endif()
	
	set(CMAKE_${lang}_COMPILER "${TOOLCHAIN_${_toolchain_${lang}_command}_BINARY}")
	
	unset(_toolchain_${lang}_command)
	
endforeach()

if(_toolchain_have_isystem)
	# Tell cmake that the compiler supports -isystem
	set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
	set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()
unset(_toolchain_have_isystem)
