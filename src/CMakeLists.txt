
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        src
        VERSION 0.0.1
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libcxx)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/arch)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/boot)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/kernel)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/driver)
