
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# project_config.cmake for MRNIU/cmake-kernel.
# 配置信息

# 设置 cmake 目标环境根目录
# @todo 设置目录
list(APPEND CMAKE_FIND_ROOT_PATH
        /usr/x86_64-linux-gnu
        /usr/riscv64-linux-gnu
        /usr/aarch64-linux-gnu
        )
# 在目标环境搜索 program
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# 在目标环境搜索库文件
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# 在目标环境搜索头文件
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 设置清理目标 在 make clean 时删除文件夹
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        # 清除缓存
        ${PROJECT_BINARY_DIR}/CMakeCache.txt
        # 删除 .gdbinit
        ${CMAKE_SOURCE_DIR}/.gdbinit
        )

# 是否为发布版，默认为 OFF
option(ENABLE_BUILD_RELEASE "Build as release" OFF)
# 是否使用 make 构建，默认为 ON
option(ENABLE_GENERATOR_MAKE "Use make or ninja" ON)
# 是否使用 gnu-efi，默认为 ON，仅在 x86_64 平台有效
option(ENABLE_GNU_EFI "Use gnu efi" ON)
# 是否开启测试覆盖率，默认为 ON
option(ENABLE_TEST_COVERAGE "Enable test coverage" ON)
# 是否开启 gdb 调试，默认为 OFF
option(ENABLE_GDB "Enable debug" OFF)

# 是否为 Debug 版本，默认为 Debug
if (ENABLE_BUILD_RELEASE)
    # 不显示 make 细节
    set(CMAKE_VERBOSE_MAKEFILE OFF)
else ()
    # 显示 make 细节
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()
message(STATUS "ENABLE_BUILD_RELEASE is: ${ENABLE_BUILD_RELEASE}")

# 设置构建使用的工具，默认为 make
if (ENABLE_GENERATOR_MAKE)
    set(GENERATOR_COMMAND make)
else ()
    set(GENERATOR_COMMAND ninja)
endif ()
message(STATUS "ENABLE_GENERATOR_MAKE is: ${ENABLE_GENERATOR_MAKE}")

# 要运行的平台
list(APPEND VALID_PLATFORM qemu)
if (NOT DEFINED PLATFORM)
    set(PLATFORM qemu)
endif ()
message(STATUS "PLATFORM is: ${PLATFORM}")
# 如果不合法则报错
if (NOT PLATFORM IN_LIST VALID_PLATFORM)
    message(FATAL_ERROR "PLATFORM must be one of ${VALID_PLATFORM}")
endif ()

# 目标架构
list(APPEND VALID_TARGET_ARCH x86_64 riscv64 aarch64)
# 默认构建 x86_64
if (NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH x86_64)
endif ()
message(STATUS "TARGET_ARCH is: ${TARGET_ARCH}")
# 如果不合法则报错
if (NOT TARGET_ARCH IN_LIST VALID_TARGET_ARCH)
    message(FATAL_ERROR "TARGET_ARCH must be one of ${VALID_TARGET_ARCH}")
endif ()

message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")
# 编译器只支持 gnu-gcc 或 clang
if (NOT ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    message(FATAL_ERROR "Only support gnu-gcc/clang")
endif ()

# 设置二进制文件名称
if (NOT DEFINED BOOT_ELF_OUTPUT_NAME)
    set(BOOT_ELF_OUTPUT_NAME boot.elf)
endif ()
if (NOT DEFINED BOOT_EFI_OUTPUT_NAME)
    set(BOOT_EFI_OUTPUT_NAME boot.efi)
endif ()
if (NOT DEFINED KERNEL_ELF_OUTPUT_NAME)
    set(KERNEL_ELF_OUTPUT_NAME kernel.elf)
endif ()
if (NOT DEFINED KERNEL_EFI_OUTPUT_NAME)
    set(KERNEL_EFI_OUTPUT_NAME kernel.efi)
endif ()

# qemu 运行依赖
if (${TARGET_ARCH} STREQUAL "x86_64")
    list(APPEND RUN_DEPENDS
    ovmf
    image_uefi
    )
elseif(${TARGET_ARCH} STREQUAL "riscv64")
    list(APPEND RUN_DEPENDS
    opensbi
    ${KERNEL_ELF_OUTPUT_NAME}
    )
elseif(${TARGET_ARCH} STREQUAL "aarch64")
    list(APPEND RUN_DEPENDS
    ovmf
    image_uefi
    )
endif()
