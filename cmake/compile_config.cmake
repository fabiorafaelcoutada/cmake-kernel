
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# compile_config.cmake for MRNIU/cmake-kernel.
# 配置信息

# 设置使用的 C/C++ 版本
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

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
        # 禁用 red-zone
        -mno-red-zone
        # 使用的 uefi 版本
        -DGNU_EFI_USE_MS_ABI
        -DENABLE_GNU_EFI=$<BOOL:${ENABLE_GNU_EFI}>
        >
        $<$<STREQUAL:${TARGET_ARCH},riscv64>:
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

        # 平台相关
        $<$<PLATFORM_ID:Darwin>:
        >
        )

# 链接选项
list(APPEND DEFAULT_LINK_OPTIONS
        # 不链接标准库
        -nostdlib
        # 链接脚本
        -T ${CMAKE_SOURCE_DIR}/src/kernel/arch/${TARGET_ARCH}/link.ld
        # 目标平台编译选项
        # @todo clang 交叉编译参数
        $<$<STREQUAL:${TARGET_ARCH},x86_64>:
        # 编译为共享库
        -shared
        # 符号级别绑定
        -Wl,-Bsymbolic
        # 设置最大页大小为 0x1000(4096) 字节
        -z max-page-size=0x1000
        >
        $<$<STREQUAL:${TARGET_ARCH},riscv64>:
        # 不生成位置无关可执行代码
        -no-pie
        >
        $<$<STREQUAL:${TARGET_ARCH},aarch64>:
        >
        )

# 库选项
list(APPEND DEFAULT_LINK_LIB
        # 目标平台编译选项
        $<$<STREQUAL:${TARGET_ARCH},x86_64>:
        # 根据 uefi 链接对应的库
        $<$<BOOL:${ENABLE_GNU_EFI}>:
        # ${gnu-efi_BINARY_DIR}/gnuefi/reloc_${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/crt0-efi-${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/libgnuefi.a
        ${gnu-efi_BINARY_DIR}/lib/libefi.a
        >
        $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
        ${3RD_ARCHIVE_OUTPUT_DIRECTORY}/posix-uefi/crt0.o
        ${3RD_LIBRARY_OUTPUT_DIRECTORY}/posix-uefi/libuefi.a
        >
        >
        $<$<STREQUAL:${TARGET_ARCH},riscv64>:
        >
        $<$<STREQUAL:${TARGET_ARCH},aarch64>:
        # 根据 uefi 链接对应的库
        $<$<BOOL:${ENABLE_GNU_EFI}>:
        # ${gnu-efi_BINARY_DIR}/gnuefi/reloc_${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/crt0-efi-${TARGET_ARCH}.o
        ${gnu-efi_BINARY_DIR}/gnuefi/libgnuefi.a
        ${gnu-efi_BINARY_DIR}/lib/libefi.a
        >
        $<$<NOT:$<BOOL:${ENABLE_GNU_EFI}>>:
        ${3RD_ARCHIVE_OUTPUT_DIRECTORY}/posix-uefi/crt0.o
        ${3RD_LIBRARY_OUTPUT_DIRECTORY}/posix-uefi/libuefi.a
        >
        >
        )

# 编译依赖
if (${TARGET_ARCH} STREQUAL "x86_64")
    list(APPEND COMPILE_DEPENDS
            gnu-efi
            )
elseif (${TARGET_ARCH} STREQUAL "riscv64")
    list(APPEND COMPILE_DEPENDS
            opensbi
            )
elseif (${TARGET_ARCH} STREQUAL "aarch64")
    list(APPEND COMPILE_DEPENDS
            gnu-efi
            )
endif ()
