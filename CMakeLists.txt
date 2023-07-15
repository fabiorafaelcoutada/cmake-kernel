
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# CMakeLists.txt for MRNIU/cmake-kernel.

# 可用的参数列表
# ENABLE_BUILD_RELEASE  是否为发布版，默认为 OFF
# ENABLE_GENERATOR_MAKE 是否使用 make 构建，默认为 ON
# ENABLE_COMPILER_GNU   是否使用 gcc，默认为 ON
# ENABLE_TEST_COVERAGE  是否开启测试覆盖率，默认为 ON
# MACHINE               运行的平台，默认为 qemu
# TARGET_ARCH           目标平台，默认为 x86_64
# BOOT_NAME             启动文件名，默认为 boot.elf
# KERNEL_NAME           内核文件名，默认为 kernel.elf

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

# 导入第三方依赖
include(3rd)

set(sources
    main.cpp
)

add_executable(helloworld
    ${sources}
)

target_compile_options(helloworld
    ${DEFAULT_COMPILE_OPTIONS}
)

# 生成依赖项 LICENSE
# add_executable(${PROJECT_NAME} main.c)
# add_custom_command(TARGET ${PROJECT_NAME}
#         PRE_BUILD 
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rd
#         COMMAND make write-licenses
#         COMMENT "build gnu-efi..."
# )

# # ---- Add source files ----

# # Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# # automatically. Keep that in mind when changing files, or explicitly mention them here.
# file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
# file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# # ---- Create library ----

# # Note: for header-only libraries change all PUBLIC flags to INTERFACE and create an interface
# # target: add_library(${PROJECT_NAME} INTERFACE)
# add_library(${PROJECT_NAME} ${headers} ${sources})
# set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# # being a cross-platform target, we enforce standards conformance on MSVC
# target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# # Link dependencies
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# target_include_directories(
#   ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
#                          $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
# )

# # ---- Create an installable target ----
# # this allows users to install and find the library via `find_package()`.

# # the location where the project's version header will be placed should match the project's regular
# # header paths
# string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

# packageProject(
#   NAME ${PROJECT_NAME}
#   VERSION ${PROJECT_VERSION}
#   NAMESPACE ${PROJECT_NAME}
#   BINARY_DIR ${PROJECT_BINARY_DIR}
#   INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
#   INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
#   VERSION_HEADER "${VERSION_HEADER_LOCATION}"
#   COMPATIBILITY SameMajorVersion
#   DEPENDENCIES "fmt 9.1.0"
# )
