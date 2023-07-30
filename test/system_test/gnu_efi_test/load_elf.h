
#ifndef CMAKE_KERNEL_LOAD_ELF_H
#define CMAKE_KERNEL_LOAD_ELF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "elf.h"

#include "efi.h"
#include "efilib.h"

/**
 * 将 eif 错误码转换为字符串
 * @param _status 错误码
 * @return 对应的字符串
 */
const CHAR16* get_efi_error_message(IN EFI_STATUS const _status);

/**
 * 输出调试信息
 * @param _fmt 要输出的格式字符串
 */
void          debug(const CHAR16* _fmt, ...);

/**
 * 输出 elf 文件信息
 * @param _header_ptr elf 文件头
 * @param _program_headers_ptr elf 程序头
 */
void          print_elf_file_info(const Elf64_Ehdr* const _header_ptr,
                                  const Elf64_Phdr* const _program_headers_ptr);

/**
 * 读取 elf 文件的 ehdr
 * @param _elf elf 文件句柄
 * @param _ehdr 输出，ehdr
 * @return efi 错误码
 */
EFI_STATUS    read_ehdr(const EFI_FILE& _elf, Elf64_Ehdr& _ehdr);

/**
 * 输出 elf ehdr
 * @param _ehdr 要输出的 ehdr
 */
void          print_ehdr(const Elf64_Ehdr& _ehdr);

/**
 * 读取 elf 文件的 phdr
 * @param _elf 要读的 elf 文件句柄
 * @param _phdr_offset phdr 偏移
 * @param _phdr_num phdr 数量
 * @param _phdr 输出，phdr 数据
 * @return efi 错误码
 */
EFI_STATUS    read_phdr(const EFI_FILE& _elf, size_t _phdr_offset,
                        size_t _phdr_num, Elf64_Phdr* _phdr);

/**
 * 输出 phdr
 * @param _phdr 要输出的 phdr
 * @param _phdr_num phdr 数量
 */
void          print_phdr(const Elf64_Phdr* const _phdr, size_t _phdr_num);

/**
 * 读取 elf 文件的 shdr
 * @param _elf 要读的 elf 文件句柄
 * @param _shdr_offset shdr 偏移
 * @param _shdr_num shdr 数量
 * @param _shdr 输出，shdr 数据
 * @return efi 错误码
 */
EFI_STATUS    read_shdr(const EFI_FILE& _elf, size_t _shdr_offset,
                        size_t _shdr_num, Elf64_Shdr* _shdr);

/**
 * 输出 shdr
 * @param _shdr 要输出的 shdr
 * @param _shdr_num shdr 数量
 */
void          print_shdr(const Elf64_Shdr* const _shdr, size_t _shdr_num);

/**
 * 读取并检查 elf 标识
 * @param _kernel_img_file elf文件句柄
 * @return efi 错误码
 */
EFI_STATUS    read_and_check_elf_identity(const EFI_FILE& _kernel_img_file);

/**
 * 是否为有效的 elf 标识
 * @param _elf_identity_buffer elf 标识缓冲区
 * @return efi 错误码
 */
EFI_STATUS    validate_elf_identity(const uint8_t* const _elf_identity_buffer);

/**
 * 等待输入
 * @param _key 输入的按键
 * @return efi 错误码
 */
EFI_STATUS    wait_for_input(OUT EFI_INPUT_KEY* _key);

/**
 * 是否为 fatal error
 * @param _status efi 错误码
 * @param _error_message 错误信息
 * @return 是否为 fatal error
 */
bool          check_for_fatal_error(IN EFI_STATUS const _status,
                                    IN const CHAR16*    _error_message);

/**
 * 将 elf 段加载到内存
 * @param _kernel_img_file elf 文件句柄
 * @param _segment_file_offset 偏移
 * @param _segment_file_size 长度
 * @param _segment_memory_size 段内存大小
 * @param _segment_physical_address 段物理地址
 * @return efi 错误码
 */
EFI_STATUS
load_segment(IN EFI_FILE* const            _kernel_img_file,
             IN EFI_PHYSICAL_ADDRESS const _segment_file_offset,
             IN const uint64_t             _segment_file_size,
             IN const uint64_t             _segment_memory_size,
             IN EFI_PHYSICAL_ADDRESS const _segment_physical_address);

/**
 * 加载程序段
 * @param _kernel_img_file elf 文件句柄
 * @param _kernel_header_buffer elf 头缓冲区
 * @param _kernel_program_headers_buffer elf 程序头缓冲区
 * @return efi 错误码
 */
EFI_STATUS load_program_segments(IN EFI_FILE* const _kernel_img_file,
                                 IN void* const     _kernel_header_buffer,
                                 IN void* const _kernel_program_headers_buffer);

/**
 * 加载 elf 内核
 * @param _root_file_system efi 根文件系统句柄
 * @param _kernel_image_filename elf 内核文件路径
 * @param _kernel_entry_point 内核入口点
 * @return efi 错误码
 */
EFI_STATUS load_kernel_image(IN EFI_FILE* const        _root_file_system,
                             IN CHAR16* const          _kernel_image_filename,
                             OUT EFI_PHYSICAL_ADDRESS* _kernel_entry_point);

#ifdef __cplusplus
}
#endif

#endif /* CMAKE_KERNEL_LOAD_ELF_H */