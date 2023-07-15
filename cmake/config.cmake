
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# config.cmake for MRNIU/cmake-kernel.
# 配置信息

# 设置依赖下载路径
set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
# 优先使用本地文件
set(CPM_USE_LOCAL_PACKAGES True)

# 设置 cmake 目标环境根目录
# @todo 设置目录
set(CMAKE_FIND_ROOT_PATH 
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

# 设置 bin、lib 的输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 是否为发布版，默认为 OFF
option(ENABLE_BUILD_RELEASE "Build as release" OFF)
# 是否使用 make 构建，默认为 ON
option(ENABLE_GENERATOR_MAKE "Use make or ninja" ON)
# 是否使用 gcc，默认为 ON
option(ENABLE_COMPILER_GNU "Use gcc or clang" ON)
# 是否开启测试覆盖率，默认为 ON
option(ENABLE_TEST_COVERAGE "Enable test coverage" ON)

# 设置构建使用的工具，默认为 make
if (ENABLE_GENERATOR_MAKE)
    set(GENERATOR_COMMAND make)
else ()
    set(GENERATOR_COMMAND ninja)
endif ()
message("GENERATOR_COMMAND is: ${GENERATOR_COMMAND}")

# 设置使用的 C/C++ 版本
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# 要运行的平台
set(VALID_PLATFORM qemu)
if (NOT DEFINED PLATFORM)
    set(PLATFORM qemu)
endif ()
message("PLATFORM is: ${PLATFORM}")
# 如果不合法则报错
if (NOT PLATFORM IN_LIST VALID_PLATFORM)
    message(FATAL_ERROR "PLATFORM must be one of ${VALID_PLATFORM}")
endif ()

# 目标架构
set(VALID_TARGET_ARCH x86_64 riscv64 aarch64)
# 默认构建 x86_64
if (NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH x86_64)
endif ()
message("TARGET_ARCH is: ${TARGET_ARCH}")
# 如果不合法则报错
if (NOT TARGET_ARCH IN_LIST VALID_TARGET_ARCH)
    message(FATAL_ERROR "TARGET_ARCH must be one of ${VALID_TARGET_ARCH}")
endif ()

# 是否为 Debug 版本，默认为 Debug
if (ENABLE_BUILD_RELEASE)
    # 不显示 make 细节
    set(CMAKE_VERBOSE_MAKEFILE OFF)
else ()
    # 显示 make 细节
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()
message("ENABLE_BUILD_RELEASE is: ${ENABLE_BUILD_RELEASE}")

# 设置编译器
if (ENABLE_COMPILER_GNU)
    # gcc 工具链文件的命名规则：target-host-gcc.cmake(目标架构-本机架构-gcc.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/${TARGET_ARCH}-${CMAKE_HOST_SYSTEM_PROCESSOR}-gcc.cmake)
else ()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/${TARGET_ARCH}-${CMAKE_HOST_SYSTEM_PROCESSOR}-clang.cmake)
endif ()
message("CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

# 编译选项
set(DEFAULT_COMPILE_OPTIONS)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(DEFAULT_COMPILE_OPTIONS
        PRIVATE
            # 发布模式
            $<$<BOOL:${ENABLE_BUILD_RELEASE}>:
                # 代码优化级别
                -O3
                # 警告作为错误处理
                -Werror
            >
            # 调试模式
            $<$<NOT:$<BOOL:${ENABLE_BUILD_RELEASE}>>:
                # 代码优化级别
                -O0
                # 生成调试信息
                -g
                # 生成 gdb 调试信息
                -ggdb
            >
            # 打开全部警告
            -Wall 
            # 打开额外警告
            -Wextra
            # 不生成位置无关可执行程序 Position-Independent Executable
            # @todo 能不能不用？
            -no-pie
            # 生成位置无关代码 Position-Independent Code
            -fPIC
            # 不链接标准库
            # @todo 能不能不用？
            -nostdlib
            # 将代码编译为无操作系统支持的独立程序
            -ffreestanding
            # 启用异常处理机制
            -fexceptions 
            # 使用 2 字节 wchar_t
            -fshort-wchar
            
            # 目标平台编译选项
            # @todo clang 交叉编译参数
            $<$<STREQUAL:${TARGET_ARCH},x86_64>:
                # @todo 能否减少
                # 生成 corei7 代码
                -march=corei7
                # 针对 corei7 优化代码
                -mtune=corei7
                # 生成 64 位代码
                -m64
                # 
                -mno-red-zone
                # 设置最大页大小为 0x1000(4096) 字节
                -z max-page-size=0x1000
                # 编译为共享库
                -Wl,-shared
                # 符号级别绑定
                -Wl,-Bsymbolic
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


            # 测试覆盖率选项
            $<$<BOOL:${ENABLE_TEST_COVERAGE}>:
                # 启用代码覆盖率
                -fprofile-arcs
                # 生成代码覆盖率报告
                -ftest-coverage
            >

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
                -pthread
            >
    )
endif ()

# 设置引导名称
set(BOOT_NAME boot.elf)
# 设置内核名称
set(KERNEL_NAME kernel.elf)
