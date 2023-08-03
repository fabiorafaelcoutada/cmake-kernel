
/**
 * @file load_elf.cpp
 * @brief 加载 elf 文件
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

#include "cstring"

#include "load_elf.h"

CHAR16        error_message_buffer[256];

const CHAR16* get_efi_error_message(EFI_STATUS _status) {
    StatusToString(error_message_buffer, _status);
    return error_message_buffer;
}

void debug(const CHAR16* _fmt, ...) {
    va_list args;
    va_start(args, _fmt);
    VPrint(_fmt, args);
    va_end(args);
}

EFI_STATUS wait_for_input(EFI_INPUT_KEY* _key) {
    EFI_STATUS status;
    do {
        status
          = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, _key);
    } while (status == EFI_NOT_READY);

    return status;
}

bool check_for_fatal_error(IN EFI_STATUS const status,
                           IN const CHAR16*    error_message) {
    if (EFI_ERROR(status)) {
        /** Input key type used to capture user input. */
        EFI_INPUT_KEY input_key;

        debug(L"Fatal Error: %s: %s\n", error_message,
              get_efi_error_message(status));

        debug(L"Press any key to reboot...");
        wait_for_input(&input_key);
        return TRUE;
    }

    return FALSE;
}

EFI_STATUS
load_sections(const EFI_FILE& _elf, const Elf64_Phdr& _phdr) {
    EFI_STATUS status;
    void*      data               = nullptr;
    // 计算使用的内存页数
    uint64_t   section_page_count = EFI_SIZE_TO_PAGES(_phdr.p_memsz);

    // 设置文件偏移到 p_offset
    status = uefi_call_wrapper(_elf.SetPosition, 2, (EFI_FILE*)&_elf,
                               _phdr.p_offset);
    if (check_for_fatal_error(
          status, L"Error setting file pointer to segment offset")) {
        return status;
    }

    /// @todo 无法映射
    // print_phdr(&_phdr, 1);
    // status = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress,
    //                            EfiLoaderData, section_page_count,
    //                            (EFI_PHYSICAL_ADDRESS*)&_phdr.p_paddr);
    // debug(L"_phdr.p_paddr: [%d]\n", status);
    // if (check_for_fatal_error(status,
    //                           L"Error allocating pages for ELF segment")) {
    //     return status;
    // }

    if (_phdr.p_filesz > 0) {
        auto buffer_read_size = _phdr.p_filesz;
        // 为 program_data 分配内存
        status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                                   buffer_read_size, (void**)&data);
        if (check_for_fatal_error(status,
                                  L"Error allocating kernel segment buffer")) {
            return status;
        }
        // 读数据
        status = uefi_call_wrapper(_elf.Read, 3, (EFI_FILE*)&_elf,
                                   &buffer_read_size, (void*)data);
        if (check_for_fatal_error(status, L"Error reading segment data")) {
            return status;
        }

        // 将读出来的数据复制到其对应的物理地址
        uefi_call_wrapper(gBS->CopyMem, 3, (void*)_phdr.p_paddr, data,
                          _phdr.p_filesz);

        // 释放 program_data
        status = uefi_call_wrapper(gBS->FreePool, 1, data);
        if (check_for_fatal_error(status, L"Error freeing program section")) {
            return status;
        }
    }

    // 计算填充大小
    EFI_PHYSICAL_ADDRESS zero_fill_start = _phdr.p_paddr + _phdr.p_filesz;
    uint64_t             zero_fill_count = _phdr.p_memsz - _phdr.p_filesz;
    if (zero_fill_count > 0) {
        debug(L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
              zero_fill_count, zero_fill_start);
        // 将填充部分置 0
        uefi_call_wrapper(gBS->SetMem, 3, (void*)zero_fill_start,
                          zero_fill_count, 0);
    }

    return EFI_SUCCESS;
}

EFI_STATUS
load_program_sections(const EFI_FILE& _elf, const Elf64_Ehdr& _ehdr,
                      const Elf64_Phdr* const _phdr) {
    EFI_STATUS status;
    uint64_t   loaded = 0;

    if (_ehdr.e_phnum == 0) {
        debug(L"Fatal Error: No program segments to load in Kernel image\n");
        return EFI_INVALID_PARAMETER;
    }

    for (uint64_t i = 0; i < _ehdr.e_phnum; i++) {
        if (_phdr[i].p_type != PT_LOAD) {
            continue;
        }
        status = load_sections(_elf, _phdr[i]);
        if (EFI_ERROR(status)) {
            return status;
        }
        loaded++;
    }

    if (loaded == 0) {
        debug(
          L"Fatal Error: No loadable program segments found in Kernel image\n");
        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(const EFI_FILE&     _root_file_system,
                             const CHAR16* const _kernel_image_filename,
                             uint64_t&           _kernel_entry_point) {
    EFI_STATUS  status = EFI_SUCCESS;
    EFI_FILE*   elf    = nullptr;
    Elf64_Ehdr  ehdr   = {};
    Elf64_Phdr* phdr   = nullptr;
    Elf64_Shdr* shdr   = nullptr;

    debug(L"Reading kernel image file\n");
    status = uefi_call_wrapper(_root_file_system.Open, 5,
                               (EFI_FILE*)&_root_file_system, &elf,
                               (CHAR16*)_kernel_image_filename,
                               EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (check_for_fatal_error(status, L"Error opening kernel file")) {
        return status;
    }

    // 读取并检查 elf 头数据
    status = read_and_check_elf_identity(*elf);
    if (check_for_fatal_error(status, L"Error reading executable identity")) {
        return status;
    }

    // 读取 ehdr
    status = read_ehdr(*elf, ehdr);
    if (check_for_fatal_error(status, L"Error reading Ehdr")) {
        return status;
    }
    print_ehdr(ehdr);

    // 为 phdr 分配内存
    auto phdr_buffer_size = sizeof(Elf64_Phdr) * ehdr.e_phnum;
    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                               phdr_buffer_size, (void**)&phdr);
    if (check_for_fatal_error(status,
                              L"Error allocating kernel segment buffer")) {
        return status;
    }
    // 读取 phdr
    status = read_phdr(*elf, ehdr.e_phoff, ehdr.e_phnum, phdr);
    if (check_for_fatal_error(status, L"Error reading Phdr")) {
        return status;
    }
    print_phdr(phdr, ehdr.e_phnum);

    // 为 shdr 分配内存
    auto shdr_buffer_size = sizeof(Elf64_Shdr) * ehdr.e_shnum;
    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                               shdr_buffer_size, (void**)&shdr);
    if (check_for_fatal_error(status,
                              L"Error allocating kernel segment buffer")) {
        return status;
    }
    // 读取 shdr
    status = read_shdr(*elf, ehdr.e_shoff, ehdr.e_shnum, ehdr.e_shstrndx, shdr);
    if (check_for_fatal_error(status, L"Error reading Phdr")) {
        return status;
    }
    print_shdr(shdr, ehdr.e_shnum);

    debug(L"ehdr.e_entry: [0x%llX]\n", ehdr.e_entry);
    // 设置内核入口地址
    _kernel_entry_point = ehdr.e_entry;

    status              = load_program_sections(*elf, ehdr, phdr);
    if (EFI_ERROR(status)) {
        return status;
    }

    // 关闭文件
    status = uefi_call_wrapper(elf->Close, 1, elf);
    if (check_for_fatal_error(status, L"Error closing elf")) {
        return status;
    }

    // 释放 phdr
    status = uefi_call_wrapper(gBS->FreePool, 1, (void*)phdr);
    if (check_for_fatal_error(status, L"Error freeing phdr")) {
        return status;
    }

    // 释放 shdr
    status = uefi_call_wrapper(gBS->FreePool, 1, (void*)shdr);
    if (check_for_fatal_error(status, L"Error freeing shdr")) {
        return status;
    }

    return status;
}

#ifdef __cplusplus
}
#endif
