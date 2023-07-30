
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

#include "elf.h"

#include "load_elf.h"

#include "efi.h"
#include "efilib.h"

#define KERNEL_EXECUTABLE_PATH (CHAR16*)L"gnu-efi-test_kernel.elf"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE        _image_handle,
                           EFI_SYSTEM_TABLE* _system_table) {
    EFI_STATUS                       status;

    EFI_FILE*                        root_file_system     = nullptr;
    EFI_PHYSICAL_ADDRESS*            kernel_entry_point   = nullptr;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system_protocol = nullptr;

    debug((CHAR16*)L"Initialising File System service\n");
    status = uefi_call_wrapper(gBS->LocateProtocol, 3,
                               &gEfiSimpleFileSystemProtocolGuid, nullptr,
                               (void**)&file_system_protocol);
    if (EFI_ERROR(status)) {
        debug((CHAR16*)L"Fatal Error: Error locating Simple File "
                       L"System Protocol: %s\n",
              get_efi_error_message(status));

        return status;
    }

    debug((CHAR16*)L"Located Simple File System Protocol\n");
    status = uefi_call_wrapper(file_system_protocol->OpenVolume, 2,
                               file_system_protocol, &root_file_system);
    if (check_for_fatal_error(status, (CHAR16*)L"Error opening root volume")) {
        return status;
    }

    debug((CHAR16*)L"Loading Kernel image\n");
    status = load_kernel_image(root_file_system, KERNEL_EXECUTABLE_PATH,
                               kernel_entry_point);
    if (EFI_ERROR(status)) {
        // In the case that loading the kernel image failed, the error
        // message will have already been printed.
        return status;
    }

    debug((CHAR16*)L"Set Kernel Entry Point to: '0x%llx'\n ",
          *kernel_entry_point);

    return EFI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
