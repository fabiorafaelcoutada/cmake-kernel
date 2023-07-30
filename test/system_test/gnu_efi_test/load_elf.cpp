
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

        debug((CHAR16*)L"Fatal Error: %s: %s\n", error_message,
              get_efi_error_message(status));

        debug((CHAR16*)L"Press any key to reboot...");
        wait_for_input(&input_key);
        return TRUE;
    }

    return FALSE;
}

EFI_STATUS
load_segment(IN EFI_FILE* const            _kernel_img_file,
             IN EFI_PHYSICAL_ADDRESS const _segment_file_offset,
             IN const uint64_t             _segment_file_size,
             IN const uint64_t             _segment_memory_size,
             IN EFI_PHYSICAL_ADDRESS const _segment_physical_address) {
    /** Program status. */
    EFI_STATUS status;
    /** Buffer to hold the segment data. */
    void*      program_data       = nullptr;
    /** The amount of data to read into the buffer. */
    uint64_t   buffer_read_size   = 0;
    /** The number of pages to allocate. */
    uint64_t   segment_page_count = EFI_SIZE_TO_PAGES(_segment_memory_size);
    /** The memory location to begin zero filling empty segment space. */
    EFI_PHYSICAL_ADDRESS zero_fill_start = 0;
    /** The number of bytes to zero fill. */
    uint64_t             zero_fill_count = 0;

    debug((CHAR16*)L"Debug: Setting file pointer to segment "
                   "offset '0x%llx'\n",
          _segment_file_offset);

    status = uefi_call_wrapper(_kernel_img_file->SetPosition, 2,
                               _kernel_img_file, _segment_file_offset);
    if (check_for_fatal_error(
          status, L"Error setting file pointer to segment offset")) {
        return status;
    }

    debug((CHAR16*)L"Debug: Allocating %lu pages at address '0x%llx'\n",
          segment_page_count, _segment_physical_address);

    status
      = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress, EfiLoaderData,
                          segment_page_count,
                          (EFI_PHYSICAL_ADDRESS*)&_segment_physical_address);
    if (check_for_fatal_error(status,
                              L"Error allocating pages for ELF segment")) {
        return status;
    }

    if (_segment_file_size > 0) {
        buffer_read_size = _segment_file_size;

        debug((CHAR16*)L"Debug: Allocating segment buffer with size '0x%llx'\n",
              buffer_read_size);

        status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                                   buffer_read_size, (void**)&program_data);
        if (check_for_fatal_error(status,
                                  L"Error allocating kernel segment buffer")) {
            return status;
        }

        debug((CHAR16*)L"Debug: Reading segment data with file size '0x%llx'\n",
              buffer_read_size);

        status = uefi_call_wrapper(_kernel_img_file->Read, 3, _kernel_img_file,
                                   &buffer_read_size, (void*)program_data);
        if (check_for_fatal_error(status, L"Error reading segment data")) {
            return status;
        }

        debug((CHAR16*)L"Debug: Copying segment to memory address '0x%llx'\n",
              _segment_physical_address);

        uefi_call_wrapper(gBS->CopyMem, 3, (void*)_segment_physical_address,
                          program_data, _segment_file_size);

        debug((CHAR16*)L"Debug: Freeing program section data buffer\n");

        status = uefi_call_wrapper(gBS->FreePool, 1, program_data);
        if (check_for_fatal_error(status, L"Error freeing program section")) {
            return status;
        }
    }

    // As per ELF Standard, if the size in memory is larger than the file
    // size the segment is mandated to be zero filled. For more information
    // on Refer to ELF standard page 34.
    zero_fill_start = _segment_physical_address + _segment_file_size;
    zero_fill_count = _segment_memory_size - _segment_file_size;

    if (zero_fill_count > 0) {
        debug((CHAR16*)L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
              zero_fill_count, zero_fill_start);

        uefi_call_wrapper(gBS->SetMem, 3, (void*)zero_fill_start,
                          zero_fill_count, 0);
    }

    return EFI_SUCCESS;
}

/**
 * load_program_segments
 */
EFI_STATUS
load_program_segments(IN EFI_FILE* const _kernel_img_file,
                      IN void* const     _kernel_header_buffer,
                      IN void* const     _kernel_program_headers_buffer) {
    /** Program status. */
    EFI_STATUS status;
    /** The number of program headers. */
    UINT16     n_program_headers = 0;
    /** The number of segments loaded. */
    UINT16     n_segments_loaded = 0;
    /** Program section iterator. */
    uint64_t   p                 = 0;

    n_program_headers = ((Elf64_Ehdr*)_kernel_header_buffer)->e_phnum;

    // Exit if there are no executable sections in the kernel image.
    if (n_program_headers == 0) {
        debug((CHAR16*)L"Fatal Error: No program segments to load ");
        debug((CHAR16*)L"in Kernel image\n");

        return EFI_INVALID_PARAMETER;
    }

    debug((CHAR16*)L"Debug: Loading %u segments\n", n_program_headers);

    /** Program headers pointer. */
    Elf64_Phdr* program_headers = (Elf64_Phdr*)_kernel_program_headers_buffer;

    for (p = 0; p < n_program_headers; p++) {
        if (program_headers[p].p_type == PT_LOAD) {
            status = load_segment(_kernel_img_file, program_headers[p].p_offset,
                                  program_headers[p].p_filesz,
                                  program_headers[p].p_memsz,
                                  program_headers[p].p_paddr);
            if (EFI_ERROR(status)) {
                return status;
            }

            n_segments_loaded++;
        }
    }

    // If we have found no loadable segments, raise an exception.
    if (n_segments_loaded == 0) {
        debug((CHAR16*)L"Fatal Error: No loadable program segments ");
        debug((CHAR16*)L"found in Kernel image\n");

        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(EFI_FILE* const       _root_file_system,
                             CHAR16* const         _kernel_image_filename,
                             EFI_PHYSICAL_ADDRESS* _kernel_entry_point) {
    EFI_STATUS  status = EFI_SUCCESS;
    EFI_FILE*   elf    = nullptr;
    Elf64_Ehdr  ehdr   = {};
    Elf64_Phdr* phdr   = nullptr;
    Elf64_Shdr* shdr   = nullptr;

    debug((CHAR16*)L"Reading kernel image file\n");
    status = uefi_call_wrapper(_root_file_system->Open, 5, _root_file_system,
                               &elf, _kernel_image_filename, EFI_FILE_MODE_READ,
                               EFI_FILE_READ_ONLY);
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
    status = read_shdr(*elf, ehdr.e_shoff, ehdr.e_shnum, shdr);
    if (check_for_fatal_error(status, L"Error reading Phdr")) {
        return status;
    }
    print_shdr(shdr, ehdr.e_shnum);

    // Set the kernel entry point to the address specified in the ELF
    // header.
    *_kernel_entry_point = ehdr.e_entry;

    status               = load_program_segments(elf, &ehdr, &phdr);
    if (EFI_ERROR(status)) {
        // In the case that loading the kernel segments failed, the error
        // message will have already been printed.
        return status;
    }

    // Cleanup.
    debug((CHAR16*)L"Closing kernel binary\n");

    status = uefi_call_wrapper(elf->Close, 1, elf);
    if (check_for_fatal_error(status, L"Error closing kernel image")) {
        return status;
    }

    debug((CHAR16*)L"Freeing kernel header buffer\n");

    status = uefi_call_wrapper(gBS->FreePool, 1, (void*)&ehdr);
    if (check_for_fatal_error(status, L"Error freeing kernel header buffer")) {
        return status;
    }

    debug((CHAR16*)L"Freeing kernel program header buffer\n");

    status = uefi_call_wrapper(gBS->FreePool, 1, (void*)&phdr);
    if (check_for_fatal_error(status,
                              L"Error freeing kernel program headers buffer")) {
        return status;
    }

    return status;
}

#ifdef __cplusplus
}
#endif
