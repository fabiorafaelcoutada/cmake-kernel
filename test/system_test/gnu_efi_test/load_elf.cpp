
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

#include <cstring>
#include <cwchar>
#include <memory>

#include "load_elf.h"

/**
 * 将 char* 转换为 wchar_t*
 * @param _d 输出
 * @param _s 输入
 * @return 字符数量
 */
static size_t char2wchar(wchar_t* _d, const char* _s) {
    size_t i = 0;
    while (_s[i] != '\0') {
        _d[i] = _s[i];
        i++;
    }
    return i;
}

size_t Elf::get_file_size(void) const {
    // 获取 elf 文件大小
    auto elf_file_info = LibFileInfo(elf);
    auto file_size     = elf_file_info->FileSize;
    return file_size;
}

bool Elf::check_elf_identity(void) const {
    if ((elf_file_buffer[EI_MAG0] != ELFMAG0)
        || (elf_file_buffer[EI_MAG1] != ELFMAG1)
        || (elf_file_buffer[EI_MAG2] != ELFMAG2)
        || (elf_file_buffer[EI_MAG3] != ELFMAG3)) {
        debug(L"Fatal Error: Invalid ELF header\n");
        return false;
    }
    if (elf_file_buffer[EI_CLASS] == ELFCLASS32) {
        debug(L"Found 32bit executable but NOT SUPPORT\n");
        return false;
    }
    else if (elf_file_buffer[EI_CLASS] == ELFCLASS64) {
        debug(L"Found 64bit executable\n");
    }
    else {
        debug(L"Fatal Error: Invalid executable\n");
        return false;
    }
    return true;
}

void Elf::get_ehdr(void) {
    ehdr = *reinterpret_cast<const Elf64_Ehdr*>(elf_file_buffer);
    return;
}

void Elf::print_ehdr(void) const {
    debug(L"  Magic:    ");
    for (uint64_t i = 0; i < EI_NIDENT; i++) {
        debug(L"%02x ", ehdr.e_ident[i]);
    }
    debug(L"\n");

    debug(L"  Class:                                ");
    switch (ehdr.e_ident[EI_CLASS]) {
        case ELFCLASSNONE: {
            debug(L"Invalid class");
            break;
        }
        case ELFCLASS32: {
            debug(L"ELF32");
            break;
        }
        case ELFCLASS64: {
            debug(L"ELF64");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_ident[EI_CLASS]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Data:                                 ");
    switch (ehdr.e_ident[EI_DATA]) {
        case ELFDATANONE: {
            debug(L"Invalid data encoding");
            break;
        }
        case ELFDATA2LSB: {
            debug(L"2's complement, little endian");
            break;
        }
        case ELFDATA2MSB: {
            debug(L"2's complement, big endian");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_ident[EI_DATA]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Version:                              %d ",
          ehdr.e_ident[EI_VERSION]);
    switch (ehdr.e_ident[EI_VERSION]) {
        case EV_NONE: {
            debug(L"Invalid ELF version");
            break;
        }
        case EV_CURRENT: {
            debug(L"Current version");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_ident[EI_VERSION]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  OS/ABI:                               ");
    switch (ehdr.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV: {
            debug(L"UNIX System V ABI");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_ident[EI_OSABI]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  ABI Version:                          %d\n",
          ehdr.e_ident[EI_ABIVERSION]);

    debug(L"  Type:                                 ");
    switch (ehdr.e_type) {
        case ET_NONE: {
            debug(L"No file type");
            break;
        }
        case ET_REL: {
            debug(L"Relocatable file");
            break;
        }
        case ET_EXEC: {
            debug(L"Executable file");
            break;
        }
        case ET_DYN: {
            debug(L"DYN (Shared object file)");
            break;
        }
        case ET_CORE: {
            debug(L"Core file");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_type);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Machine:                              ");
    switch (ehdr.e_machine) {
        case EM_X86_64: {
            debug(L"AMD x86-64 architecture");
            break;
        }
        case EM_RISCV: {
            debug(L"RISC-V");
            break;
        }
        case EM_AARCH64: {
            debug(L"ARM AARCH64");
            break;
        }
        default: {
            debug(L"%d", ehdr.e_machine);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Version:                              0x%x\n", ehdr.e_version);
    debug(L"  Entry point address:                  0x%x\n", ehdr.e_entry);
    debug(L"  Start of program headers:             %d (bytes into "
          L"file)\n",
          ehdr.e_phoff);
    debug(L"  Start of section headers:             %d (bytes into "
          L"file)\n",
          ehdr.e_shoff);
    debug(L"  Flags:                                0x%x\n", ehdr.e_flags);
    debug(L"  Size of this header:                  %d (bytes)\n",
          ehdr.e_ehsize);
    debug(L"  Size of program headers:              %d (bytes)\n",
          ehdr.e_phentsize);
    debug(L"  Number of program headers:            %d\n", ehdr.e_phnum);
    debug(L"  Size of section headers:              %d (bytes)\n",
          ehdr.e_shentsize);
    debug(L"  Number of section headers:            %d\n", ehdr.e_shnum);
    debug(L"  Section header string table index:    %d\n", ehdr.e_shstrndx);
    return;
}

void Elf::get_phdr(void) {
    phdr = reinterpret_cast<Elf64_Phdr*>(elf_file_buffer + ehdr.e_phoff);
    return;
}

void Elf::print_phdr(void) const {
    debug(L"\nProgram Headers:\n");
    debug(
      (wchar_t*)L"  "
                L"Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\t"
                L"Flags\tAlign\n");
    for (uint64_t i = 0; i < ehdr.e_phnum; i++) {
        switch (phdr[i].p_type) {
            case PT_NULL: {
                debug(L"  NULL\t\t");
                break;
            }

            case PT_LOAD: {
                debug(L"  LOAD\t\t");
                break;
            }
            case PT_DYNAMIC: {
                debug(L"  DYNAMIC\t");
                break;
            }
            case PT_INTERP: {
                debug(L"  INTERP\t");
                break;
            }
            case PT_NOTE: {
                debug(L"  NOTE\t\t");
                break;
            }
            case PT_SHLIB: {
                debug(L"  SHLIB\t\t");
                break;
            }
            case PT_PHDR: {
                debug(L"  PHDR\t\t");
                break;
            }
            case PT_TLS: {
                debug(L"  TLS\t\t");
                break;
            }
            case PT_NUM: {
                debug(L"  NUM\t\t");
                break;
            }
            case PT_LOOS: {
                debug(L"  LOOS\t\t");
                break;
            }
            case PT_GNU_EH_FRAME: {
                debug(L"  GNU_EH_FRAME\t");
                break;
            }
            case PT_GNU_STACK: {
                debug(L"  GNU_STACK\t");
                break;
            }
            case PT_GNU_RELRO: {
                debug(L"  GNU_RELRO\t");
                break;
            }
            case PT_GNU_PROPERTY: {
                debug(L"  GNU_PROPERTY\t");
                break;
            }
            case PT_SUNWBSS: {
                debug(L"  SUNWBSS\t\t");
                break;
            }
            case PT_SUNWSTACK: {
                debug(L"  SUNWSTACK\t");
                break;
            }
            case PT_HIOS: {
                debug(L"  HIOS\t\t");
                break;
            }
            case PT_LOPROC: {
                debug(L"  LOPROC\t\t");
                break;
            }
            case PT_HIPROC: {
                debug(L"  HIPROC\t\t");
                break;
            }
            default: {
                debug(L"  Unknown 0x%X\t", phdr[i].p_type);
                break;
            }
        }

        debug(L"0x%X\t", phdr[i].p_offset);
        debug(L"0x%X\t", phdr[i].p_vaddr);
        debug(L"0x%X\t", phdr[i].p_paddr);
        debug(L"0x%X\t", phdr[i].p_filesz);
        debug(L"0x%X\t", phdr[i].p_memsz);

        switch (phdr[i].p_flags) {
            case PF_X: {
                debug(L"E\t");
                break;
            }
            case PF_W: {
                debug(L"W\t");
                break;
            }
            case PF_R: {
                debug(L"R\t");
                break;
            }
            case PF_MASKOS: {
                debug(L"OS-specific\t");
                break;
            }
            case PF_MASKPROC: {
                debug(L"Processor-specific\t");
                break;
            }
            case (PF_X | PF_R): {
                debug(L"R E\t");
                break;
            }
            case (PF_W | PF_R): {
                debug(L"RW\t");
                break;
            }
            default: {
                debug(L"Unknown 0x%x\t", phdr[i].p_flags);
                break;
            }
        }
        debug(L"0x%x\n", phdr[i].p_align);
    }
    return;
}

void Elf::get_shdr(void) {
    shdr = reinterpret_cast<Elf64_Shdr*>(elf_file_buffer + ehdr.e_shoff);
    // 将 shstrtab 的内容复制到 shstrtab_buf 中
    memcpy(shstrtab_buf, elf_file_buffer + shdr[ehdr.e_shstrndx].sh_offset,
           shdr[ehdr.e_shstrndx].sh_size);
    return;
}

void Elf::print_shdr(void) const {
    debug(L"\nSection Headers:\n");
    debug(L"  [Nr] "
          L"Name\t\t\tType\t\tAddress\t\tOffset\t\tSize\t\tEntSize\t\tFl"
          L"ags\tLink\tInfo\tAlign\n");
    for (uint64_t i = 0; i < ehdr.e_shnum; i++) {
        debug(L"  [%2d] ", i);

        wchar_t buf[SECTION_BUF_SIZE];
        bzero(buf, SECTION_BUF_SIZE);
        auto char2wchar_ret
          = char2wchar(buf, (const char*)(shstrtab_buf + shdr[i].sh_name));
        debug(L"%s\t", buf);

        if (char2wchar_ret <= 16) {
            debug(L"%s", L"\t");
        }
        if (char2wchar_ret <= 8) {
            debug(L"%s", L"\t");
        }
        if (char2wchar_ret <= 1) {
            debug(L"%s", L"\t");
        }
        switch (shdr[i].sh_type) {
            case SHT_NULL: {
                debug(L"NULL\t\t");
                break;
            }
            case SHT_PROGBITS: {
                debug(L"PROGBITS\t");
                break;
            }
            case SHT_SYMTAB: {
                debug(L"SYMTAB\t\t");
                break;
            }
            case SHT_STRTAB: {
                debug(L"STRTAB\t\t");
                break;
            }
            case SHT_RELA: {
                debug(L"RELA\t\t");
                break;
            }
            case SHT_HASH: {
                debug(L"HASH\t\t");
                break;
            }
            case SHT_DYNAMIC: {
                debug(L"DYNAMIC\t\t");
                break;
            }
            case SHT_NOTE: {
                debug(L"NOTE\t\t");
                break;
            }
            case SHT_NOBITS: {
                debug(L"NOBITS\t\t");
                break;
            }
            case SHT_REL: {
                debug(L"REL\t\t");
                break;
            }
            case SHT_SHLIB: {
                debug(L"SHLIB\t\t");
                break;
            }
            case SHT_DYNSYM: {
                debug(L"DYNSYM\t\t");
                break;
            }
            case SHT_INIT_ARRAY: {
                debug(L"INIT_ARRAY\t");
                break;
            }
            case SHT_FINI_ARRAY: {
                debug(L"FINI_ARRAY\t");
                break;
            }
            case SHT_PREINIT_ARRAY: {
                debug(L"PREINIT_ARRAY\t\t");
                break;
            }
            case SHT_GROUP: {
                debug(L"GROUP\t\t");
                break;
            }
            case SHT_SYMTAB_SHNDX: {
                debug(L"SYMTAB_SHNDX\t\t");
                break;
            }
            case SHT_RELR: {
                debug(L"RELR\t\t");
                break;
            }
            case SHT_NUM: {
                debug(L"NUM\t\t");
                break;
            }
            case SHT_LOOS: {
                debug(L"LOOS\t\t");
                break;
            }
            case SHT_GNU_ATTRIBUTES: {
                debug(L"GNU_ATTRIBUTE\t\t");
                break;
            }
            case SHT_GNU_HASH: {
                debug(L"GNU_HASH\t");
                break;
            }
            case SHT_GNU_LIBLIST: {
                debug(L"GNU_LIBLIST\t\t");
                break;
            }
            case SHT_CHECKSUM: {
                debug(L"CHECKSUM\t\t");
                break;
            }
            case SHT_SUNW_move: {
                debug(L"SUNW_move\t\t");
                break;
            }
            case SHT_SUNW_COMDAT: {
                debug(L"SUNW_COMDAT\t\t");
                break;
            }
            case SHT_SUNW_syminfo: {
                debug(L"SUNW_syminfo\t\t");
                break;
            }
            case SHT_GNU_verdef: {
                debug(L"GNU_verdef\t\t");
                break;
            }
            case SHT_GNU_verneed: {
                debug(L"GNU_verneed\t");
                break;
            }
            case SHT_GNU_versym: {
                debug(L"GNU_versym\t");
                break;
            }
            case SHT_LOPROC: {
                debug(L"LOPROC\t\t");
                break;
            }
            case SHT_HIPROC: {
                debug(L"HIPROC\t\t");
                break;
            }
            case SHT_LOUSER: {
                debug(L"LOUSER\t\t");
                break;
            }
            case SHT_HIUSER: {
                debug(L"HIUSER\t\t");
                break;
            }
            default: {
                debug(L"Unknown 0x%X\t", shdr[i].sh_type);
                break;
            }
        }

        debug(L"0x%X\t", shdr[i].sh_addr);
        debug(L"0x%x\t\t", shdr[i].sh_offset);
        debug(L"0x%X\t", shdr[i].sh_size);
        debug(L"0x%X\t", shdr[i].sh_entsize);

        switch (shdr[i].sh_flags) {
            case 0: {
                debug(L"0\t");
                break;
            }
            case SHF_WRITE: {
                debug(L"WRITE\t");
                break;
            }
            case SHF_ALLOC: {
                debug(L"A\t");
                break;
            }
            case SHF_EXECINSTR: {
                debug(L"EXECINSTR\t");
                break;
            }
            case SHF_MERGE: {
                debug(L"MERGE\t");
                break;
            }
            case SHF_STRINGS: {
                debug(L"STRINGS\t");
                break;
            }
            case SHF_INFO_LINK: {
                debug(L"INFO_LINK\t");
                break;
            }
            case SHF_LINK_ORDER: {
                debug(L"LINK_ORDER\t");
                break;
            }
            case SHF_OS_NONCONFORMING: {
                debug(L"OS_NONCONFORMING\t");
                break;
            }
            case SHF_GROUP: {
                debug(L"GROUP\t");
                break;
            }
            case SHF_TLS: {
                debug(L"TLS\t");
                break;
            }
            case SHF_COMPRESSED: {
                debug(L"COMPRESSED\t");
                break;
            }
            case SHF_MASKOS: {
                debug(L"MASKOS\t");
                break;
            }
            case SHF_MASKPROC: {
                debug(L"MASKPROC\t");
                break;
            }
            case SHF_GNU_RETAIN: {
                debug(L"GNU_RETAIN\t");
                break;
            }
            case SHF_ORDERED: {
                debug(L"ORDERED\t");
                break;
            }
            case SHF_EXCLUDE: {
                debug(L"EXCLUDE\t");
                break;
            }
            case (SHF_WRITE | SHF_ALLOC): {
                debug(L"WA\t");
                break;
            }
            case (SHF_ALLOC | SHF_MERGE): {
                debug(L"AM\t");
                break;
            }
            case (SHF_ALLOC | SHF_EXECINSTR): {
                debug(L"AX\t");
                break;
            }
            case (SHF_MERGE | SHF_STRINGS): {
                debug(L"MS\t");
                break;
            }
            default: {
                debug(L"Unknown 0x%X\t", shdr[i].sh_flags);
                break;
            }
        }

        debug(L"%d\t", shdr[i].sh_link);
        debug(L"%d\t", shdr[i].sh_info);
        debug(L"%d\t", shdr[i].sh_addralign);
        debug(L"\n");
    }
    return;
}

void Elf::load_sections(const Elf64_Phdr& _phdr) const {
    EFI_STATUS status;
    void*      data               = nullptr;
    // 计算使用的内存页数
    uint64_t   section_page_count = EFI_SIZE_TO_PAGES(_phdr.p_memsz);

    // 设置文件偏移到 p_offset
    status = uefi_call_wrapper(elf->SetPosition, 2, elf, _phdr.p_offset);
    if (EFI_ERROR(status)) {
        debug(L"SetPosition failed %d\n", status);
        throw std::runtime_error("memory_map == nullptr");
    }
    uint64_t aaa = 0;
    // status = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAddress,
    //                            EfiLoaderData, section_page_count,
    //                            (EFI_PHYSICAL_ADDRESS*)&_phdr.p_paddr);
    status       = uefi_call_wrapper(gBS->AllocatePages, 4, AllocateAnyPages,
                                     EfiLoaderData, section_page_count,
                                     (EFI_PHYSICAL_ADDRESS*)&aaa);
    debug(L"_phdr.p_paddr: [%d] [%d] 0x%X\n", status, section_page_count, aaa);
    if (EFI_ERROR(status)) {
        debug(L"AllocatePages AllocateAddress failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    if (_phdr.p_filesz > 0) {
        auto buffer_read_size = _phdr.p_filesz;
        // 为 program_data 分配内存
        status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderCode,
                                   buffer_read_size, (void**)&data);
        if (EFI_ERROR(status)) {
            debug(L"AllocatePool failed %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
        // 读数据
        status = uefi_call_wrapper(elf->Read, 3, elf, &buffer_read_size,
                                   (void*)data);
        if (EFI_ERROR(status)) {
            debug(L"Read failed %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }

        // 将读出来的数据复制到其对应的物理地址
        uefi_call_wrapper(gBS->CopyMem, 3, (void*)aaa + _phdr.p_paddr, data,
                          _phdr.p_filesz);

        // 释放 program_data
        status = uefi_call_wrapper(gBS->FreePool, 1, data);
        if (EFI_ERROR(status)) {
            debug(L"FreePool failed %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    }

    // 计算填充大小
    EFI_PHYSICAL_ADDRESS zero_fill_start = aaa + _phdr.p_paddr + _phdr.p_filesz;
    uint64_t             zero_fill_count = _phdr.p_memsz - _phdr.p_filesz;
    if (zero_fill_count > 0) {
        debug(L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
              zero_fill_count, zero_fill_start);
        // 将填充部分置 0
        uefi_call_wrapper(gBS->SetMem, 3, (void*)zero_fill_start,
                          zero_fill_count, 0);
    }

    return;
}

void Elf::load_program_sections(void) const {
    uint64_t loaded = 0;
    for (uint64_t i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }
        load_sections(phdr[i]);
        loaded++;
    }

    if (loaded == 0) {
        debug(
          L"Fatal Error: No loadable program segments found in Kernel image\n");
        throw std::runtime_error("loaded == 0");
    }

    return;
}

Elf::Elf(const wchar_t* const _kernel_image_filename) {
    EFI_STATUS status;
    // 打开文件系统协议
    status
      = LibLocateProtocol(&FileSystemProtocol, (void**)&file_system_protocol);
    if (EFI_ERROR(status)) {
        debug(L"LibLocateProtocol failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 打开根文件系统
    status = uefi_call_wrapper(file_system_protocol->OpenVolume, 2,
                               file_system_protocol, &root_file_system);
    if (EFI_ERROR(status)) {
        debug(L"OpenVolume failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 打开 elf 文件
    status = uefi_call_wrapper(root_file_system->Open, 5, root_file_system,
                               &elf, (wchar_t*)_kernel_image_filename,
                               EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status)) {
        debug(L"Open failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 获取 elf 文件大小
    try {
        elf_file_size = get_file_size();
    } catch (std::runtime_error& _e) {
        debug(L"get_file_size failed %s\n", _e.what());
        throw std::runtime_error(_e.what());
    }
    debug(L"Kernel file size: %llu\n", elf_file_size);

    // 分配 elf 文件缓存
    status = uefi_call_wrapper(gBS->AllocatePool, 3, EfiLoaderData,
                               elf_file_size, (void**)&elf_file_buffer);
    if (EFI_ERROR(status)) {
        debug(L"AllocatePool failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 将内核文件读入内存
    status = uefi_call_wrapper(elf->Read, 3, (EFI_FILE*)elf, &elf_file_size,
                               elf_file_buffer);
    if (EFI_ERROR(status)) {
        debug(L"Read failed %d\n", status);
        throw std::runtime_error("EFI_ERROR(status)");
    }

    // 检查 elf 头数据
    auto check_elf_identity_ret = check_elf_identity();
    if (check_elf_identity_ret == false) {
        debug(L"NOT valid ELF file\n");
        throw std::runtime_error("check_elf_identity_ret == false");
    }

    // 读取 ehdr
    get_ehdr();
    print_ehdr();
    // 读取 phdr
    get_phdr();
    print_phdr();
    // 读取 shdr
    get_shdr();
    print_shdr();

    return;
}

Elf::~Elf(void) {
    try {
        EFI_STATUS status;
        // 关闭 elf 文件
        status = uefi_call_wrapper(elf->Close, 1, elf);
        if (EFI_ERROR(status)) {
            debug(L"Close failed %d\n", status);
            throw std::runtime_error("EFI_ERROR(status)");
        }
    } catch (std::runtime_error& _e) {
        debug(L"~Elf failed %s\n", _e.what());
    }
    return;
}

uint64_t Elf::load_kernel_image(void) const {
    try {
        load_program_sections();
    } catch (std::runtime_error& _e) {
        debug(L"load_kernel_image %s\n", _e.what());
    }

    return 0x06409000 + 0x1040;
}
