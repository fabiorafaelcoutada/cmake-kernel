
#ifndef CMAKE_KERNEL_LOAD_ELF_H
#define CMAKE_KERNEL_LOAD_ELF_H

#include "elf.h"
#include "utility"

#ifdef __cplusplus
extern "C" {
#endif

#include "efi.h"
#include "efilib.h"

#ifdef __cplusplus
}
#endif

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
 * 获取文件大小
 * @param _file 文件句柄
 * @return <efi 错误码, 文件大小>
 */
std::pair<EFI_STATUS, size_t> get_file_size(const EFI_FILE& _file);

/**
 * 读取 elf 文件的 ehdr
 * @param _elf_file_buffer elf 文件缓冲
 * @param _ehdr 输出，ehdr
 * @return efi 错误码
 */
const Elf64_Ehdr&             get_ehdr(const uint8_t* const _elf_file_buffer);

/**
 * 输出 elf ehdr
 * @param _ehdr 要输出的 ehdr
 */
void                          print_ehdr(const Elf64_Ehdr& _ehdr);

/**
 * 读取 elf 文件的 phdr
 * @param _offset phdr 在 elf 文件中的偏移 e_phoff
 * @param _elf_file_buffer elf 文件缓冲
 * @return Elf64_Phdr 指针，长度为 e_phnum
 */
const Elf64_Phdr* const
     get_phdr(uint64_t _offset, const uint8_t* const _elf_file_buffer);

/**
 * 输出 phdr
 * @param _phdr 要输出的 phdr
 * @param _phdr_num phdr 数量
 */
void print_phdr(const Elf64_Phdr* const _phdr, size_t _phdr_num);

/// section 缓冲区大小
static constexpr const size_t SECTION_BUF_SIZE = 1024;
/// shstrtab 缓冲
extern uint8_t                shstrtab_buf[SECTION_BUF_SIZE];

/**
 * 读取 shdr 指定的 section
 * @param _elf_file_buffer elf 文件缓冲
 * @param _shdr 要读的 shdr
 * @param _buffer 输出
 */
void read_section(const uint8_t* const _elf_file_buffer,
                  const Elf64_Shdr& _shdr, uint8_t* const _buffer);

/**
 * 读取 elf 文件的 shdr
 * @param _offset shdr 在 elf 文件中的偏移 e_shoff
 * @param _elf_file_buffer elf 文件缓冲
 * @return Elf64_Shdr 指针，长度为 e_shnum
 */
const Elf64_Shdr* const
           get_shdr(uint64_t _offset, const uint8_t* const _elf_file_buffer);

/**
 * 输出 shdr
 * @param _shdr 要输出的 shdr
 * @param _shdr_num shdr 数量
 */
void       print_shdr(const Elf64_Shdr* const _shdr, size_t _shdr_num);

/**
 * 检查 elf 标识
 * @param _elf_file_buffer elf 文件数据
 * @return 失败返回 false
 */
EFI_STATUS check_elf_identity(const uint8_t* const _elf_file_buffer);

/**
 * 等待输入
 * @param _key 输入的按键
 * @return efi 错误码
 */
EFI_STATUS wait_for_input(OUT EFI_INPUT_KEY* _key);

/**
 * 是否为 fatal error
 * @param _status efi 错误码
 * @param _error_message 错误信息
 * @return 是否为 fatal error
 */
bool       check_for_fatal_error(IN EFI_STATUS const _status,
                                 IN const CHAR16*    _error_message);

/**
 * 将 elf 段加载到内存
 * @param _elf elf 文件句柄
 * @param _phdr 要加载的程序段 phdr
 * @return efi 错误码
 */
EFI_STATUS
load_sections(const EFI_FILE& _elf, const Elf64_Phdr& _phdr);

/**
 * 加载程序段
 * @param _elf elf 文件句柄
 * @param _ehdr elf ehdr
 * @param _phdr elf phdr 指针
 * @return efi 错误码
 */
EFI_STATUS
load_program_sections(const EFI_FILE& _elf, const Elf64_Ehdr& _ehdr,
                      const Elf64_Phdr* const _phdr);

/**
 * 加载 elf 内核
 * @param _root_file_system efi 根文件系统句柄
 * @param _kernel_image_filename elf 内核文件路径
 * @param _kernel_entry_point 内核入口点
 * @return efi 错误码
 */
EFI_STATUS load_kernel_image(const EFI_FILE&     _root_file_system,
                             const CHAR16* const _kernel_image_filename,
                             uint64_t&           _kernel_entry_point);

#endif /* CMAKE_KERNEL_LOAD_ELF_H */
