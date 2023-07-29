
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        ${BOOT_ELF_OUTPUT_NAME}
        VERSION 0.0.1
)

enable_language(CXX)

add_executable(${PROJECT_NAME}
        boot.cpp
        )

# 添加头文件
add_header_boot(${PROJECT_NAME})
add_header_3rd(${PROJECT_NAME})

# 添加依赖
add_dependencies(${PROJECT_NAME}
        ${COMPILE_DEPENDS}
        )

# 添加编译参数
target_compile_options(${PROJECT_NAME} PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        )

# 添加链接参数
target_link_options(${PROJECT_NAME} PRIVATE
        ${DEFAULT_LINK_OPTIONS}
        )

# 添加要链接的库
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${DEFAULT_LINK_LIB}
        )
