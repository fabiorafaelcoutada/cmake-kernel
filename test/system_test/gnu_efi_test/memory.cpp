
/**
 * @file memory.cpp
 * @brief 内存相关
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

void Memory::flush_desc(void) {
    EFI_STATUS status = 0;
    // 第一次读取，获取 MemoryMapSize
    status = uefi_call_wrapper(gBS->GetMemoryMap, 5, &mem_map_size, memory_map,
                               &map_key, &desc_size, &desc_version);
    if (EFI_ERROR(status)) {
        if (status != EFI_BUFFER_TOO_SMALL) {
            debug(L"GetMemoryMap failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    }

    // 分配空间
    if (memory_map != nullptr) {
        EFI_STATUS status = uefi_call_wrapper(gBS->FreePool, 1, memory_map);
        if (EFI_ERROR(status)) {
            debug(L"FreePool failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
        memory_map = nullptr;
    }
    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiRuntimeServicesData,
                               mem_map_size, (void**)&memory_map);
    if (EFI_ERROR(status)) {
        debug(L"AllocatePool failed: %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 第二次读取，获取 MemoryMap
    status = uefi_call_wrapper(gBS->GetMemoryMap, 5, &mem_map_size, memory_map,
                               &map_key, &desc_size, &desc_version);
    if (EFI_ERROR(status)) {
        debug(L"GetMemoryMap failed: %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }
    return;
}

Memory::Memory(void) {
    flush_desc();
    return;
}

Memory::~Memory(void) {
    if (memory_map == nullptr) {
        return;
    }
    try {
        EFI_STATUS status = uefi_call_wrapper(gBS->FreePool, 1, memory_map);
        if (EFI_ERROR(status)) {
            debug(L"FreePool failed: %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    } catch (const std::exception& e) {
        debug(L"FreePool failed: %s\n", e.what());
    }
    return;
}

void Memory::print_info(void) {
    flush_desc();
    debug(L"Type\t\t\t\tPages\tPhysicalStart\tVirtualStart\tAttribute\n");
    for (uint64_t i = 0; i < mem_map_size / desc_size; i++) {
        EFI_MEMORY_DESCRIPTOR* MMap
          = (EFI_MEMORY_DESCRIPTOR*)(((CHAR8*)memory_map) + i * desc_size);

        switch (MMap->Type) {
            case EfiReservedMemoryType: {
                debug(L"iReservedMemoryType\t\t");
                break;
            }
            case EfiLoaderCode: {
                debug(L"EfiLoaderCode\t\t\t");
                break;
            }
            case EfiLoaderData: {
                debug(L"EfiLoaderData\t\t");
                break;
            }
            case EfiBootServicesCode: {
                debug(L"EfiBootServicesCode\t\t");
                break;
            }
            case EfiBootServicesData: {
                debug(L"EfiBootServicesData\t\t");
                break;
            }
            case EfiRuntimeServicesCode: {
                debug(L"EfiRuntimeServicesCode\t\t");
                break;
            }
            case EfiRuntimeServicesData: {
                debug(L"EfiRuntimeServicesData\t\t");
                break;
            }
            case EfiConventionalMemory: {
                debug(L"EfiConventionalMemory\t\t");
                break;
            }
            case EfiUnusableMemory: {
                debug(L"EfiUnusableMemory\t\t");
                break;
            }
            case EfiACPIReclaimMemory: {
                debug(L"EfiACPIReclaimMemory\t\t");
                break;
            }
            case EfiACPIMemoryNVS: {
                debug(L"EfiACPIMemoryNVS\t\t");
                break;
            }
            case EfiMemoryMappedIO: {
                debug(L"EfiMemoryMappedIO\t\t");
                break;
            }
            case EfiMemoryMappedIOPortSpace: {
                debug(L"EfiMemoryMappedIOPortSpace\t\t");
                break;
            }
            case EfiPalCode: {
                debug(L"EfiPalCode\t\t");
                break;
            }
            case EfiMaxMemoryType: {
                debug(L"EfiMaxMemoryType\t\t");
                break;
            }
            default: {
                debug(L"Unknown 0x%x\t\t", MMap->Type);
                break;
            }
        }

        Print(L"%d\t%X\t%X\t%X\n", MMap->NumberOfPages, MMap->PhysicalStart,
              MMap->VirtualStart, MMap->Attribute);
    }
    return;
}
