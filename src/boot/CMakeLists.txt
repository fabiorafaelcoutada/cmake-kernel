
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
    -DENABLE_GNU_EFI=$<BOOL:${ENABLE_GNU_EFI}>
#     -DEFI_FUNCTION_WRAPPER -DGNU_EFI_USE_MS_ABI -DHAVE_USE_MS_ABI
)

target_link_options(${PROJECT_NAME} PRIVATE
        ${DEFAULT_LINK_OPTIONS}
        # 不链接标准库
        -nostdlib
        # 编译为共享库
        -shared
        # 符号级别绑定
        -Wl,-Bsymbolic
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${BOOT_ELF_OUTPUT_NAME})
add_header_boot(${PROJECT_NAME})
add_header_3rd(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
        # 根据 uefi 链接对应的库
        $<$<BOOL:${ENABLE_GNU_EFI}>:
                # ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gnu-efi/reloc_${TARGET_ARCH}.o
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gnu-efi/crt0-efi-${TARGET_ARCH}.o
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gnu-efi/libgnuefi.a
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gnu-efi/libefi.a
        >
        $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/posix-uefi/crt0.o
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/posix-uefi/libuefi.a
        >
)
