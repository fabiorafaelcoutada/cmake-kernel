
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libc)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libcxx)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/boot)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/driver)
