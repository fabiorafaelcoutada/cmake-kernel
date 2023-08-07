
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

#include <cstring>
#include <exception>
#include <stdexcept>

#include "load_elf.h"

#define KERNEL_EXECUTABLE_PATH (wchar_t*)L"gnu-efi-test_kernel.elf"

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE        _image_handle,
                                      EFI_SYSTEM_TABLE* _system_table) {
    try {
        // 输出 efi 信息
        EFI_LOADED_IMAGE* loaded_image = nullptr;
        auto              status
          = LibLocateProtocol(&LoadedImageProtocol, (void**)&loaded_image);
        if (EFI_ERROR(status)) {
            debug(L"handleprotocol: %d\n", status);
        }

        debug(L"Revision:        0x%X\n", loaded_image->Revision);
        debug(L"ParentHandle:    0x%X\n", loaded_image->ParentHandle);
        debug(L"SystemTable:     0x%X\n", loaded_image->SystemTable);
        debug(L"DeviceHandle:    0x%X\n", loaded_image->DeviceHandle);
        debug(L"FilePath:        0x%X\n", loaded_image->FilePath);
        debug(L"Reserved:        0x%X\n", loaded_image->Reserved);
        debug(L"LoadOptionsSize: 0x%X\n", loaded_image->LoadOptionsSize);
        debug(L"LoadOptions:     0x%X\n", loaded_image->LoadOptions);
        debug(L"ImageBase:       0x%X\n", loaded_image->ImageBase);
        debug(L"ImageSize:       0x%X\n", loaded_image->ImageSize);
        debug(L"ImageCodeType:   0x%X\n", loaded_image->ImageCodeType);
        debug(L"ImageDataType:   0x%X\n", loaded_image->ImageDataType);
        debug(L"Unload:          0x%X\n", loaded_image->Unload);

        // 初始化 Graphics
        auto graphics = Graphics();
        // 打印图形信息
        graphics.print_info();
        // 设置为 1920*1080
        graphics.set_mode(PixelBlueGreenRedReserved8BitPerColor, 1920, 1080);
        // 初始化 Memory
        auto memory = Memory();
        memory.print_info();
        // 加载内核
        auto elf = Elf(KERNEL_EXECUTABLE_PATH);
        elf.load_kernel_image(KERNEL_EXECUTABLE_PATH);

        // 退出 boot service
        status = uefi_call_wrapper(gBS->ExitBootServices, 2, _image_handle,
                                   memory.get_map_key());
        if (EFI_ERROR(status)) {
            debug(L"ExitBootServices failed, Memory Map has Changed %d\n",
                  status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    } catch (const std::exception& e) {
        debug(L"Fatal Error: %s\n", e.what());
        return EFI_LOAD_ERROR;
    }

    // debug(L"Set Kernel Entry Point to: [0x%llX]\n ", kernel_entry_point);
    // auto kernel_entry = (void (*)(void))0x100000;
    // kernel_entry();

    return EFI_SUCCESS;
}
