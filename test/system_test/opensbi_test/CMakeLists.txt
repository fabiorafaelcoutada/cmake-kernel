
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        opensbi-test
        VERSION 0.0.1
)

enable_language(CXX)

add_executable(${PROJECT_NAME}
        boot.cpp
        )

add_header_3rd(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE
        -fpermissive -mno-red-zone -fshort-wchar -Wall -Wextra
        -march=rv64imafdc
        )

target_link_options(${PROJECT_NAME} PRIVATE
        -nostdlib
        )

add_custom_target(test_opensbi DEPENDS ${PROJECT_NAME}.efi)
add_custom_command(TARGET test_opensbi
        COMMENT "Run ${PROJECT_NAME} in qemu."
        COMMAND qemu-system-riscv64
        -serial stdio -monitor telnet::2333,server,nowait -net none
        -machine virt -nographic
        -bios ${opensbi_BINARY_DIR}/
        -kernel ${PROJECT_NAME}
        )
