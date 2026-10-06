cmake_minimum_required(VERSION 3.16)
project(kernelforge C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/O2 /W4)
else()
  # -march=native lets the compiler use your CPU's AVX2 instructions.
  add_compile_options(-O2 -march=native -Wall -Wextra)
endif()

add_library(kernels src/transpose.c)
target_include_directories(kernels PUBLIC include)
if(NOT MSVC)
  target_link_libraries(kernels PUBLIC m)
endif()

add_executable(test_transpose tests/test_transpose.c)
target_link_libraries(test_transpose kernels)

add_executable(bench_transpose bench/bench_transpose.c)
target_link_libraries(bench_transpose kernels)

enable_testing()
add_test(NAME transpose COMMAND test_transpose)
