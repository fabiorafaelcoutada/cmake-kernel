
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

#ifdef __cplusplus
extern "C" {
#endif

#include "load_elf.h"

EFI_STATUS read_and_check_elf_identity(const EFI_FILE& _kernel_img_file) {
    uint8_t    buffer[EI_NIDENT] = { 0 };
    uint64_t   buffer_read_size  = EI_NIDENT;

    // 将文件指针设置为 0
    EFI_STATUS status = uefi_call_wrapper(_kernel_img_file.SetPosition, 2,
                                          (EFI_FILE*)&_kernel_img_file, 0);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error resetting file pointer position: %s\n",
              get_efi_error_message(status));
        return status;
    }

    debug((CHAR16*)L"Reading ELF identity\n");
    status = uefi_call_wrapper(_kernel_img_file.Read, 3,
                               (EFI_FILE*)&_kernel_img_file, &buffer_read_size,
                               (void*)buffer);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error reading kernel identity: %s\n",
              get_efi_error_message(status));
        return status;
    }

    if ((buffer[EI_MAG0] != ELFMAG0) || (buffer[EI_MAG1] != ELFMAG1)
        || (buffer[EI_MAG2] != ELFMAG2) || (buffer[EI_MAG3] != ELFMAG3)) {
        debug((CHAR16*)L"Fatal Error: Invalid ELF header\n");
        return EFI_INVALID_PARAMETER;
    }

    if (buffer[EI_CLASS] == ELFCLASS32) {
        debug((CHAR16*)L"Found 32bit executable but NOT SUPPORT\n");
        return EFI_UNSUPPORTED;
    }
    else if (buffer[EI_CLASS] == ELFCLASS64) {
        debug((CHAR16*)L"Found 64bit executable\n");
    }
    else {
        debug((CHAR16*)L"Fatal Error: Invalid executable\n");
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS read_ehdr(const EFI_FILE& _elf, Elf64_Ehdr& _ehdr) {
    // 将文件指针设置为 0
    auto status = uefi_call_wrapper(_elf.SetPosition, 2, (EFI_FILE*)&_elf, 0);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error resetting file pointer position: %s\n",
              get_efi_error_message(status));
        return status;
    }
    auto buffer_read_size = sizeof(Elf64_Ehdr);
    status                = uefi_call_wrapper(_elf.Read, 3, (EFI_FILE*)&_elf,
                                              &buffer_read_size, &_ehdr);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error reading kernel header: %s\n",
              get_efi_error_message(status));
        return status;
    }
    return EFI_SUCCESS;
}

EFI_STATUS read_phdr(const EFI_FILE& _elf, size_t _phdr_offset,
                     size_t _phdr_num, Elf64_Phdr* _phdr) {
    // 将文件指针设置为 _phdr_offset
    auto status
      = uefi_call_wrapper(_elf.SetPosition, 2, (EFI_FILE*)&_elf, _phdr_offset);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error resetting file pointer position: %s\n",
              get_efi_error_message(status));
        return status;
    }

    auto buffer_read_size = sizeof(Elf64_Phdr) * _phdr_num;

    status                = uefi_call_wrapper(_elf.Read, 3, (EFI_FILE*)&_elf,
                                              &buffer_read_size, _phdr);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error reading kernel program headers: %s\n",
              get_efi_error_message(status));
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS read_shdr(const EFI_FILE& _elf, size_t _shdr_offset,
                     size_t _shdr_num, Elf64_Shdr* _shdr) {
    // 将文件指针设置为 _shdr_offset
    auto status
      = uefi_call_wrapper(_elf.SetPosition, 2, (EFI_FILE*)&_elf, _shdr_offset);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error: Error resetting file pointer position: %s\n",
              get_efi_error_message(status));
        return status;
    }

    auto buffer_read_size = sizeof(Elf64_Shdr) * _shdr_num;

    status                = uefi_call_wrapper(_elf.Read, 3, (EFI_FILE*)&_elf,
                                              &buffer_read_size, _shdr);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Error reading kernel program headers: %s\n",
              get_efi_error_message(status));
        return status;
    }

    return EFI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
