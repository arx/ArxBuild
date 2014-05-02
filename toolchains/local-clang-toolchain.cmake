
# This toolchain file determines the compiler version used from its own filename.
# It should be named clang-<ver>-toolchain.cmake

string(REGEX REPLACE "-toolchain.cmake$" "" _clang_dir "${CMAKE_CURRENT_LIST_FILE}")

# Which compilers to use for C and C++
set(CMAKE_C_COMPILER "${_clang_dir}/clang")
set(CMAKE_CXX_COMPILER "${_clang_dir}/clang++")

unset(_clang_dir)
