cmake_minimum_required(VERSION 3.7)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CC_FLAGS_RELEASE "-O3")

project(vm C)
add_executable(vm-rwa2-0 vm-rwa2-0.c)
add_executable(vm-rwa2-1 vm-rwa2-1.c)
add_executable(vm-rwa2-2 vm-rwa2-2.c)
add_executable(vm-rwa2-3 vm-rwa2-3.c)
add_executable(vm-rwa2-4 vm-rwa2-4.c)
add_executable(vm-rwb2 vm-rwb2.c)

target_compile_options(vm-rwa2-0 PRIVATE -Wall)
target_compile_options(vm-rwa2-1 PRIVATE -Wall)
target_compile_options(vm-rwa2-2 PRIVATE -Wall)
target_compile_options(vm-rwa2-3 PRIVATE -Wall)
target_compile_options(vm-rwa2-4 PRIVATE -Wall)
target_compile_options(vm-rwb2 PRIVATE -Wall)