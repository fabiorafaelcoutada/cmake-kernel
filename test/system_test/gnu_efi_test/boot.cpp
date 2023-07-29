
/**
 * @file boot.cpp
 * @brief boot cpp
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

#include "efi.h"
#include "efilib.h"

#include <elf.h>

extern bool       check_for_fatal_error(IN EFI_STATUS const status,
                                        IN const CHAR16*    error_message);
extern EFI_STATUS debug_print_line(IN CHAR16* fmt, ...);
extern EFI_STATUS
                     load_kernel_image(IN EFI_FILE* const        root_file_system,
                                       IN CHAR16* const          kernel_image_filename,
                                       OUT EFI_PHYSICAL_ADDRESS* kernel_entry_point);
extern const CHAR16* get_efi_error_message(IN EFI_STATUS const status);
#define KERNEL_EXECUTABLE_PATH L"gnu-efi-test_kernel.elf"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE        _image_handle,
                           EFI_SYSTEM_TABLE* _system_table) {
    EFI_STATUS                       status;

    EFI_FILE*                        root_file_system     = nullptr;
    EFI_PHYSICAL_ADDRESS*            kernel_entry_point   = nullptr;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system_protocol = nullptr;

    InitializeLib(_image_handle, _system_table);

    Print(L"111111aaa\n");
    debug_print_line((CHAR16*)L"Debug: Initialising File System service\n");
    Print(L"1111222211aaa\n");

    status = uefi_call_wrapper((void*)gBS->LocateProtocol, 3,
                               &gEfiSimpleFileSystemProtocolGuid, nullptr,
                               (void**)&file_system_protocol);
    if (EFI_ERROR(status)) {
        debug_print_line((CHAR16*)L"Fatal Error: Error locating Simple File "
                                  L"System Protocol: %s\n",
                         get_efi_error_message(status));

        return status;
    }

    debug_print_line((CHAR16*)L"Debug: Located Simple File System Protocol\n");

    status = uefi_call_wrapper((void*)file_system_protocol->OpenVolume, 2,
                               file_system_protocol, &root_file_system);
    if (check_for_fatal_error(status, (CHAR16*)L"Error opening root volume")) {
        return status;
    }

    debug_print_line(L"Debug: Loading Kernel image\n");
    status = load_kernel_image(root_file_system, KERNEL_EXECUTABLE_PATH,
                               kernel_entry_point);
    if (EFI_ERROR(status)) {
        // In the case that loading the kernel image failed, the error
        // message will have already been printed.
        return status;
    }

    debug_print_line((CHAR16*)L"Debug: Set Kernel Entry Point to: '0x%llx'\n ",
                     *kernel_entry_point);

    return EFI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
