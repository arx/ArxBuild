
# which compilers to use for C and C++
set(CMAKE_C_COMPILER "x86_64-bindist-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "x86_64-bindist-linux-gnu-g++")

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-bindist-linux-gnu")

# adjust the default behaviour of the find_xxx() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
