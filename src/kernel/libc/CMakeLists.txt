
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

# 设置项目名与版本
project(
        libc
        VERSION 0.0.1
)

enable_language(C)

# 生成对象库
add_library(${PROJECT_NAME} OBJECT
        ${PROJECT_SOURCE_DIR}/libc.c
)

# 添加头文件
add_header_libc(${PROJECT_NAME})

# 添加编译参数
target_compile_options(${PROJECT_NAME} PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        )

# 添加链接参数
target_link_options(${PROJECT_NAME} PRIVATE
        ${DEFAULT_LINK_OPTIONS}
        )
