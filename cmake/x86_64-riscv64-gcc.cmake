
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
# 
# x86_64-riscv64-gcc.cmake for MRNIU/cmake-kernel.

# 目标为无操作系统的环境
set(CMAKE_SYSTEM_NAME Generic)
# 目标架构
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# @todo mac 测试
if (APPLE)
    message(STATUS "Now is Apple systens.")
    # GCC
    find_program(Compiler_gcc riscv64-linux-gnu-g++)
    if (NOT Compiler_gcc)
        message(FATAL_ERROR "riscv64-linux-gnu-g++ not found.\n"
                "Run `brew install riscv64-linux-gnu-g++` to install.")
    else ()
        message(STATUS "Found riscv64-linux-gnu-g++ ${Compiler_gcc}")
    endif ()

    # qemu
    find_program(QEMU qemu-system-riscv64)
    if (NOT QEMU)
        message(FATAL_ERROR "qemu not found.\n"
                "Run `brew install qemu-system` to install.")
    else ()
        message(STATUS "Found qemu ${QEMU}")
    endif ()
elseif (UNIX)
    message(STATUS "Now is UNIX-like OS's.")
    # GCC
    find_program(Compiler_gcc riscv64-linux-gnu-g++)
    if (NOT Compiler_gcc)
        message(FATAL_ERROR "riscv64-linux-gnu-g++ not found.\n"
                "Run `sudo apt-get install -y gcc riscv64-linux-gnu-g++` to install.")
    else ()
        message(STATUS "Found riscv64-linux-gnu-g++ ${Compiler_gcc}")
    endif ()

    set(TOOLCHAIN_PREFIX   riscv64-linux-gnu-)
    set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_READELF      ${TOOLCHAIN_PREFIX}readelf)
    set(CMAKE_AR           ${TOOLCHAIN_PREFIX}ar)
    set(CMAKE_LINKER       ${TOOLCHAIN_PREFIX}ld)
    set(CMAKE_NM           ${TOOLCHAIN_PREFIX}nm)
    set(CMAKE_OBJDUMP      ${TOOLCHAIN_PREFIX}objdump)
    set(CMAKE_RANLIB       ${TOOLCHAIN_PREFIX}ranlib)

    # qemu
    find_program(QEMU qemu-system-riscv64)
    if (NOT QEMU)
        message(FATAL_ERROR "qemu not found.\n"
                "Run `sudo apt-get install -y qemu-system` to install.")
    else ()
        message(STATUS "Found qemu ${QEMU}")
    endif ()
endif ()