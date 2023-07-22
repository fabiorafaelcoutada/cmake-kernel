
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        gnu-efi-test
        VERSION 0.0.1
)

enable_language(CXX)

add_executable(${PROJECT_NAME}
        boot.cpp
        )

add_header_3rd(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE
        -fpermissive -mno-red-zone -fshort-wchar -Wall -Wextra
        -DGNU_EFI_USE_MS_ABI
        )

target_link_options(${PROJECT_NAME} PRIVATE
        -nostdlib -shared -Wl,-Bsymbolic
        -T ${CMAKE_SOURCE_DIR}/src/arch/${TARGET_ARCH}/link.ld
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${gnu-efi_BINARY_DIR}/gnuefi/crt0-efi-${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/libgnuefi.a
        ${gnu-efi_BINARY_DIR}/lib/libefi.a
        )

add_custom_target(${PROJECT_NAME}.efi DEPENDS ${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME}.efi
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} ${PROJECT_NAME} ${PROJECT_NAME}.efi
        -g -R .comment -R .gnu_debuglink -R .note.gnu.build-id
        -R .gnu.hash -R .plt -R .rela.plt -R .dynstr -R .dynsym -R .rela.dyn
        -S -R .eh_frame -R .gcc_except_table -R .hash
        --target=efi-app-${TARGET_ARCH} --subsystem=10
        COMMENT "Build ${PROJECT_NAME}.efi done."
        )

add_custom_target(test_gnu_efi DEPENDS ${PROJECT_NAME}.efi)
add_custom_command(TARGET test_gnu_efi
        COMMENT "Run ${PROJECT_NAME} in qemu."
        COMMAND mkdir -p ./image
        COMMAND cp ./*.efi ./image/
        COMMAND qemu-system-x86_64
        -serial stdio -monitor telnet::2333,server,nowait -net none
        -bios ${ovmf_BINARY_DIR}/OVMF_${TARGET_ARCH}.fd
        -hda fat:rw:./image/
        )
