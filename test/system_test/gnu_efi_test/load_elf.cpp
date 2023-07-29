
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

#include "efibind.h"
#include "elf.h"

#include "efi.h"
#include "efilib.h"

CHAR16        error_message_buffer[256];

/**
 * get_efi_error_message
 */
const CHAR16* get_efi_error_message(IN EFI_STATUS const status) {
    // Copy the status into the error message buffer.
    StatusToString(error_message_buffer, status);
    return error_message_buffer;
}

#define MAX_SERIAL_OUT_STRING_LENGTH 256

/**
 * debug_print_line
 */
EFI_STATUS debug_print_line(IN CHAR16* fmt, ...) {
    /** Main bootloader application status. */
    EFI_STATUS status;
    /** The variadic argument list passed to the VSPrintf function. */
    va_list    args;
    /**
     * The output message buffer.
     * The string content is copied into this buffer. The maximum length is set
     * to the maximum serial buffer length.
     */
    CHAR16     output_message[MAX_SERIAL_OUT_STRING_LENGTH];

    va_start(args, fmt);

    VPrint(fmt, args);

    va_end(args);

    return EFI_SUCCESS;
};

/**
 * print_elf_file_info
 */
VOID print_elf_file_info(IN VOID* const header_ptr,
                         IN VOID* const program_headers_ptr) {
    /**
     * The header pointer cast to a 32bit ELF Header so that we can read the
     * header and determine the file type. Then cast to a 64bit header for
     * specific fields if necessary.
     */
    Elf32_Ehdr* header   = (Elf32_Ehdr*)header_ptr;
    Elf64_Ehdr* header64 = (Elf64_Ehdr*)header_ptr;

    debug_print_line(L"Debug: ELF Header Info:\n");

    debug_print_line(L"  Magic:                    ");
    UINTN i = 0;
    for (i = 0; i < 4; i++) {
        debug_print_line(L"0x%x ", header->e_ident[i]);
    }
    debug_print_line(L"\n");

    debug_print_line(L"  Class:                    ");
    if (header->e_ident[EI_CLASS] == ELFCLASS32) {
        debug_print_line(L"32bit");
    }
    else if (header->e_ident[EI_CLASS] == ELFCLASS64) {
        debug_print_line(L"64bit");
    }
    debug_print_line(L"\n");

    debug_print_line(L"  Endianness:               ");
    if (header->e_ident[EI_DATA] == 1) {
        debug_print_line(L"Little-Endian");
    }
    else if (header->e_ident[EI_DATA] == 2) {
        debug_print_line(L"Big-Endian");
    }
    debug_print_line(L"\n");

    debug_print_line(L"  Version:                  0x%x\n",
                     header->e_ident[EI_VERSION]);

    debug_print_line(L"  OS ABI:                   ");
    if (header->e_ident[EI_OSABI] == 0x00) {
        debug_print_line(L"System V");
    }
    else if (header->e_ident[EI_OSABI] == 0x01) {
        debug_print_line(L"HP-UX");
    }
    else if (header->e_ident[EI_OSABI] == 0x02) {
        debug_print_line(L"NetBSD");
    }
    else if (header->e_ident[EI_OSABI] == 0x03) {
        debug_print_line(L"Linux");
    }
    else if (header->e_ident[EI_OSABI] == 0x04) {
        debug_print_line(L"GNU Hurd");
    }
    else if (header->e_ident[EI_OSABI] == 0x06) {
        debug_print_line(L"Solaris");
    }
    else if (header->e_ident[EI_OSABI] == 0x07) {
        debug_print_line(L"AIX");
    }
    else if (header->e_ident[EI_OSABI] == 0x08) {
        debug_print_line(L"IRIX");
    }
    else if (header->e_ident[EI_OSABI] == 0x09) {
        debug_print_line(L"FreeBSD");
    }
    else if (header->e_ident[EI_OSABI] == 0x0A) {
        debug_print_line(L"Tru64");
    }
    else if (header->e_ident[EI_OSABI] == 0x0B) {
        debug_print_line(L"Novell Modesto");
    }
    else if (header->e_ident[EI_OSABI] == 0x0C) {
        debug_print_line(L"OpenBSD");
    }
    else if (header->e_ident[EI_OSABI] == 0x0D) {
        debug_print_line(L"OpenVMS");
    }
    else if (header->e_ident[EI_OSABI] == 0x0E) {
        debug_print_line(L"NonStop Kernel");
    }
    else if (header->e_ident[EI_OSABI] == 0x0F) {
        debug_print_line(L"AROS");
    }
    else if (header->e_ident[EI_OSABI] == 0x10) {
        debug_print_line(L"Fenix OS");
    }
    else if (header->e_ident[EI_OSABI] == 0x11) {
        debug_print_line(L"CloudABI");
    }
    debug_print_line(L"\n");

    debug_print_line(L"  File Type:                ");
    if (header->e_type == 0x00) {
        debug_print_line(L"None");
    }
    else if (header->e_type == 0x01) {
        debug_print_line(L"Relocatable");
    }
    else if (header->e_type == 0x02) {
        debug_print_line(L"Executable");
    }
    else if (header->e_type == 0x03) {
        debug_print_line(L"Dynamic");
    }
    else {
        debug_print_line(L"Other");
    }
    debug_print_line(L"\n");

    debug_print_line(L"  Machine Type:             ");
    if (header->e_machine == 0x00) {
        debug_print_line(L"No specific instruction set");
    }
    else if (header->e_machine == 0x02) {
        debug_print_line(L"SPARC");
    }
    else if (header->e_machine == 0x03) {
        debug_print_line(L"x86");
    }
    else if (header->e_machine == 0x08) {
        debug_print_line(L"MIPS");
    }
    else if (header->e_machine == 0x14) {
        debug_print_line(L"PowerPC");
    }
    else if (header->e_machine == 0x16) {
        debug_print_line(L"S390");
    }
    else if (header->e_machine == 0x28) {
        debug_print_line(L"ARM");
    }
    else if (header->e_machine == 0x2A) {
        debug_print_line(L"SuperH");
    }
    else if (header->e_machine == 0x32) {
        debug_print_line(L"IA-64");
    }
    else if (header->e_machine == 0x3E) {
        debug_print_line(L"x86-64");
    }
    else if (header->e_machine == 0xB7) {
        debug_print_line(L"AArch64");
    }
    else if (header->e_machine == 0xF3) {
        debug_print_line(L"RISC-V");
    }
    debug_print_line(L"\n");

    if (header->e_ident[EI_CLASS] == ELFCLASS32) {
        debug_print_line(L"  Entry point:              0x%lx\n",
                         header->e_entry);
        debug_print_line(L"  Program header offset:    0x%lx\n",
                         header->e_phoff);
        debug_print_line(L"  Section header offset:    0x%lx\n",
                         header->e_shoff);
        debug_print_line(L"  Program header count:     %u\n", header->e_phnum);
        debug_print_line(L"  Section header count:     %u\n", header->e_shnum);

        Elf32_Phdr* program_headers = program_headers_ptr;

        debug_print_line(L"\nProgram Headers:\n");
        UINTN p = 0;
        for (p = 0; p < header->e_phnum; p++) {
            debug_print_line(L"[%u]:\n", p);
            debug_print_line(L"  p_type:      0x%lx\n",
                             program_headers[p].p_type);
            debug_print_line(L"  p_offset:    0x%lx\n",
                             program_headers[p].p_offset);
            debug_print_line(L"  p_vaddr:     0x%lx\n",
                             program_headers[p].p_vaddr);
            debug_print_line(L"  p_paddr:     0x%lx\n",
                             program_headers[p].p_paddr);
            debug_print_line(L"  p_filesz:    0x%lx\n",
                             program_headers[p].p_filesz);
            debug_print_line(L"  p_memsz:     0x%lx\n",
                             program_headers[p].p_memsz);
            debug_print_line(L"  p_flags:     0x%lx\n",
                             program_headers[p].p_flags);
            debug_print_line(L"  p_align:     0x%lx\n",
                             program_headers[p].p_align);
            debug_print_line(L"\n");
        }
    }
    else if (header->e_ident[EI_CLASS] == ELFCLASS64) {
        debug_print_line(L"  Entry point:              0x%llx\n",
                         header64->e_entry);
        debug_print_line(L"  Program header offset:    0x%llx\n",
                         header64->e_phoff);
        debug_print_line(L"  Section header offset:    0x%llx\n",
                         header64->e_shoff);
        debug_print_line(L"  Program header count:     %u\n",
                         header64->e_phnum);
        debug_print_line(L"  Section header count:     %u\n",
                         header64->e_shnum);

        Elf64_Phdr* program_headers = program_headers_ptr;

        debug_print_line(L"\nDebug: Program Headers:\n");
        UINTN p = 0;
        for (p = 0; p < header64->e_phnum; p++) {
            debug_print_line(L"[%u]:\n", p);
            debug_print_line(L"  p_type:      0x%lx\n",
                             program_headers[p].p_type);
            debug_print_line(L"  p_flags:     0x%lx\n",
                             program_headers[p].p_flags);
            debug_print_line(L"  p_offset:    0x%llx\n",
                             program_headers[p].p_offset);
            debug_print_line(L"  p_vaddr:     0x%llx\n",
                             program_headers[p].p_vaddr);
            debug_print_line(L"  p_paddr:     0x%llx\n",
                             program_headers[p].p_paddr);
            debug_print_line(L"  p_filesz:    0x%llx\n",
                             program_headers[p].p_filesz);
            debug_print_line(L"  p_memsz:     0x%llx\n",
                             program_headers[p].p_memsz);
            debug_print_line(L"  p_align:     0x%llx\n",
                             program_headers[p].p_align);
            debug_print_line(L"\n");
        }
    }
}

/**
 * read_elf_file
 */
EFI_STATUS
read_elf_file(IN EFI_FILE* const kernel_img_file, IN const UINT32 file_class,
              OUT VOID** kernel_header_buffer,
              OUT VOID** kernel_program_headers_buffer) {
    /** The number of bytes to read into the read buffers. */
    UINTN buffer_read_size       = 0;
    /** The offset of the program headers in the executable. */
    UINTN program_headers_offset = 0;

// Reset to start of file.
#ifdef DEBUG
    debug_print_line(L"Debug: Setting file pointer to "
                     "read executable header\n");
#endif

    EFI_STATUS status
      = uefi_call_wrapper(kernel_img_file->SetPosition, 2, kernel_img_file, 0);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error setting file pointer position: %s\n",
                         get_efi_error_message(status));

        return status;
    }

    if (file_class == ELFCLASS32) {
        buffer_read_size = sizeof(Elf32_Ehdr);
    }
    else if (file_class == ELFCLASS64) {
        buffer_read_size = sizeof(Elf64_Ehdr);
    }
    else {
        debug_print_line(L"Error: Invalid file class\n", status);
        return EFI_INVALID_PARAMETER;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Allocating '0x%lx' for ", buffer_read_size);
    debug_print_line(L"kernel executable header buffer\n");
#endif

    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData,
                               buffer_read_size, kernel_header_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error allocating kernel header buffer: %s\n",
                         get_efi_error_message(status));

        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Reading kernel executable header\n");
#endif

    status = uefi_call_wrapper(kernel_img_file->Read, 3, kernel_img_file,
                               &buffer_read_size, *kernel_header_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error reading kernel header: %s\n",
                         get_efi_error_message(status));

        return status;
    }

    if (file_class == ELFCLASS32) {
        program_headers_offset = ((Elf32_Ehdr*)*kernel_header_buffer)->e_phoff;
        buffer_read_size
          = sizeof(Elf32_Phdr) * ((Elf32_Ehdr*)*kernel_header_buffer)->e_phnum;
    }
    else if (file_class == ELFCLASS64) {
        program_headers_offset = ((Elf64_Ehdr*)*kernel_header_buffer)->e_phoff;
        buffer_read_size
          = sizeof(Elf64_Phdr) * ((Elf64_Ehdr*)*kernel_header_buffer)->e_phnum;
    }

// Read program headers.
#ifdef DEBUG
    debug_print_line(L"Debug: Setting file offset to '0x%lx' "
                     "to read program headers\n",
                     program_headers_offset);
#endif

    status = uefi_call_wrapper(kernel_img_file->SetPosition, 2, kernel_img_file,
                               program_headers_offset);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error setting file pointer position: %s\n",
                         get_efi_error_message(status));

        return status;
    }

// Allocate memory for program headers.
#ifdef DEBUG
    debug_print_line(L"Debug: Allocating '0x%lx' for program headers buffer\n",
                     buffer_read_size);
#endif

    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData,
                               buffer_read_size, kernel_program_headers_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error allocating kernel "
                         "program header buffer: %s\n",
                         get_efi_error_message(status));

        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Reading program headers\n");
#endif

    status
      = uefi_call_wrapper(kernel_img_file->Read, 3, kernel_img_file,
                          &buffer_read_size, *kernel_program_headers_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error reading kernel program headers: %s\n",
                         get_efi_error_message(status));

        return status;
    }

    return EFI_SUCCESS;
}

/**
 * read_elf_identity
 */
EFI_STATUS read_elf_identity(IN EFI_FILE* const kernel_img_file,
                             OUT UINT8**        elf_identity_buffer) {
    /** The amount of bytes to read into the buffer. */
    UINTN buffer_read_size = EI_NIDENT;

#ifdef DEBUG
    debug_print_line(L"Debug: Setting file pointer position "
                     "to read ELF identity\n");
#endif

    // Reset to the start of the file.
    EFI_STATUS status
      = uefi_call_wrapper(kernel_img_file->SetPosition, 2, kernel_img_file, 0);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error resetting file pointer position: %s\n",
                         get_efi_error_message(status));

        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Allocating buffer for ELF identity\n");
#endif

    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData, EI_NIDENT,
                               (VOID**)elf_identity_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(
          L"Error: Error allocating kernel identity buffer: %s\n",
          get_efi_error_message(status));

        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Reading ELF identity\n");
#endif

    status = uefi_call_wrapper(kernel_img_file->Read, 3, kernel_img_file,
                               &buffer_read_size, (VOID*)*elf_identity_buffer);
    if (EFI_ERROR(status)) {
        debug_print_line(L"Error: Error reading kernel identity: %s\n",
                         get_efi_error_message(status));

        return status;
    }

    return EFI_SUCCESS;
}

/**
 * validate_elf_identity
 */
EFI_STATUS validate_elf_identity(IN UINT8* const elf_identity_buffer) {
    if ((elf_identity_buffer[EI_MAG0] != 0x7F)
        || (elf_identity_buffer[EI_MAG1] != 0x45)
        || (elf_identity_buffer[EI_MAG2] != 0x4C)
        || (elf_identity_buffer[EI_MAG3] != 0x46)) {
        debug_print_line(L"Fatal Error: Invalid ELF header\n");
        return EFI_INVALID_PARAMETER;
    }

    if (elf_identity_buffer[EI_CLASS] == ELFCLASS32) {
#ifdef DEBUG
        debug_print_line(L"Debug: Found 32bit executable\n");
#endif
    }
    else if (elf_identity_buffer[EI_CLASS] == ELFCLASS64) {
#ifdef DEBUG
        debug_print_line(L"Debug: Found 64bit executable\n");
#endif
    }
    else {
        debug_print_line(L"Fatal Error: Invalid executable\n");

        return EFI_UNSUPPORTED;
    }

    if (elf_identity_buffer[EI_DATA] != 1) {
        debug_print_line(L"Fatal Error: Only LSB ELF executables "
                         "current supported\n");

        return EFI_INCOMPATIBLE_VERSION;
    }

    return EFI_SUCCESS;
}

/**
 * check_for_fatal_error
 */
bool check_for_fatal_error(IN EFI_STATUS const status,
                           IN const CHAR16*    error_message) {
    if (EFI_ERROR(status)) {
        /** Input key type used to capture user input. */
        EFI_INPUT_KEY input_key;

        // debug_print_line(L"Fatal Error: %s: %s\n", error_message,
        //                  get_efi_error_message(status));

#if PROMPT_FOR_INPUT_BEFORE_REBOOT_ON_FATAL_ERROR
        debug_print_line(L"Press any key to reboot...");
        wait_for_input(&input_key);
#endif

        return TRUE;
    }

    return FALSE;
}

/**
 * load_segment
 */
EFI_STATUS
load_segment(IN EFI_FILE* const            kernel_img_file,
             IN EFI_PHYSICAL_ADDRESS const segment_file_offset,
             IN UINTN const                segment_file_size,
             IN UINTN const                segment_memory_size,
             IN EFI_PHYSICAL_ADDRESS const segment_physical_address) {
    /** Program status. */
    EFI_STATUS status;
    /** Buffer to hold the segment data. */
    VOID*      program_data       = NULL;
    /** The amount of data to read into the buffer. */
    UINTN      buffer_read_size   = 0;
    /** The number of pages to allocate. */
    UINTN      segment_page_count = EFI_SIZE_TO_PAGES(segment_memory_size);
    /** The memory location to begin zero filling empty segment space. */
    EFI_PHYSICAL_ADDRESS zero_fill_start = 0;
    /** The number of bytes to zero fill. */
    UINTN                zero_fill_count = 0;

#ifdef DEBUG
    debug_print_line(L"Debug: Setting file pointer to segment "
                     "offset '0x%llx'\n",
                     segment_file_offset);
#endif

    status = uefi_call_wrapper(kernel_img_file->SetPosition, 2, kernel_img_file,
                               segment_file_offset);
    if (check_for_fatal_error(
          status, L"Error setting file pointer to segment offset")) {
        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Allocating %lu pages at address '0x%llx'\n",
                     segment_page_count, segment_physical_address);
#endif

    status
      = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress, EfiLoaderData,
                          segment_page_count,
                          (EFI_PHYSICAL_ADDRESS*)&segment_physical_address);
    if (check_for_fatal_error(status,
                              L"Error allocating pages for ELF segment")) {
        return status;
    }

    if (segment_file_size > 0) {
        buffer_read_size = segment_file_size;

#ifdef DEBUG
        debug_print_line(
          L"Debug: Allocating segment buffer with size '0x%llx'\n",
          buffer_read_size);
#endif

        status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                                   buffer_read_size, (VOID**)&program_data);
        if (check_for_fatal_error(status,
                                  L"Error allocating kernel segment buffer")) {
            return status;
        }

#ifdef DEBUG
        debug_print_line(
          L"Debug: Reading segment data with file size '0x%llx'\n",
          buffer_read_size);
#endif

        status = uefi_call_wrapper(kernel_img_file->Read, 3, kernel_img_file,
                                   &buffer_read_size, (VOID*)program_data);
        if (check_for_fatal_error(status, L"Error reading segment data")) {
            return status;
        }

#ifdef DEBUG
        debug_print_line(L"Debug: Copying segment to memory address '0x%llx'\n",
                         segment_physical_address);
#endif

        uefi_call_wrapper(gBS->CopyMem, 3, segment_physical_address,
                          program_data, segment_file_size);

#ifdef DEBUG
        debug_print_line(L"Debug: Freeing program section data buffer\n");
#endif

        status = uefi_call_wrapper(gBS->FreePool, 1, program_data);
        if (check_for_fatal_error(status, L"Error freeing program section")) {
            return status;
        }
    }

    // As per ELF Standard, if the size in memory is larger than the file size
    // the segment is mandated to be zero filled.
    // For more information on Refer to ELF standard page 34.
    zero_fill_start = segment_physical_address + segment_file_size;
    zero_fill_count = segment_memory_size - segment_file_size;

    if (zero_fill_count > 0) {
#ifdef DEBUG
        debug_print_line(
          L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
          zero_fill_count, zero_fill_start);
#endif

        uefi_call_wrapper(gBS->SetMem, 3, zero_fill_start, zero_fill_count, 0);
    }

    return EFI_SUCCESS;
}

/**
 * load_program_segments
 */
EFI_STATUS load_program_segments(IN EFI_FILE* const kernel_img_file,
                                 IN const UINT32    file_class,
                                 IN VOID* const     kernel_header_buffer,
                                 IN VOID* const kernel_program_headers_buffer) {
    /** Program status. */
    EFI_STATUS status;
    /** The number of program headers. */
    UINT16     n_program_headers = 0;
    /** The number of segments loaded. */
    UINT16     n_segments_loaded = 0;
    /** Program section iterator. */
    UINTN      p                 = 0;

    if (file_class == ELFCLASS32) {
        n_program_headers = ((Elf32_Ehdr*)kernel_header_buffer)->e_phnum;
    }
    else if (file_class == ELFCLASS64) {
        n_program_headers = ((Elf64_Ehdr*)kernel_header_buffer)->e_phnum;
    }

    // Exit if there are no executable sections in the kernel image.
    if (n_program_headers == 0) {
        debug_print_line(L"Fatal Error: No program segments to load ");
        debug_print_line(L"in Kernel image\n");

        return EFI_INVALID_PARAMETER;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Loading %u segments\n", n_program_headers);
#endif

    if (file_class == ELFCLASS32) {
        /** Program headers pointer. */
        Elf32_Phdr* program_headers
          = (Elf32_Phdr*)kernel_program_headers_buffer;

        for (p = 0; p < n_program_headers; p++) {
            if (program_headers[p].p_type == PT_LOAD) {
                status
                  = load_segment(kernel_img_file, program_headers[p].p_offset,
                                 program_headers[p].p_filesz,
                                 program_headers[p].p_memsz,
                                 program_headers[p].p_paddr);
                if (EFI_ERROR(status)) {
                    // Error has already been printed in the case that loading
                    // an individual segment failed.
                    return status;
                }

                // Increment the number of segments lodaed.
                n_segments_loaded++;
            }
        }
    }
    else if (file_class == ELFCLASS64) {
        /** Program headers pointer. */
        Elf64_Phdr* program_headers
          = (Elf64_Phdr*)kernel_program_headers_buffer;

        for (p = 0; p < n_program_headers; p++) {
            if (program_headers[p].p_type == PT_LOAD) {
                status
                  = load_segment(kernel_img_file, program_headers[p].p_offset,
                                 program_headers[p].p_filesz,
                                 program_headers[p].p_memsz,
                                 program_headers[p].p_paddr);
                if (EFI_ERROR(status)) {
                    return status;
                }

                n_segments_loaded++;
            }
        }
    }

    // If we have found no loadable segments, raise an exception.
    if (n_segments_loaded == 0) {
        debug_print_line(L"Fatal Error: No loadable program segments ");
        debug_print_line(L"found in Kernel image\n");

        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}

/**
 * load_kernel_image
 */
EFI_STATUS load_kernel_image(IN EFI_FILE* const        root_file_system,
                             IN CHAR16* const          kernel_image_filename,
                             OUT EFI_PHYSICAL_ADDRESS* kernel_entry_point) {
    /** Program status. */
    EFI_STATUS status;
    /** The kernel file handle. */
    EFI_FILE*  kernel_img_file;
    /** The kernel ELF header buffer. */
    VOID*      kernel_header          = NULL;
    /** The kernel program headers buffer. */
    VOID*      kernel_program_headers = NULL;
    /** The ELF file identity buffer. */
    UINT8*     elf_identity_buffer    = NULL;
    /** The ELF file class. */
    UINT32     file_class             = ELFCLASSNONE;

#ifdef DEBUG
    debug_print_line(L"Debug: Reading kernel image file\n");
#endif

    status = uefi_call_wrapper(root_file_system->Open, 5, root_file_system,
                               &kernel_img_file, kernel_image_filename,
                               EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (check_for_fatal_error(status, L"Error opening kernel file")) {
        return status;
    }

    // Read ELF Identity.
    // From here we can validate the ELF executable, as well as determine the
    // file class.
    status = read_elf_identity(kernel_img_file, &elf_identity_buffer);
    if (check_for_fatal_error(status, L"Error reading executable identity")) {
        return status;
    }

    file_class = elf_identity_buffer[EI_CLASS];

    // Validate the ELF file.
    status     = validate_elf_identity(elf_identity_buffer);
    if (EFI_ERROR(status)) {
        // Error message printed in validation function.
        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: ELF header is valid\n");
#endif

    // Free identity buffer.
    status = uefi_call_wrapper(gBS->FreePool, 1, elf_identity_buffer);
    if (check_for_fatal_error(status,
                              L"Error freeing kernel identity buffer")) {
        return status;
    }

    // Read the ELF file and program headers.
    status = read_elf_file(kernel_img_file, file_class, &kernel_header,
                           &kernel_program_headers);
    if (check_for_fatal_error(status, L"Error reading ELF file")) {
        return status;
    }

#ifdef DEBUG
    print_elf_file_info(kernel_header, kernel_program_headers);
#endif

    // Set the kernel entry point to the address specified in the ELF header.
    if (file_class == ELFCLASS32) {
        *kernel_entry_point = ((Elf32_Ehdr*)kernel_header)->e_entry;
    }
    else if (file_class == ELFCLASS64) {
        *kernel_entry_point = ((Elf64_Ehdr*)kernel_header)->e_entry;
    }

    status = load_program_segments(kernel_img_file, file_class, kernel_header,
                                   kernel_program_headers);
    if (EFI_ERROR(status)) {
        // In the case that loading the kernel segments failed, the error
        // message will have already been printed.
        return status;
    }

// Cleanup.
#ifdef DEBUG
    debug_print_line(L"Debug: Closing kernel binary\n");
#endif

    status = uefi_call_wrapper(kernel_img_file->Close, 1, kernel_img_file);
    if (check_for_fatal_error(status, L"Error closing kernel image")) {
        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Freeing kernel header buffer\n");
#endif

    status = uefi_call_wrapper(gBS->FreePool, 1, (VOID*)kernel_header);
    if (check_for_fatal_error(status, L"Error freeing kernel header buffer")) {
        return status;
    }

#ifdef DEBUG
    debug_print_line(L"Debug: Freeing kernel program header buffer\n");
#endif

    status = uefi_call_wrapper(gBS->FreePool, 1, (VOID*)kernel_program_headers);
    if (check_for_fatal_error(status,
                              L"Error freeing kernel program headers buffer")) {
        return status;
    }

    return status;
}

#ifdef __cplusplus
}
#endif
