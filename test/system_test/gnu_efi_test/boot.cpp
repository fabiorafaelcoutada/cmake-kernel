
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

#include <exception>
#include <stdexcept>

#include "load_elf.h"

#define KERNEL_EXECUTABLE_PATH (wchar_t*)L"gnu-efi-test_kernel.elf"

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE        _image_handle,
                                      EFI_SYSTEM_TABLE* _system_table) {
    try {
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

        // auto status = uefi_call_wrapper(gBS->ExitBootServices, 2,
        // _image_handle,
        //                                 memory.get_map_key());
        // if (EFI_ERROR(status)) {
        // debug(L"ExitBootServices failed, Memory Map has Changed %d\n",
        //    status);
        // throw std::runtime_error("EFI_ERROR(status)");
        // }
    } catch (const std::exception& e) {
        debug(L"Fatal Error: %s\n", e.what());
        return EFI_LOAD_ERROR;
    }

    // debug(L"Set Kernel Entry Point to: [0x%llX]\n ", kernel_entry_point);
    // auto kernel_entry = (void (*)(void))0x100000;
    // kernel_entry();

    return EFI_SUCCESS;
}
