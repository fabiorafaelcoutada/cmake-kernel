
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# 3rd.cmake for MRNIU/cmake-kernel.
# 第三方依赖管理

# https://github.com/cpm-cmake/CPM.cmake
include(CPM)

# https://github.com/google/googletest
CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG v1.13.0
  VERSION 1.13.0
  OPTIONS
      "INSTALL_GTEST OFF"
      "gtest_force_shared_crt ON"
)

# https://github.com/abumq/easyloggingpp
CPMAddPackage(
  NAME easylogingpp
  VERSION 9.97.0
  GITHUB_REPOSITORY amrayn/easyloggingpp
  OPTIONS
  "build_static_lib ON"
  "lib_utc_datetime ON"
)
# target_include_directories(<<Target>> SYSTEM PRIVATE "${easylogingpp_SOURCE_DIR}/src" <<etc>>)
# target_link_libraries(<<Target>> PRIVATE easyloggingpp <<etc>>)

# https://github.com/rttrorg/rttr
CPMAddPackage(
  NAME rttr # link against RTTR::Core_Lib
  VERSION 0.9.6
  GITHUB_REPOSITORY rttrorg/rttr
  OPTIONS
    "BUILD_RTTR_DYNAMIC Off"
    "BUILD_UNIT_TESTS Off"
    "BUILD_STATIC On"
    "BUILD_PACKAGE Off"
    "BUILD_WITH_RTTI On"
    "BUILD_EXAMPLES Off"
    "BUILD_DOCUMENTATION Off"
    "BUILD_INSTALLER Off"
    "USE_PCH Off"
    "CUSTOM_DOXYGEN_STYLE Off"
)

# https://github.com/TheLartians/Format.cmake
CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.7.3
)

# https://github.com/freetype/freetype
CPMAddPackage(
  NAME freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG VER-2-13-0
  VERSION 2.13.0
)
if (freetype_ADDED)
  add_library(Freetype::Freetype ALIAS freetype)
endif()

# https://github.com/cpm-cmake/CPMLicenses.cmake
CPMAddPackage(
  NAME CPMLicenses.cmake 
  GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
  VERSION 0.0.7
)
cpm_licenses_create_disclaimer_target(
  write-licenses "${CMAKE_CURRENT_SOURCE_DIR}/3rd/LICENSE" "${CPM_PACKAGES}"
)
