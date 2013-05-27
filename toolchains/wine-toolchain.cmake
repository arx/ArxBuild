
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# The name of the target operating system
set(CMAKE_SYSTEM_NAME Linux-wine)

set(_toolchain_C_command   "winegcc")
set(_toolchain_CXX_command "wineg++")

include("${CMAKE_CURRENT_LIST_DIR}/simple-toolchain.cmake")

set(CMAKE_EXECUTABLE_SUFFIX ".exe")

include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/dxerr/")
