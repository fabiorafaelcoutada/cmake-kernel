
#ifndef CMAKE_KERNEL_LOAD_ELF_H
#define CMAKE_KERNEL_LOAD_ELF_H

#include <elf.h>
#include <utility>

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
const wchar_t* get_efi_error_message(IN EFI_STATUS const _status);

/**
 * 输出调试信息
 * @param _fmt 要输出的格式字符串
 */
void           debug(const wchar_t* _fmt, ...);

/**
 * 等待输入
 * @param _key 输入的按键
 * @return efi 错误码
 */
EFI_STATUS     wait_for_input(OUT EFI_INPUT_KEY* _key);

/**
 * 是否为 fatal error
 * @param _status efi 错误码
 * @param _error_message 错误信息
 * @return 是否为 fatal error
 */
bool           check_for_fatal_error(IN EFI_STATUS const _status,
                                     IN const wchar_t*   _error_message);

/**
 * 将 elf 段加载到内存
 * @param _phdr 要加载的程序段 phdr
 * @param _elf_file_buffer elf 文件数据
 * @return efi 错误码
 */
EFI_STATUS
load_sections(const Elf64_Phdr& _phdr, const uint8_t* const _elf_file_buffer);

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
EFI_STATUS load_kernel_image(const EFI_FILE&      _root_file_system,
                             const wchar_t* const _kernel_image_filename,
                             uint64_t&            _kernel_entry_point);

/**
 * 图形相关
 */
class Graphics {
private:
    /// 图形输出协议
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

public:
    /**
     * 构造函数
     */
    Graphics(void);

    /**
     * 析构函数
     */
    ~Graphics(void) = default;

    /**
     * 设置图形模式
     * @param _format 图形像素格式，默认为 PixelBlueGreenRedReserved8BitPerColor
     * @param _w 宽度，默认为 1920
     * @param _h 高度，默认为 1080
     */
    void set_mode(EFI_GRAPHICS_PIXEL_FORMAT _format
                  = PixelBlueGreenRedReserved8BitPerColor,
                  uint32_t _w = 1920, uint32_t _h = 1080) const;

    /**
     * 输出图形信息
     */
    void print_info(void) const;
};

class Memory {
private:
    /// 内存映射信息
    uint64_t               mem_map_size = 0;
    EFI_MEMORY_DESCRIPTOR* memory_map   = nullptr;
    uint64_t               map_key      = 0;
    uint64_t               desc_size    = 0;
    uint32_t               desc_version = 0;

    /**
     * 更新内存映射信息
     */
    void                   flush_desc(void);

public:
    /**
     * 构造函数
     */
    Memory(void);

    /**
     * 析构函数
     */
    ~Memory(void);

    /**
     * 输出内存映射信息
     */
    void     print_info(void);

    /**
     * 获取 map_key
     * @return map_key
     */
    uint64_t get_map_key(void);
};

/**
 * elf 文件相关
 */
class Elf {
private:
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system_protocol = nullptr;
    EFI_FILE*                        root_file_system     = nullptr;
    /// elf 文件句柄
    EFI_FILE*                        elf                  = nullptr;
    size_t                           elf_file_size        = 0;
    uint8_t*                         elf_file_buffer      = nullptr;
    Elf64_Ehdr                       ehdr                 = {};
    Elf64_Phdr*                      phdr                 = nullptr;
    Elf64_Shdr*                      shdr                 = nullptr;
    /// section 缓冲区大小
    static constexpr const size_t    SECTION_BUF_SIZE     = 1024;
    /// shstrtab 缓冲
    uint8_t                          shstrtab_buf[SECTION_BUF_SIZE];

    /**
     * 获取文件大小
     * @return 文件大小
     */
    size_t                           get_file_size(void) const;

    /**
     * 检查 elf 标识
     * @return 失败返回 false
     */
    bool                             check_elf_identity(void) const;

    /**
     * 读取 elf 文件的 ehdr
     */
    void                             get_ehdr(void);

    /**
     * 输出 elf ehdr
     */
    void                             print_ehdr(void) const;

    /**
     * 读取 elf 文件的 phdr
     */
    void                             get_phdr(void);

    /**
     * 输出 phdr
     */
    void                             print_phdr(void) const;

    /**
     * 读取 elf 文件的 shdr
     */
    void                             get_shdr(void);

    /**
     * 输出 shdr
     */
    void                             print_shdr(void) const;

public:
    Elf(const wchar_t* const _kernel_image_filename);
    ~Elf(void);

    /**
     * 加载 elf 内核
     * @param _kernel_image_filename elf 内核文件路径
     * @return 内核入口点
     */
    uint64_t load_kernel_image(const wchar_t* const _kernel_image_filename);
};

#endif /* CMAKE_KERNEL_LOAD_ELF_H */
