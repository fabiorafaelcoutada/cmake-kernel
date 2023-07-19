
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 设置项目名与版本
project(
        cmake-kernel
        VERSION 0.0.1
)

# 禁止原地编译
if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    # 如果你看到这句话，cmake 此时已经在根目录下生成了一些临时文件，你需要删除它们
    # CMakeFiles, CMakeCache.txt
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

# 设置辅助 cmake 脚本路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 导入配置
include(config)
# 导入头文件处理
include(add_header)

# 导入第三方依赖
include(3rd)

# 导入函数
include(functions)

# 添加要编译的目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

# readelf -a
readelf_a(${BOOT_ELF_OUTPUT_NAME})

# objdump -D
objdump_D(${BOOT_ELF_OUTPUT_NAME})

# 制作 boot.efi
# 将引导调整为 efi
elf2efi(${BOOT_ELF_OUTPUT_NAME} ${BOOT_EFI_OUTPUT_NAME})

add_custom_target(image_uefi DEPENDS ${BOOT_EFI_OUTPUT_NAME} ${KERNEL_ELF_OUTPUT_NAME}
        COMMENT "Copying bootloader and kernel"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/image/efi/boot
        COMMAND ${CMAKE_COMMAND} -E copy ${${BOOT_ELF_OUTPUT_NAME}_BINARY_DIR}/${BOOT_EFI_OUTPUT_NAME} ${PROJECT_BINARY_DIR}/image/efi/boot/
        COMMAND ${CMAKE_COMMAND} -E copy ${${KERNEL_ELF_OUTPUT_NAME}_BINARY_DIR}/${KERNEL_ELF_OUTPUT_NAME} ${PROJECT_BINARY_DIR}/image
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tools/startup.nsh ${PROJECT_BINARY_DIR}/image/startup.nsh
)

# 固件
if (TARGET_ARCH STREQUAL "riscv64")
    set(FIRMWARE fw_jump.elf)
    set(QEMU_FIRMWARE -bios ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FIRMWARE})
elseif (TARGET_ARCH STREQUAL "x86_64")
    set(FIRMWARE OVMF_${TARGET_ARCH}.fd)
    set(QEMU_FIRMWARE -bios ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FIRMWARE})
elseif (TARGET_ARCH STREQUAL "aarch64")
    set(FIRMWARE ovmf-aarch64.fd)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/emulation/)
    execute_process(COMMAND truncate -s 64m ${CMAKE_BINARY_DIR}/emulation/${FIRMWARE})
    execute_process(COMMAND truncate -s 64m ${CMAKE_BINARY_DIR}/emulation/nvram.img)
    execute_process(COMMAND dd if=${CMAKE_CURRENT_SOURCE_DIR}/third_party/firmware/tianocore/${FIRMWARE} of=${CMAKE_BINARY_DIR}/emulation/${FIRMWARE} conv=notrunc)
    set(QEMU_FIRMWARE -drive if=pflash,format=raw,file=${CMAKE_BINARY_DIR}/emulation/${FIRMWARE},readonly -drive if=pflash,format=raw,file=$(CMAKE_BINARY_DIR)/emulation/nvram.img)
endif ()

# 运行 qemu
add_custom_target(run DEPENDS image_uefi
        COMMENT "Run SimpleKernel in qemu."
        COMMAND ${CMAKE_COMMAND} -E echo ${QEMU_FLAGS}
        COMMAND qemu-system-${TARGET_ARCH} ${QEMU_FLAGS}
)
