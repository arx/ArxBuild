
set(_toolchain_C_command   "icc" )
set(_toolchain_CXX_command "icpc")
set(_toolchain_have_isystem 1    )

execute_process(COMMAND ${_toolchain_C_command} --version OUTPUT_VARIABLE _icc_version)
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" _icc_version ${_icc_version})

if(NOT _icc_version VERSION_GREATER 16.0.2)
	
	# icc ≤ 16.0.2 is incompatible with GCC ≥ 6
	
	function(_find_gcc variable command)
		
		foreach(major_version IN ITEMS 5 4)
			
			find_program(${variable} NAMES
				"${command}-${major_version}"
				"${command}${major_version}"
			)
			mark_as_advanced(${variable})
			if(${variable})
				return()
			endif()
			
			foreach(minor_version IN ITEMS 9 8 7 6 5 4 3 2 1 0)
				
				find_program(${variable} NAMES
					"${command}-${major_version}.${minor_version}"
					"${command}-${major_version}.${minor_version}.0"
					"${command}${major_version}${minor_version}"
				)
				mark_as_advanced(${variable})
				if(${variable})
					return()
				endif()
				
			endforeach()
		endforeach()
		
	endfunction()
	
	_find_gcc(_gcc_command "gcc")
	if(_gcc_command)
		add_definitions(-gcc-name=${_gcc_command})
	endif()
	
	_find_gcc(_gxx_command "g++")
	if(_gxx_command)
		add_definitions(-gxx-name=${_gxx_command})
	endif()
	
endif()

include("${CMAKE_CURRENT_LIST_DIR}/simple-toolchain.cmake")
