
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

# 设置 doxygen 相关参数
set(DOXYGEN_HAVE_DOT YES)
set(DOXYGEN_DOT_MULTI_TARGETS YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_PROJECT_NAME ${CMAKE_PROJECT_NAME})
set(DOXYGEN_PROJECT_NUMBER ${CMAKE_PROJECT_VERSION})
set(DOXYGEN_PROJECT_BRIEF ${PROJECT_DESCRIPTION})
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_EXCLUDE_PATTERNS */3rd/*, */.vscode/*, */.idea/*, */.github/*, */.git/*, */build*/*, */cmake-/*)
set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# 创建 target 并通过 VERBATIM 将 cmake 参数传递给 doxygen
doxygen_add_docs(doc
        COMMENT "Generating docs at ${PROJECT_BINARY_DIR}/html/index.html ..."
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        )
