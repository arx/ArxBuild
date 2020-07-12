
# which compilers to use for C and C++

find_program(CMAKE_C_COMPILER "ccc-analyzer" PATHS /opt/llvm-dev/libexec ${CMAKE_PREFIX_PATH}/libexec /usr/libexec)

find_program(CMAKE_CXX_COMPILER "c++-analyzer" PATHS /opt/llvm-dev/libexec ${CMAKE_PREFIX_PATH}/libexec /usr/libexec)
