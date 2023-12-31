
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# add_header.cmake for MRNIU/cmake-kernel.
# 将头文件路径添加到 _target 的搜索路径中

function(add_header_boot _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/boot/include)
endfunction()

function(add_header_libc _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/libc/include)
endfunction()

function(add_header_libcxx _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/libcxx/include)
endfunction()

function(add_header_arch _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/arch/include)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/arch/${TARGET_ARCH}/include)
endfunction()

function(add_header_kernel _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/include)
endfunction()

function(add_header_driver _target)
    target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/kernel/driver/include)
endfunction()

function(add_header_3rd _target)
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc)
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc/${TARGET_ARCH})
    target_include_directories(${_target} PRIVATE
            ${gnu-efi_BINARY_DIR}/inc/protocol)
    target_include_directories(${_target} PRIVATE
            ${opensbi_BINARY_DIR}/include)
endfunction()
