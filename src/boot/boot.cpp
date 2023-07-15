
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

#include "boot.h"

#include "gnu-efi/efi.h"
#include "gnu-efi/efilib.h"

extern "C" EFI_STATUS
efi_main(void* systemTable) {
    // uefi_call_wrapper(InitializeLib, 2, image, systemTable);
    // EFI_STATUS status = uefi_call_wrapper(systemTable->ConOut->ClearScreen,
    // 1,
    //                                       systemTable->ConOut);

    // status            = uefi_call_wrapper(systemTable->ConOut->OutputString,
    // 2,
    //                                       systemTable->ConOut, L"Hello
    //                                       UEFI!\n");

    return EFI_SUCCESS;
}

int32_t main(int32_t _argc, int8_t **_argv) {
    (void)_argc;
    (void)_argv;

    // 进入死循环
    while (1) {
        ;
    }

    return 0;
}
