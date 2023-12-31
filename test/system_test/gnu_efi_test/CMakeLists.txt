
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

# 设置项目名与版本
project(
        gnu-efi-test
        VERSION 0.0.1
)

enable_language(CXX)

add_executable(${PROJECT_NAME}_boot.elf
        boot.cpp
        load_elf.cpp
        err.cpp
        graphics.cpp
        memory.cpp
        )

add_header_3rd(${PROJECT_NAME}_boot.elf)

target_compile_options(${PROJECT_NAME}_boot.elf PRIVATE
        -fpermissive -fshort-wchar -Wall -Wextra
        -DGNU_EFI_USE_MS_ABI
        -g -ggdb
        -lgcc -lg++ -lsupc++ -lstdc++
        -fPIC
        )

target_link_options(${PROJECT_NAME}_boot.elf PRIVATE
        -shared -Wl,-Bsymbolic
        -nostartfiles
        -T ${CMAKE_SOURCE_DIR}/src/kernel/arch/${TARGET_ARCH}/link.ld
        )

target_link_libraries(${PROJECT_NAME}_boot.elf PRIVATE
        ${gnu-efi_BINARY_DIR}/gnuefi/crt0-efi-${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/libgnuefi.a
        ${gnu-efi_BINARY_DIR}/lib/libefi.a
        #        ${libcxxrt_BINARY_DIR}/lib/libcxxrt.a
        )

target_compile_options(cxxrt-static PRIVATE
        -fPIC
        )

add_dependencies(${PROJECT_NAME}_boot.elf
        gnu-efi
        cxxrt-static
        )

add_custom_target(${PROJECT_NAME}_boot.efi DEPENDS ${PROJECT_NAME}_boot.elf)
add_custom_command(TARGET ${PROJECT_NAME}_boot.efi
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} ${PROJECT_NAME}_boot.elf ${PROJECT_NAME}_boot.efi
        -S
        -R .comment
        -R .note.gnu.build-id
        -R .gnu.hash
        -R .dynsym
        --target=efi-app-${TARGET_ARCH} --subsystem=10
        COMMENT "Build ${PROJECT_NAME}_boot.efi done."
        )

add_executable(${PROJECT_NAME}_kernel.elf
        main.cpp
        )

add_header_kernel(${PROJECT_NAME}_kernel.elf)

target_compile_options(${PROJECT_NAME}_kernel.elf PRIVATE
        -mno-red-zone -Wall -Wextra
        )

target_link_options(${PROJECT_NAME}_kernel.elf PRIVATE
        -ffreestanding
        #        -T ${CMAKE_SOURCE_DIR}/src/kernel/arch/${TARGET_ARCH}/link.ld
        )

add_custom_target(test_gnu_efi DEPENDS ${PROJECT_NAME}_boot.efi ${PROJECT_NAME}_kernel.elf ovmf)
add_custom_command(TARGET test_gnu_efi
        COMMENT "Run ${PROJECT_NAME} in qemu."
        COMMAND mkdir -p ./image
        COMMAND cp ./${PROJECT_NAME}_boot.efi ./image/
        COMMAND cp ./${PROJECT_NAME}_kernel.elf ./image/
        COMMAND qemu-system-x86_64
        -serial stdio -monitor telnet::2333,server,nowait -net none
        -bios ${ovmf_BINARY_DIR}/OVMF_${TARGET_ARCH}.fd
        -hda fat:rw:./image/
        )

# 在 qemu 中调试
add_custom_target(test_gnu_efi_debug DEPENDS ${PROJECT_NAME}_boot.efi ${PROJECT_NAME}_kernel.elf ovmf
        COMMENT "Debug Kernel in qemu ..."
        COMMAND ${CMAKE_COMMAND} -E echo ${QEMU_FLAGS}
        COMMAND qemu-system-${TARGET_ARCH}
        -m 128M
        -serial stdio -monitor telnet::2333,server,nowait -net none
        -bios ${ovmf_BINARY_DIR}/OVMF_${TARGET_ARCH}.fd
        -hda fat:rw:./image/
        -S
        -gdb ${QEMU_GDB_PORT}
        )
