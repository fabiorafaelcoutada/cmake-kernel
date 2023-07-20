
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# add_header.cmake for MRNIU/cmake-kernel.
# 将头文件路径添加到 _target 的搜索路径中

function(add_header_libc _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/libc/include)
endfunction()

function(add_header_libcxx _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/libcxx/include)
endfunction()

function(add_header_arch _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/arch/include)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/arch/${TARGET_ARCH}/include)
endfunction()

function(add_header_boot _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/boot/include)
endfunction()

function(add_header_kernel _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/include)
endfunction()

function(add_header_driver _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/driver/include)
endfunction()

function(add_header_3rd _target)
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc)
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc/${TARGET_ARCH})
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc/protocol)
endfunction()
