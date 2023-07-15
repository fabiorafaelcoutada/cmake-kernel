
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
enable_language(ASM)

add_executable(${PROJECT_NAME} 
        boot.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

target_link_options(${PROJECT_NAME} PRIVATE 
    -T ${SCRIPTS_FILE_OUTPUT_DIRECTORY}/gnu-efi/link.ld 
    ${DEFAULT_LINK_OPTIONS}
)

message("DEFAULT_COMPILE_OPTIONS: ${DEFAULT_COMPILE_OPTIONS}")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${BOOT_ELF_OUTPUT_NAME})
add_header_boot(${PROJECT_NAME})
add_header_3rd(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${gnu-efi_BINARY_DIR}/gnuefi/crt0-efi-${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/reloc_${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/libgnuefi.a
        ${gnu-efi_BINARY_DIR}/lib/libefi.a
)
