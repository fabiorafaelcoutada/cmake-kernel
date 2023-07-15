
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        kernel
        VERSION 0.0.1
)

enable_language(CXX)

add_executable(${PROJECT_NAME} main.cpp)
add_header_kernel(${PROJECT_NAME})
