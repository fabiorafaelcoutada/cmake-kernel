
/**
 * @file printf_elf_info.cpp
 * @brief 打印 elf 文件信息
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

size_t char2wchar(const char* _s, CHAR16* _d) {
    size_t i = 0;
    while (_s[i] != '\0') {
        _d[i] = _s[i];
        i++;
    }
    return i;
}

void print_ehdr(const Elf64_Ehdr& _ehdr) {
    debug(L"  Magic:    ");
    for (uint64_t i = 0; i < EI_NIDENT; i++) {
        debug(L"%02x ", _ehdr.e_ident[i]);
    }
    debug(L"\n");

    debug(L"  Class:                                ");
    switch (_ehdr.e_ident[EI_CLASS]) {
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
            debug(L"%d", _ehdr.e_ident[EI_CLASS]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Data:                                 ");
    switch (_ehdr.e_ident[EI_DATA]) {
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
            debug(L"%d", _ehdr.e_ident[EI_DATA]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Version:                              %d ",
          _ehdr.e_ident[EI_VERSION]);
    switch (_ehdr.e_ident[EI_VERSION]) {
        case EV_NONE: {
            debug(L"Invalid ELF version");
            break;
        }
        case EV_CURRENT: {
            debug(L"Current version");
            break;
        }
        default: {
            debug(L"%d", _ehdr.e_ident[EI_VERSION]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  OS/ABI:                               ");
    switch (_ehdr.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV: {
            debug(L"UNIX System V ABI");
            break;
        }
        default: {
            debug(L"%d", _ehdr.e_ident[EI_OSABI]);
            break;
        }
    }
    debug(L"\n");

    debug(L"  ABI Version:                          %d\n",
          _ehdr.e_ident[EI_ABIVERSION]);

    debug(L"  Type:                                 ");
    switch (_ehdr.e_type) {
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
            debug(L"%d", _ehdr.e_type);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Machine:                              ");
    switch (_ehdr.e_machine) {
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
            debug(L"%d", _ehdr.e_machine);
            break;
        }
    }
    debug(L"\n");

    debug(L"  Version:                              0x%x\n", _ehdr.e_version);

    debug(L"  Entry point address:                  0x%x\n", _ehdr.e_entry);

    debug(L"  Start of program headers:             %d (bytes into "
          L"file)\n",
          _ehdr.e_phoff);

    debug(L"  Start of section headers:             %d (bytes into "
          L"file)\n",
          _ehdr.e_shoff);

    debug(L"  Flags:                                0x%x\n", _ehdr.e_flags);

    debug(L"  Size of this header:                  %d (bytes)\n",
          _ehdr.e_ehsize);

    debug(L"  Size of program headers:              %d (bytes)\n",
          _ehdr.e_phentsize);

    debug(L"  Number of program headers:            %d\n", _ehdr.e_phnum);

    debug(L"  Size of section headers:              %d (bytes)\n",
          _ehdr.e_shentsize);

    debug(L"  Number of section headers:            %d\n", _ehdr.e_shnum);

    debug(L"  Section header string table index:    %d\n", _ehdr.e_shstrndx);
}

void print_phdr(const Elf64_Phdr* const _phdr, size_t _phdr_num) {
    debug(L"\nProgram Headers:\n");
    debug(
      (CHAR16*)L"  "
               L"Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\t"
               L"Flags\tAlign\n");
    for (uint64_t i = 0; i < _phdr_num; i++) {
        switch (_phdr[i].p_type) {
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
                debug(L"  Unknown 0x%X\t", _phdr[i].p_type);
                break;
            }
        }

        debug(L"0x%X\t", _phdr[i].p_offset);
        debug(L"0x%X\t", _phdr[i].p_vaddr);
        debug(L"0x%X\t", _phdr[i].p_paddr);
        debug(L"0x%X\t", _phdr[i].p_filesz);
        debug(L"0x%X\t", _phdr[i].p_memsz);

        switch (_phdr[i].p_flags) {
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
                debug(L"Unknown 0x%x\t", _phdr[i].p_flags);
                break;
            }
        }

        debug(L"0x%x\n", _phdr[i].p_align);
    }
}

void print_shdr(const Elf64_Shdr* const _shdr, size_t _shdr_num) {
    debug(L"\nSection Headers:\n");
    debug(L"  [Nr] "
          L"Name\t\t\tType\t\tAddress\t\tOffset\t\tSize\t\tEntSize\t\tFl"
          L"ags\tLink\tInfo\tAlign\n");
    for (uint64_t i = 0; i < _shdr_num; i++) {
        debug(L"  [%2d] ", i);
        CHAR16 buf[SECTION_BUF_SIZE];
        bzero(buf, SECTION_BUF_SIZE);
        auto char2wchar_ret
          = char2wchar((const char*)(shstrtab_buf + _shdr[i].sh_name), buf);
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
        switch (_shdr[i].sh_type) {
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
                debug(L"Unknown 0x%X\t", _shdr[i].sh_type);
                break;
            }
        }

        debug(L"0x%X\t", _shdr[i].sh_addr);
        debug(L"0x%x\t\t", _shdr[i].sh_offset);
        debug(L"0x%X\t", _shdr[i].sh_size);
        debug(L"0x%X\t", _shdr[i].sh_entsize);

        switch (_shdr[i].sh_flags) {
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
            case (SHF_ALLOC | SHF_EXECINSTR): {
                debug(L"AX\t");
                break;
            }
            case (SHF_MERGE | SHF_STRINGS): {
                debug(L"MS\t");
                break;
            }
            default: {
                debug(L"Unknown 0x%X\t", _shdr[i].sh_flags);
                break;
            }
        }

        debug(L"%d\t", _shdr[i].sh_link);
        debug(L"%d\t", _shdr[i].sh_info);
        debug(L"%d\t", _shdr[i].sh_addralign);
        debug(L"\n");
    }
}
