
/**
 * @file graphics.cpp
 * @brief 图形相关
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

#include <stdexcept>

#include "load_elf.h"

Graphics::Graphics(void) {
    EFI_STATUS status
      = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);
    if (EFI_ERROR(status)) {
        debug(L"Could not locate GOP: %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }
    if (gop == nullptr) {
        debug(L"LibLocateProtocol(GraphicsOutputProtocol, &gop) returned %d "
              L"but gop is nullptr\n",
              status);
        throw std::runtime_error("gop == nullptr");
    }
    return;
}

void Graphics::set_mode(EFI_GRAPHICS_PIXEL_FORMAT _format, uint32_t _w,
                        uint32_t _h) const {
    EFI_STATUS status;

    for (uint32_t i = 0; i < gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info      = nullptr;
        uint64_t                              mode_info_size = 0;
        status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &mode_info_size,
                                   &mode_info);
        if (EFI_ERROR(status)) {
            debug(L"QueryMode failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }

        if ((mode_info->PixelFormat == _format)
            && (mode_info->HorizontalResolution == _w)
            && (mode_info->VerticalResolution == _h)) {
            status = uefi_call_wrapper(gop->SetMode, 2, gop, i);
            if (EFI_ERROR(status)) {
                debug(L"SetMode failed: %d\n", status);
                throw std::runtime_error("EFI_ERROR(status)");
            }
        }
        status = uefi_call_wrapper(gBS->FreePool, 1, mode_info);
        if (EFI_ERROR(status)) {
            debug(L"FreePool failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    }

    debug(
      L"Current Mode: %d, Version: 0x%x, Format: %d, Horizontal: %d, Vertical: "
      L"%d, ScanLine: %d, FrameBufferBase: 0x%X, FrameBufferSize: 0x%X\n",
      gop->Mode->Mode, gop->Mode->Info->Version, gop->Mode->Info->PixelFormat,
      gop->Mode->Info->HorizontalResolution,
      gop->Mode->Info->VerticalResolution, gop->Mode->Info->PixelsPerScanLine,
      gop->Mode->FrameBufferBase, gop->Mode->FrameBufferSize);

    return;
}

void Graphics::print_info(void) const {
    debug(
      L"Current Mode: %d, Version: 0x%x, Format: %d, Horizontal: %d, Vertical: "
      L"%d, ScanLine: %d, FrameBufferBase: 0x%X, FrameBufferSize: 0x%X\n",
      gop->Mode->Mode, gop->Mode->Info->Version, gop->Mode->Info->PixelFormat,
      gop->Mode->Info->HorizontalResolution,
      gop->Mode->Info->VerticalResolution, gop->Mode->Info->PixelsPerScanLine,
      gop->Mode->FrameBufferBase, gop->Mode->FrameBufferSize);

    for (uint32_t i = 0; i < gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info      = nullptr;
        uint64_t                              mode_info_size = 0;
        auto status = uefi_call_wrapper(gop->QueryMode, 4, gop, i,
                                        &mode_info_size, &mode_info);
        if (EFI_ERROR(status)) {
            debug(L"QueryMode failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
        debug(
          L"Mode: %d, Version: 0x%x, Format: %d, Horizontal: %d, Vertical: %d, "
          L"ScanLine: %d\n",
          i, mode_info->Version, mode_info->PixelFormat,
          mode_info->HorizontalResolution, mode_info->VerticalResolution,
          mode_info->PixelsPerScanLine);
        status = uefi_call_wrapper(gBS->FreePool, 1, mode_info);
        if (EFI_ERROR(status)) {
            debug(L"FreePool failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    }
    return;
}
