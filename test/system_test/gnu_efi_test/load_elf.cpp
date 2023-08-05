
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

#include "cstring"

#include "load_elf.h"
#include <memory>

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

    status = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress,
                               EfiLoaderData, section_page_count,
                               (EFI_PHYSICAL_ADDRESS*)&_phdr.p_paddr);
    debug(L"_phdr.p_paddr: [%d] [%d]\n", status, section_page_count);
    if (check_for_fatal_error(status,
                              L"Error allocating pages for ELF segment")) {
        return status;
    }

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
    EFI_STATUS status      = EFI_SUCCESS;
    uint8_t*   file_buffer = nullptr;

    // 打开 elf 文件
    debug(L"Reading kernel image file\n");
    EFI_FILE* elf = nullptr;
    status        = uefi_call_wrapper(_root_file_system.Open, 5,
                                      (EFI_FILE*)&_root_file_system, &elf,
                                      (CHAR16*)_kernel_image_filename,
                                      EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (check_for_fatal_error(status, L"Error opening kernel file")) {
        return status;
    }

    // 获取内核文件大小
    auto get_file_size_ret = get_file_size(*elf);
    if (check_for_fatal_error(get_file_size_ret.first,
                              L"Error get_file_size")) {
        return get_file_size_ret.first;
    }
    debug(L"Kernel file size: %llu\n", get_file_size_ret.second);

    // 分配 elf 文件缓存
    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData,
                               get_file_size_ret.second, (void**)&file_buffer);
    if (check_for_fatal_error(status, L"Error allocating kernel buffer")) {
        return status;
    }
    // 将内核文件读入内存
    auto kernel_file_size = get_file_size_ret.second;
    status = uefi_call_wrapper(elf->Read, 3, (EFI_FILE*)elf, &kernel_file_size,
                               file_buffer);
    if (EFI_ERROR(status)) {
        debug(L"Error reading kernel %s\n", get_efi_error_message(status));
        return status;
    }

    // 检查 elf 头数据
    status = check_elf_identity(file_buffer);
    if (EFI_ERROR(status)) {
        debug(L"NOT valid ELF file %s\n", get_efi_error_message(status));
        return status;
    }

    // 读取 ehdr
    auto ehdr = get_ehdr(file_buffer);
    print_ehdr(ehdr);
    // 读取 phdr
    auto phdr = get_phdr(ehdr.e_phoff, file_buffer);
    print_phdr(phdr, ehdr.e_phnum);
    // 读取 shdr
    auto shdr = get_shdr(ehdr.e_shoff, file_buffer);
    // 将 shstrtab 读入 shstrtab_buf
    read_section(file_buffer, shdr[ehdr.e_shstrndx], shstrtab_buf);
    print_shdr(shdr, ehdr.e_shnum);

    debug(L"ehdr.e_entry: [0x%llX]\n", ehdr.e_entry);
    // 设置内核入口地址
    _kernel_entry_point = ehdr.e_entry;

    // status              = load_program_sections(*elf, ehdr, phdr);
    // if (EFI_ERROR(status)) {
    //     return status;
    // }

    // 释放 elf 文件缓存
    status = uefi_call_wrapper(gBS->FreePool, 1, (void*)file_buffer);
    if (check_for_fatal_error(status, L"Error freeing file_buffer")) {
        return status;
    }

    // auto a = std::make_shared<int>;

    // 关闭 elf 文件
    status = uefi_call_wrapper(elf->Close, 1, elf);
    if (check_for_fatal_error(status, L"Error closing elf")) {
        return status;
    }

    return status;
}
