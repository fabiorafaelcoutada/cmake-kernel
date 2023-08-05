
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

static wchar_t error_message_buffer[256];

const wchar_t* get_efi_error_message(EFI_STATUS _status) {
    bzero(error_message_buffer, 256);
    StatusToString(error_message_buffer, _status);
    return error_message_buffer;
}

void debug(const wchar_t* _fmt, ...) {
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

bool check_for_fatal_error(const EFI_STATUS     _status,
                           const wchar_t* const _error_message) {
    if (EFI_ERROR(_status)) {
        EFI_INPUT_KEY input_key;

        debug(L"Fatal Error: %s: [%d] %s\n", _error_message, _status,
              get_efi_error_message(_status));

        debug(L"Press any key to reboot...");
        wait_for_input(&input_key);
        return true;
    }

    return false;
}
