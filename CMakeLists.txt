
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

# 创建 image 目录并将文件复制
add_custom_target(image_uefi DEPENDS ${BOOT_EFI_OUTPUT_NAME} ${KERNEL_ELF_OUTPUT_NAME}
        COMMENT "Copying bootloader and kernel"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/image/
        COMMAND ${CMAKE_COMMAND} -E copy ${${BOOT_ELF_OUTPUT_NAME}_BINARY_DIR}/${BOOT_EFI_OUTPUT_NAME} ${PROJECT_BINARY_DIR}/image/
        COMMAND ${CMAKE_COMMAND} -E copy ${${KERNEL_ELF_OUTPUT_NAME}_BINARY_DIR}/${KERNEL_ELF_OUTPUT_NAME} ${PROJECT_BINARY_DIR}/image/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tools/startup.nsh ${PROJECT_BINARY_DIR}/image/
        )

# qemu 参数设置
list(APPEND QEMU_FLAGS
        # 使用标准输出显示
        -serial stdio
        # 启动 telnet 服务，使用 2333 端口，不等待连接
        -monitor telnet::2333,server,nowait
        )
# 目标平台参数
if (TARGET_ARCH STREQUAL "x86_64")
    list(APPEND QEMU_FLAGS
            -m 128M
            -net none
            -bios ${ovmf_BINARY_DIR}/OVMF_${TARGET_ARCH}.fd
            -hda fat:rw:${CMAKE_BINARY_DIR}/image/
            )
elseif (TARGET_ARCH STREQUAL "riscv64")
    list(APPEND QEMU_FLAGS
            -machine virt
            -nographic
            -bios ${opensbi_BINARY_DIR}/platform/generic/firmware/fw_jump.elf
            -kernel ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KERNEL_ELF_OUTPUT_NAME}
            )
elseif (TARGET_ARCH STREQUAL "aarch64")
    # @todo
endif ()

# 如果 ENABLE_GDB 为 ON
if (ENABLE_GDB)
    list(APPEND QEMU_FLAGS
            # 等待 gdb 连接
            -S
            # 使用 1234 端口
            -gdb tcp::1234
            )
endif ()

# 运行 qemu
add_custom_target(run DEPENDS image_uefi
        COMMENT "Run SimpleKernel in qemu."
        COMMAND ${CMAKE_COMMAND} -E echo ${QEMU_FLAGS}
        COMMAND qemu-system-${TARGET_ARCH} ${QEMU_FLAGS}
        )
