
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        doc
        VERSION 0.0.1
)

find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen NOT FOUND.")
endif()

# 设置输入文件
set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
# 设置输出文件
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# 将输入文件的 @VAR@ 替换成 cmake 变量，保存为输出文件
configure_file(${doxyfile_in} ${doxyfile} @ONLY)

# 创建 target 并通过 VERBATIM 将 cmake 参数传递给 doxygen
add_custom_target(doc
    COMMENT "Generating docs ..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    VERBATIM
)
