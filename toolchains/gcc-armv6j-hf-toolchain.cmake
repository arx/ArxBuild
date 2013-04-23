
# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "armv6j-hardfloat-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "armv6j-hardfloat-linux-gnueabi-g++")

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH "/usr/armv6j-hardfloat-linux-gnueabi")

# adjust the default behaviour of the find_xxx() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_definitions(-mfpu=vfp -mfloat-abi=hard)
