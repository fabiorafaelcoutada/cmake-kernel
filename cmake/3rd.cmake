
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# 3rd.cmake for MRNIU/cmake-kernel.
# 依赖管理

# 设置依赖下载路径
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/3rd)
# 优先使用本地文件
set(CPM_USE_LOCAL_PACKAGES True)
# https://github.com/cpm-cmake/CPM.cmake
# -------- get_cpm.cmake --------
set(CPM_DOWNLOAD_VERSION 0.38.2)

if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

function(download_cpm)
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
       https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION}
  )
endfunction()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  download_cpm()
else()
  # resume download if it previously failed
  file(READ ${CPM_DOWNLOAD_LOCATION} check)
  if("${check}" STREQUAL "")
    download_cpm()
  endif()
  unset(check)
endif()

include(${CPM_DOWNLOAD_LOCATION})
# -------- get_cpm.cmake --------

# # https://github.com/google/googletest
# CPMAddPackage(
#   NAME googletest
#   GITHUB_REPOSITORY google/googletest
#   GIT_TAG v1.13.0
#   VERSION 1.13.0
#   OPTIONS
#       "INSTALL_GTEST OFF"
#       "gtest_force_shared_crt ON"
# )

# # https://github.com/abumq/easyloggingpp
# CPMAddPackage(
#   NAME easylogingpp
#   VERSION 9.97.0
#   GITHUB_REPOSITORY amrayn/easyloggingpp
#   OPTIONS
#   "build_static_lib ON"
#   "lib_utc_datetime ON"
# )

# https://github.com/rttrorg/rttr
# @bug 打开这个会导致编译参数中多出来几个
# CPMAddPackage(
#   NAME rttr # link against RTTR::Core_Lib
#   VERSION 0.9.6
#   GITHUB_REPOSITORY rttrorg/rttr
#   OPTIONS
#     "BUILD_RTTR_DYNAMIC Off"
#     "BUILD_UNIT_TESTS Off"
#     "BUILD_STATIC On"
#     "BUILD_PACKAGE Off"
#     "BUILD_WITH_RTTI On"
#     "BUILD_EXAMPLES Off"
#     "BUILD_DOCUMENTATION Off"
#     "BUILD_INSTALLER Off"
#     "USE_PCH Off"
#     "CUSTOM_DOXYGEN_STYLE Off"
# )

# https://github.com/TheLartians/Format.cmake
# CPMAddPackage(
#   NAME Format.cmake
#   GITHUB_REPOSITORY TheLartians/Format.cmake
#   VERSION 1.7.3
# )

# # https://github.com/freetype/freetype
# CPMAddPackage(
#   NAME freetype
#   GIT_REPOSITORY https://github.com/freetype/freetype.git
#   GIT_TAG VER-2-13-0
#   VERSION 2.13.0
# )
# if (freetype_ADDED)
#   add_library(Freetype::Freetype ALIAS freetype)
# endif()

# # https://github.com/riscv-software-src/opensbi
# CPMAddPackage(
#   NAME opensbi
#   GIT_REPOSITORY https://github.com/riscv-software-src/opensbi.git
#   GIT_TAG v1.3
#   VERSION 1.3
#   DOWNLOAD_ONLY True
# )
# if (opensbi_ADDED)
#   # 编译 opensbi
#   add_custom_target(opensbi
#     COMMENT "build opensbi..."
#     # make 时编译
#     ALL
#     WORKING_DIRECTORY ${opensbi_SOURCE_DIR}
#     COMMAND 
#     ${CMAKE_COMMAND} 
#     -E 
#     make_directory 
#     ${opensbi_BINARY_DIR}
#     COMMAND
#       make
#       # @todo 这个工具链只在 ubuntu 上测试过
#       CROSS_COMPILE=riscv64-linux-gnu-
#       FW_JUMP=y 
#       FW_JUMP_ADDR=0x80200000
#       PLATFORM_RISCV_XLEN=64
#       PLATFORM=generic
#       O=${opensbi_BINARY_DIR}
#   )
# endif()

# # https://gitlab.com/bztsrc/posix-uefi
# CPMAddPackage(
#   NAME posix-uefi
#   GIT_REPOSITORY https://gitlab.com/bztsrc/posix-uefi.git
#   GIT_TAG a643ed09f52575d402b934d6f1c6f08c64fd8c64
#   DOWNLOAD_ONLY True
# )
# if (posix-uefi_ADDED)
#   # 编译 posix-uefi
#   add_custom_target(posix-uefi
#           # make 时编译
#           ALL
#           WORKING_DIRECTORY ${posix-uefi_SOURCE_DIR}/uefi
#           COMMAND
#           # @todo 仅支持 gcc
#             USE_GCC=1
#             ARCH=${TARGET_ARCH}
#             make
#           COMMAND 
#             ${CMAKE_COMMAND} 
#             -E 
#             make_directory 
#             ${posix-uefi_BINARY_DIR}
#           COMMAND 
#             ${CMAKE_COMMAND} 
#             -E 
#             copy_directory
#             ${posix-uefi_SOURCE_DIR}/build
#             ${posix-uefi_BINARY_DIR}
#           COMMAND
#             make clean
#           COMMAND
#             rm -rf ${posix-uefi_SOURCE_DIR}/build
#           COMMENT "build posix-uefi..."
#   )
# endif()

# https://sourceforge.net/projects/gnu-efi/
CPMAddPackage(
  NAME gnu-efi
  URL "https://sourceforge.net/projects/gnu-efi/files/gnu-efi-3.0.17.tar.bz2"
  VERSION 3.0.17
  DOWNLOAD_ONLY True
)
if (gnu-efi_ADDED)
  # 编译 gnu-efi
  add_custom_target(gnu-efi
    COMMENT "build gnu-efi..."
    # make 时编译
    ALL
    WORKING_DIRECTORY ${gnu-efi_SOURCE_DIR}
    COMMAND 
      ${CMAKE_COMMAND} 
      -E 
      make_directory 
    ${gnu-efi_BINARY_DIR}
    COMMAND
      make
      ARCH=${TARGET_ARCH}
      OBJDIR=${gnu-efi_BINARY_DIR}
    COMMAND 
      ${CMAKE_COMMAND} 
      -E 
      copy_directory 
      ${gnu-efi_SOURCE_DIR}/inc
      ${gnu-efi_BINARY_DIR}/inc
  )
endif ()

# ovmf
# @todo 使用互联网连接或从 edk2 编译
CPMAddPackage(
  NAME ovmf
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/tools/ovmf
)
if (ovmf_ADDED)
  add_custom_target(ovmf
    COMMENT "build ovmf ..."
    # make 时编译
    ALL
    WORKING_DIRECTORY ${ovmf_SOURCE_DIR}
    COMMAND 
      ${CMAKE_COMMAND} 
      -E
      copy
      ${ovmf_SOURCE_DIR}/*
      ${ovmf_BINARY_DIR}
  )
endif()

# https://github.com/gdbinit/Gdbinit
# @todo 下载下来的文件为 makefile 形式，需要自己编译
CPMAddPackage(
  NAME gdbinit
  GIT_REPOSITORY https://github.com/gdbinit/Gdbinit.git
  GIT_TAG e5138c24226bdd05360ca41743d8315a9e366c40
  DOWNLOAD_ONLY True
)
if (gdbinit_ADDED)
  if (ENABLE_GDB)
    add_custom_target(gdbinit
      COMMENT "build gnu-efi ..."
      # make 时编译
      ALL
      WORKING_DIRECTORY ${gdbinit_SOURCE_DIR}
      # 复制到根目录下并重命名
      COMMAND 
        ${CMAKE_COMMAND} 
        -E
        copy 
        ${gdbinit_SOURCE_DIR}/gdbinit
        ${CMAKE_SOURCE_DIR}/.gdbinit
    )
  endif()
endif()

# # https://github.com/tianocore/edk2
# # @todo 下载下来的文件为 makefile 形式，需要自己编译
# CPMAddPackage(
#   NAME edk2
#   GIT_REPOSITORY https://github.com/tianocore/edk2.git
#   GIT_TAG edk2-stable202305
#   DOWNLOAD_ONLY True
# )

# https://github.com/cpm-cmake/CPMLicenses.cmake
# 保持在文件最后
CPMAddPackage(
  NAME CPMLicenses.cmake 
  GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
  VERSION 0.0.7
)
if (gnu-efi_ADDED)
  cpm_licenses_create_disclaimer_target(
    write-licenses "${CMAKE_CURRENT_SOURCE_DIR}/3rd/LICENSE" "${CPM_PACKAGES}"
  )
endif ()
# make 时自动在 3rd 文件夹下生成 LICENSE 文件
add_custom_target(3rd_licenses
  ALL
  COMMAND
  make
  write-licenses
)
