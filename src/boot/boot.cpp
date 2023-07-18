
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

#if ENABLE_GNU_EFI == 1
#    include "efi.h"
#    include "efilib.h"
#else
#    include "posix-uefi/uefi.h"
#endif

#include <elf.h>

EFI_STATUS
efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab) {
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	EFI_STATUS efi_status;
	EFI_LOADED_IMAGE *li;
	UINTN pat = PoolAllocationType;
	VOID *void_li_p;

	InitializeLib(image_handle, systab);
	PoolAllocationType = 2; /* klooj */

	Print(L"Hello World! (0xd=0x%x, 13=%d)\n", 13, 13);

	Print(L"before InitializeLib(): PoolAllocationType=%d\n",
		pat);

	Print(L" after InitializeLib(): PoolAllocationType=%d\n",
		PoolAllocationType);

	/*
	 * Locate loaded_image_handle instance.
	 */

	Print(L"BS->HandleProtocol()  ");

	efi_status = uefi_call_wrapper(
		BS->HandleProtocol,
		3,
		image_handle,
		&loaded_image_protocol,
		&void_li_p);
	li = void_li_p;

	Print(L"%xh (%r)\n", efi_status, efi_status);

	if (efi_status != EFI_SUCCESS) {
		return efi_status;
	}

	Print(L"  li: %xh\n", li);

	if (!li) {
		return EFI_UNSUPPORTED;
	}

	Print(L"  li->Revision:        %xh\n", li->Revision);
	Print(L"  li->ParentHandle:    %xh\n", li->ParentHandle);
	Print(L"  li->SystemTable:     %xh\n", li->SystemTable);
	Print(L"  li->DeviceHandle:    %xh\n", li->DeviceHandle);
	Print(L"  li->FilePath:        %xh\n", li->FilePath);
	Print(L"  li->Reserved:        %xh\n", li->Reserved);
	Print(L"  li->LoadOptionsSize: %xh\n", li->LoadOptionsSize);
	Print(L"  li->LoadOptions:     %xh\n", li->LoadOptions);
	Print(L"  li->ImageBase:       %xh\n", li->ImageBase);
	Print(L"  li->ImageSize:       %xh\n", li->ImageSize);
	Print(L"  li->ImageCodeType:   %xh\n", li->ImageCodeType);
	Print(L"  li->ImageDataType:   %xh\n", li->ImageDataType);
	Print(L"  li->Unload:          %xh\n", li->Unload);

#if 0
typedef struct {
    UINT32                          Revision;
    EFI_HANDLE                      ParentHandle;
    struct _EFI_SYSTEM_TABLE        *SystemTable;

    // Source location of image
    EFI_HANDLE                      DeviceHandle;
    EFI_DEVICE_PATH                 *FilePath;
    VOID                            *Reserved;

    // Images load options
    UINT32                          LoadOptionsSize;
    VOID                            *LoadOptions;

    // Location of where image was loaded
    VOID                            *ImageBase;
    UINT64                          ImageSize;
    EFI_MEMORY_TYPE                 ImageCodeType;
    EFI_MEMORY_TYPE                 ImageDataType;

    // If the driver image supports a dynamic unload request
    EFI_IMAGE_UNLOAD                Unload;

} EFI_LOADED_IMAGE;
#endif

	return EFI_SUCCESS;
}



#ifdef __cplusplus
}
#endif
