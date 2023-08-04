
/**
 * @file read_elf.cpp
 * @brief 读取 elf 文件
 * @author Zone.N (Zone.Niuzh@hotmail.com)
 * @version 1.0
 * @date 2023-07-15
 * @copyright MIT LICENSE
 * https://github.com/MRNIU/cmake-kernel
 * @par change log:
 * <table>
 * <tr><th>Date<th>Author<th>Description
 * <tr><td>2023-07-15<td>Zone.N (Zone.Niuzh@hotmail.com)<td>创建文件
 * </table>
 */

#include "cstring"
#include "load_elf.h"

uint8_t                       shstrtab_buf[SECTION_BUF_SIZE] = { 0 };

std::pair<EFI_STATUS, size_t> get_file_size(const EFI_FILE& _file) {
    // 获取 elf 文件大小
    auto       elf_file_info = LibFileInfo(&_file);
    auto       file_size     = elf_file_info->FileSize;
    EFI_STATUS status
      = uefi_call_wrapper(gBS->FreePool, 1, (void*)elf_file_info);
    if (check_for_fatal_error(status, L"Error FreePool")) {
        return std::make_pair(status, 0);
    }
    return std::make_pair(status, file_size);
}

EFI_STATUS check_elf_identity(const uint8_t* const _elf_file_buffer) {
    if ((_elf_file_buffer[EI_MAG0] != ELFMAG0)
        || (_elf_file_buffer[EI_MAG1] != ELFMAG1)
        || (_elf_file_buffer[EI_MAG2] != ELFMAG2)
        || (_elf_file_buffer[EI_MAG3] != ELFMAG3)) {
        debug(L"Fatal Error: Invalid ELF header\n");
        return EFI_INVALID_PARAMETER;
    }

    if (_elf_file_buffer[EI_CLASS] == ELFCLASS32) {
        debug(L"Found 32bit executable but NOT SUPPORT\n");
        return EFI_UNSUPPORTED;
    }
    else if (_elf_file_buffer[EI_CLASS] == ELFCLASS64) {
        debug(L"Found 64bit executable\n");
    }
    else {
        debug(L"Fatal Error: Invalid executable\n");
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

const Elf64_Ehdr& get_ehdr(const uint8_t* const _elf_file_buffer) {
    return *reinterpret_cast<const Elf64_Ehdr*>(_elf_file_buffer);
}

const Elf64_Phdr* const
get_phdr(uint64_t _offset, const uint8_t* const _elf_file_buffer) {
    return reinterpret_cast<const Elf64_Phdr* const>(_elf_file_buffer
                                                     + _offset);
}

void read_section(const uint8_t* const _elf_file_buffer,
                  const Elf64_Shdr& _shdr, uint8_t* _buffer) {
    memcpy(_buffer, _elf_file_buffer + _shdr.sh_offset, _shdr.sh_size);
    return;
}

const Elf64_Shdr* const
get_shdr(uint64_t _offset, const uint8_t* const _elf_file_buffer) {
    return reinterpret_cast<const Elf64_Shdr* const>(_elf_file_buffer
                                                     + _offset);
}
