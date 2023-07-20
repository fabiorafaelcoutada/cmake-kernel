
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# config.cmake for MRNIU/cmake-kernel.
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

# 设置使用的 C/C++ 版本
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

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
# 是否使用 gcc，默认为 ON
option(ENABLE_COMPILER_GNU "Use gcc or clang" ON)
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
message("ENABLE_BUILD_RELEASE is: ${ENABLE_BUILD_RELEASE}")

# 设置构建使用的工具，默认为 make
if (ENABLE_GENERATOR_MAKE)
    set(GENERATOR_COMMAND make)
else ()
    set(GENERATOR_COMMAND ninja)
endif ()
message("GENERATOR_COMMAND is: ${GENERATOR_COMMAND}")

# 要运行的平台
list(APPEND VALID_PLATFORM qemu)
if (NOT DEFINED PLATFORM)
    set(PLATFORM qemu)
endif ()
message("PLATFORM is: ${PLATFORM}")
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
message("TARGET_ARCH is: ${TARGET_ARCH}")
# 如果不合法则报错
if (NOT TARGET_ARCH IN_LIST VALID_TARGET_ARCH)
    message(FATAL_ERROR "TARGET_ARCH must be one of ${VALID_TARGET_ARCH}")
endif ()

# 设置编译器
if (ENABLE_COMPILER_GNU)
    # gcc 工具链文件的命名规则：target-host-gcc.cmake(目标架构-本机架构-gcc.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/${TARGET_ARCH}-${CMAKE_HOST_SYSTEM_PROCESSOR}-gcc.cmake)
else ()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/${TARGET_ARCH}-${CMAKE_HOST_SYSTEM_PROCESSOR}-clang.cmake)
endif ()
message("CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

# 编译器只支持 gnu-gcc 或 clang
if (NOT ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    message(FATAL_ERROR "Only support gnu-gcc/clang")
endif()

# 编译选项
list(APPEND DEFAULT_COMPILE_OPTIONS
    # 如果 ENABLE_BUILD_RELEASE 为 ON 则使用 -O3 -Werror，否则使用 -O0 -g -ggdb
    $<IF:$<BOOL:${ENABLE_BUILD_RELEASE}>,-O3;-Werror,-O0;-g;-ggdb>
    # 打开全部警告
    -Wall 
    # 打开额外警告
    -Wextra
    # 将代码编译为无操作系统支持的独立程序
    -ffreestanding
    # 启用异常处理机制
    -fexceptions 
    # 使用 2 字节 wchar_t
    -fshort-wchar
    # 允许 wchar_t
    -fpermissive
    
    # 目标平台编译选项
    # @todo clang 交叉编译参数
    $<$<STREQUAL:${TARGET_ARCH},x86_64>:
        # 
        -mno-red-zone
    >
    $<$<STREQUAL:${TARGET_ARCH},riscv64>:
        # 生成 rv64imafdc 代码
        -march=rv64imafdc
    >
    $<$<STREQUAL:${TARGET_ARCH},aarch64>:
        # 生成 armv8-a 代码
        -march=armv8-a
        # 针对 cortex-a72 优化代码
        -mtune=cortex-a72
    >

    # 如果 ENABLE_TEST_COVERAGE 为 ON 则使用 -fprofile-arcs -ftest-coverage，否则为空
    # $<BOOL:${ENABLE_TEST_COVERAGE}:-fprofile-arcs;-ftest-coverage>

    # gcc 特定选项
    $<$<CXX_COMPILER_ID:GNU>:
    >
    
    # clang 特定选项
    $<$<CXX_COMPILER_ID:Clang>:
    >

    # 定义 gnuefi 宏
    -DGNU_EFI_USE_MS_ABI
    
    # 平台相关
    $<$<PLATFORM_ID:Darwin>:
    >
)

# 链接选项
list(APPEND DEFAULT_LINK_OPTIONS
    # 生成位置无关代码 Position-Independent Code
    # -fPIC
    # 链接脚本
    -T ${CMAKE_SOURCE_DIR}/src/arch/${TARGET_ARCH}/link.ld
    # 目标平台编译选项
    # @todo clang 交叉编译参数
    $<$<STREQUAL:${TARGET_ARCH},x86_64>:
        # 设置最大页大小为 0x1000(4096) 字节
        -z max-page-size=0x1000
    >
    $<$<STREQUAL:${TARGET_ARCH},riscv64>:
    >
    $<$<STREQUAL:${TARGET_ARCH},aarch64>:
    >
)

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
