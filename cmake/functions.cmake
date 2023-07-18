
# This file is a part of MRNIU/cmake-kernel
# (https://github.com/MRNIU/cmake-kernel).
#
# functions.cmake for MRNIU/cmake-kernel.
# 辅助函数

# 生成 elf 文件的 readelf -a
# _elf: elf 文件 target
# 在 ${${_elf}_BINARY_DIR} 目录下生成 ${_elf}.readelf 文件
function(readelf_a _elf)
    add_custom_target(readelf_a_${_elf} DEPENDS ${_elf})
    add_custom_command(TARGET readelf_a_${_elf}
        COMMENT "readelf -a ${_elf} ..."
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_READELF} -a ${${_elf}_BINARY_DIR}/${_elf} > ${${_elf}_BINARY_DIR}/${_elf}.readelf || (exit 0)
    )
endfunction()

# 生成 elf 文件的 objdump -D
# _elf: elf 文件 target
# 在 ${${_elf}_BINARY_DIR} 目录下生成 ${_elf}.disassembly 文件 
function(objdump_D _elf)
    add_custom_target(objdump_D_${_elf} DEPENDS ${_elf})
    add_custom_command(TARGET objdump_D_${_elf}
        COMMENT "objdump -D ${_elf} ..."
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_OBJDUMP} -D ${${_elf}_BINARY_DIR}/${_elf} > ${${_elf}_BINARY_DIR}/${_elf}.disassembly
    )
endfunction()

# 将 elf 转换为 efi，会添加一个 ${efi} 命令
# _elf: 要转换的 elf 文件 target
# _efi: 输出的 efi 文件 target
# 在 ${${_elf}_BINARY_DIR} 目录下生成 ${_efi} 文件
function(elf2efi _elf _efi)
    add_custom_target(${_efi} DEPENDS ${_elf})
    add_custom_command(TARGET ${_efi}
        COMMENT "Building ${_efi} ..."
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} ${${_elf}_BINARY_DIR}/${_elf} ${${_elf}_BINARY_DIR}/${_efi}
        -g -R .comment -R .gnu_debuglink -R .note.gnu.build-id
        -R .gnu.hash -R .plt -R .rela.plt -R .dynstr -R .dynsym -R .rela.dyn
        -S -R .eh_frame -R .gcc_except_table -R .hash
        --target=efi-app-${TARGET_ARCH} --subsystem=10
)
endfunction()