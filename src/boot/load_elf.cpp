
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

#include "efibind.h"
#include "elf.h"

#include "efi.h"
#include "efilib.h"

/**
 * load_segment
 */
EFI_STATUS
load_segment(IN EFI_FILE* const            _kernel_img_file,
             IN EFI_PHYSICAL_ADDRESS const _segment_file_offset,
             IN UINTN const                _segment_file_size,
             IN UINTN const                _segment_memory_size,
             IN EFI_PHYSICAL_ADDRESS const _segment_physical_address) {
    /** Program status. */
    EFI_STATUS status;
    /** Buffer to hold the segment data. */
    VOID*      program_data       = nullptr;
    /** The amount of data to read into the buffer. */
    UINTN      buffer_read_size   = 0;
    /** The number of pages to allocate. */
    UINTN      segment_page_count = EFI_SIZE_TO_PAGES(_segment_memory_size);
    /** The memory location to begin zero filling empty segment space. */
    EFI_PHYSICAL_ADDRESS zero_fill_start = 0;
    /** The number of bytes to zero fill. */
    UINTN                zero_fill_count = 0;

    status = uefi_call_wrapper((void*)_kernel_img_file->SetPosition, 2,
                               _kernel_img_file, _segment_file_offset);
    if (status != EFI_SUCCESS) {
        Print(L"Error setting file pointer to segment offset");
        return status;
    }

    status
      = uefi_call_wrapper((void*)gBS->AllocatePages, 4, AllocateAddress,
                          EfiLoaderData, segment_page_count,
                          (EFI_PHYSICAL_ADDRESS*)&_segment_physical_address);
    if (status != EFI_SUCCESS) {
        Print(L"Error allocating pages for ELF segment");
        return status;
    }

    if (_segment_file_size > 0) {
        buffer_read_size = _segment_file_size;

        status = uefi_call_wrapper((void*)gBS->AllocatePool, 3, EfiLoaderCode,
                                   buffer_read_size, (VOID**)&program_data);
        if (status != EFI_SUCCESS) {
            Print(L"Error allocating kernel segment buffer");
            return status;
        }

        status = uefi_call_wrapper((void*)_kernel_img_file->Read, 3,
                                   _kernel_img_file, &buffer_read_size,
                                   (VOID*)program_data);
        if (status != EFI_SUCCESS) {
            Print(L"Error reading segment data");
            return status;
        }

        status
          = uefi_call_wrapper((void*)gBS->CopyMem, 3, _segment_physical_address,
                              program_data, _segment_file_size);
        if (status != EFI_SUCCESS) {
            Print(L"Error copying program section into memory");
            return status;
        }

        status = uefi_call_wrapper((void*)gBS->FreePool, 1, program_data);
        if (status != EFI_SUCCESS) {
            Print(L"Error freeing program section");
            return status;
        }
    }

    // As per ELF Standard, if the size in memory is larger than the file size
    // the segment is mandated to be zero filled.
    // For more information on Refer to ELF standard page 34.
    zero_fill_start = _segment_physical_address + _segment_file_size;
    zero_fill_count = _segment_memory_size - _segment_file_size;

    if (zero_fill_count > 0) {
        status = uefi_call_wrapper((void*)gBS->SetMem, 3, zero_fill_start,
                                   zero_fill_count, 0);
        if (status != EFI_SUCCESS) {
            Print(L"Error zero filling segment");
            return status;
        }
    }

    return EFI_SUCCESS;
}

/**
 * load_program_segments
 */
EFI_STATUS
load_program_segments(IN EFI_FILE* const _kernel_img_file,
                      IN const UINT32    _file_class,
                      IN VOID* const     _kernel_header_buffer,
                      IN VOID* const     _kernel_program_headers_buffer) {
    /** Program status. */
    EFI_STATUS status;
    /** The number of program headers. */
    UINT16     n_program_headers = 0;
    /** The number of segments loaded. */
    UINT16     n_segments_loaded = 0;
    /** Program section iterator. */
    UINTN      p                 = 0;

    if (_file_class != ELFCLASS64) {
        Print(L"Fatal Error: Only 64bit ELF executables "
              "current supported\n");
        return EFI_UNSUPPORTED;
    }

    n_program_headers = ((Elf64_Ehdr*)_kernel_header_buffer)->e_phnum;

    // Exit if there are no executable sections in the kernel image.
    if (n_program_headers == 0) {
        Print(L"Fatal Error: No program segments to load ");
        Print(L"in Kernel image\n");

        return EFI_INVALID_PARAMETER;
    }

    if (_file_class != ELFCLASS64) {
        Print(L"Fatal Error: Only 64bit ELF executables "
              "current supported\n");
        return EFI_UNSUPPORTED;
    }
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
        Print(L"Fatal Error: No loadable program segments ");
        Print(L"found in Kernel image\n");

        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}

/**
 * read_elf_identity
 */
EFI_STATUS read_elf_identity(IN EFI_FILE* const _kernel_img_file,
                             OUT UINT8**        _elf_identity_buffer) {
    /** The amount of bytes to read into the buffer. */
    UINTN      buffer_read_size = EI_NIDENT;

    // Reset to the start of the file.
    EFI_STATUS status = uefi_call_wrapper((void*)_kernel_img_file->SetPosition,
                                          2, _kernel_img_file, 0);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error resetting file pointer position: %d\n", status);

        return status;
    }

    status = uefi_call_wrapper((void*)gBS->AllocatePool, 3, EfiLoaderData,
                               EI_NIDENT, (VOID**)_elf_identity_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error allocating kernel identity buffer: %d\n", status);

        return status;
    }

    status
      = uefi_call_wrapper((void*)_kernel_img_file->Read, 3, _kernel_img_file,
                          &buffer_read_size, (VOID*)*_elf_identity_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error reading kernel identity: %d\n", status);

        return status;
    }

    return EFI_SUCCESS;
}

/**
 * validate_elf_identity
 */
EFI_STATUS validate_elf_identity(IN UINT8* const _elf_identity_buffer) {
    if ((_elf_identity_buffer[EI_MAG0] != 0x7F)
        || (_elf_identity_buffer[EI_MAG1] != 0x45)
        || (_elf_identity_buffer[EI_MAG2] != 0x4C)
        || (_elf_identity_buffer[EI_MAG3] != 0x46)) {
        Print(L"Fatal Error: Invalid ELF header\n");
        return EFI_INVALID_PARAMETER;
    }

    if (_elf_identity_buffer[EI_CLASS] != ELFCLASS64) {
        Print(L"Fatal Error: Only 64bit ELF executables "
              "current supported\n");
        return EFI_UNSUPPORTED;
    }

    if (_elf_identity_buffer[EI_DATA] != 1) {
        Print(L"Fatal Error: Only LSB ELF executables "
              "current supported\n");

        return EFI_INCOMPATIBLE_VERSION;
    }

    return EFI_SUCCESS;
}

/**
 * read_elf_file
 */
EFI_STATUS
read_elf_file(IN EFI_FILE* const _kernel_img_file, IN const UINT32 _file_class,
              OUT VOID** _kernel_header_buffer,
              OUT VOID** _kernel_program_headers_buffer) {
    /** The number of bytes to read into the read buffers. */
    UINTN      buffer_read_size       = 0;
    /** The offset of the program headers in the executable. */
    UINTN      program_headers_offset = 0;

    EFI_STATUS status = uefi_call_wrapper((void*)_kernel_img_file->SetPosition,
                                          2, _kernel_img_file, 0);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error setting file pointer position: %d\n", status);

        return status;
    }

    if (_file_class != ELFCLASS64) {
        Print(L"Fatal Error: Only 64bit ELF executables "
              "current supported\n");
        return EFI_UNSUPPORTED;
    }
    buffer_read_size = sizeof(Elf64_Ehdr);

    status = uefi_call_wrapper((void*)gBS->AllocatePool, 3, EfiLoaderData,
                               buffer_read_size, _kernel_header_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error allocating kernel header buffer: %d\n", status);

        return status;
    }

    status
      = uefi_call_wrapper((void*)_kernel_img_file->Read, 3, _kernel_img_file,
                          &buffer_read_size, *_kernel_header_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error reading kernel header: %d\n", status);

        return status;
    }

    program_headers_offset = ((Elf64_Ehdr*)*_kernel_header_buffer)->e_phoff;
    buffer_read_size
      = sizeof(Elf64_Phdr) * ((Elf64_Ehdr*)*_kernel_header_buffer)->e_phnum;

    // Read program headers.
    status = uefi_call_wrapper((void*)_kernel_img_file->SetPosition, 2,
                               _kernel_img_file, program_headers_offset);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error setting file pointer position: %d\n", status);

        return status;
    }

    // Allocate memory for program headers.
    status
      = uefi_call_wrapper((void*)gBS->AllocatePool, 3, EfiLoaderData,
                          buffer_read_size, _kernel_program_headers_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error allocating kernel "
              "program header buffer: %d\n",
              status);

        return status;
    }

    status
      = uefi_call_wrapper((void*)_kernel_img_file->Read, 3, _kernel_img_file,
                          &buffer_read_size, *_kernel_program_headers_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Error reading kernel program headers: %d\n", status);

        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(IN EFI_FILE* const        _root_file_system,
                             IN CHAR16* const          _kernel_image_filename,
                             OUT EFI_PHYSICAL_ADDRESS* _kernel_entry_point) {
    /** Program status. */
    EFI_STATUS status;
    /** The kernel file handle. */
    EFI_FILE*  kernel_img_file;
    /** The kernel ELF header buffer. */
    VOID*      kernel_header          = nullptr;
    /** The kernel program headers buffer. */
    VOID*      kernel_program_headers = nullptr;
    /** The ELF file identity buffer. */
    UINT8*     elf_identity_buffer    = nullptr;
    /** The ELF file class. */
    UINT32     file_class             = ELFCLASSNONE;

    status
      = uefi_call_wrapper((void*)_root_file_system->Open, 5, _root_file_system,
                          &kernel_img_file, _kernel_image_filename,
                          EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status != EFI_SUCCESS) {
        Print(L"Error opening kernel file");
        return status;
    }

    // Read ELF Identity.
    // From here we can validate the ELF executable, as well as determine the
    // file class.
    status = read_elf_identity(kernel_img_file, &elf_identity_buffer);
    if (status != EFI_SUCCESS) {
        Print(L"Error reading executable identity");
        return status;
    }

    file_class = elf_identity_buffer[EI_CLASS];

    // Validate the ELF file.
    status     = validate_elf_identity(elf_identity_buffer);
    if (EFI_ERROR(status)) {
        Print(L"111\n");
        return status;
    }

    // Free identity buffer.
    status = uefi_call_wrapper((void*)gBS->FreePool, 1, elf_identity_buffer);
    if (status != EFI_SUCCESS) {
        Print(L"Error freeing kernel identity buffer");
        return status;
    }

    // Read the ELF file and program headers.
    status = read_elf_file(kernel_img_file, file_class, &kernel_header,
                           &kernel_program_headers);
    if (status != EFI_SUCCESS) {
        Print(L"Error reading ELF file");
        return status;
    }

    if (file_class != ELFCLASS64) {
        Print(L"Fatal Error: Only 64bit ELF executables "
              "current supported\n");
        return EFI_UNSUPPORTED;
    }
    *_kernel_entry_point = ((Elf64_Ehdr*)kernel_header)->e_entry;

    status = load_program_segments(kernel_img_file, file_class, kernel_header,
                                   kernel_program_headers);
    if (EFI_ERROR(status)) {
        // In the case that loading the kernel segments failed, the error
        // message will have already been printed.
        Print(L"222\n");
        return status;
    }

    status
      = uefi_call_wrapper((void*)kernel_img_file->Close, 1, kernel_img_file);
    if (status != EFI_SUCCESS) {
        Print(L"Error closing kernel image");
        return status;
    }

    status = uefi_call_wrapper((void*)gBS->FreePool, 1, (VOID*)kernel_header);
    if (status != EFI_SUCCESS) {
        Print(L"Error freeing kernel header buffer");
        return status;
    }

    status = uefi_call_wrapper((void*)gBS->FreePool, 1,
                               (VOID*)kernel_program_headers);
    if (status != EFI_SUCCESS) {
        Print(L"Error freeing kernel program headers buffer");
        return status;
    }

    return status;
}
