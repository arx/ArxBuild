
# Tell cmake that ekopath supports -isystem
set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

set(_toolchain_C_command   "pathcc")
set(_toolchain_CXX_command "pathCC")
set(_toolchain_have_isystem 1      )

include("${CMAKE_CURRENT_LIST_DIR}/simple-toolchain.cmake")
