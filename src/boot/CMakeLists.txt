
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

target_compile_options(${PROJECT_NAME} PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
    # 使用的 uefi 版本
    $<$<BOOL:${ENABLE_GNU_EFI}>:
        -DENABLE_GNU_EFI=1
    >
    $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
        -DENABLE_GNU_EFI=0
    >
)

target_link_options(${PROJECT_NAME} PRIVATE 
        ${DEFAULT_LINK_OPTIONS}
    
        # 链接脚本
        $<$<BOOL:${ENABLE_GNU_EFI}>:
                -T ${SCRIPTS_FILE_OUTPUT_DIRECTORY}/gnu-efi/link.ld 
        >
        $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
                -T ${SCRIPTS_FILE_OUTPUT_DIRECTORY}/posix-uefi/link.ld 
        >
)

message("DEFAULT_COMPILE_OPTIONS: ${DEFAULT_COMPILE_OPTIONS}")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${BOOT_ELF_OUTPUT_NAME})
add_header_boot(${PROJECT_NAME})
add_header_kernel(${PROJECT_NAME})
add_header_3rd(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
        # 根据 uefi 链接对应的库
        $<$<BOOL:${ENABLE_GNU_EFI}>:
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/crt0-efi-${TARGET_ARCH}.o
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/reloc_${TARGET_ARCH}.o
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libgnuefi.a
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libefi.a
        >
        $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
                ${posix-uefi_BINARY_DIR}/crt0.o
                ${posix-uefi_BINARY_DIR}/libuefi.a
        >
)
