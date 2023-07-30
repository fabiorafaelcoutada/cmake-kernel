
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

#ifdef __cplusplus
extern "C" {
#endif

#include "load_elf.h"

void print_ehdr(const Elf64_Ehdr& _ehdr) {
    debug((CHAR16*)L"  Magic:    ");
    for (uint64_t i = 0; i < EI_NIDENT; i++) {
        debug((CHAR16*)L"%02x ", _ehdr.e_ident[i]);
    }
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  Class:                                ");
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  Data:                                 ");
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  Version:                              %d ",
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  OS/ABI:                               ");
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  ABI Version:                          %d\n",
          _ehdr.e_ident[EI_ABIVERSION]);

    debug((CHAR16*)L"  Type:                                 ");
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  Machine:                              ");
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
    debug((CHAR16*)L"\n");

    debug((CHAR16*)L"  Version:                              0x%x\n",
          _ehdr.e_version);

    debug((CHAR16*)L"  Entry point address:                  0x%x\n",
          _ehdr.e_entry);

    debug((CHAR16*)L"  Start of program headers:             %d (bytes into "
                   L"file)\n",
          _ehdr.e_phoff);

    debug((CHAR16*)L"  Start of section headers:             %d (bytes into "
                   L"file)\n",
          _ehdr.e_shoff);

    debug((CHAR16*)L"  Flags:                                0x%x\n",
          _ehdr.e_flags);

    debug((CHAR16*)L"  Size of this header:                  %d (bytes)\n",
          _ehdr.e_ehsize);

    debug((CHAR16*)L"  Size of program headers:              %d (bytes)\n",
          _ehdr.e_phentsize);

    debug((CHAR16*)L"  Number of program headers:            %d\n",
          _ehdr.e_phnum);

    debug((CHAR16*)L"  Size of section headers:              %d (bytes)\n",
          _ehdr.e_shentsize);

    debug((CHAR16*)L"  Number of section headers:            %d\n",
          _ehdr.e_shnum);

    debug((CHAR16*)L"  Section header string table index:    %d\n",
          _ehdr.e_shstrndx);
}

void print_phdr(const Elf64_Phdr* const _phdr, size_t _phdr_num) {
    debug((CHAR16*)L"\nProgram Headers:\n");
    debug(
      (CHAR16*)L"  "
               L"Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\t"
               L"Flags\tAlign\n");
    for (uint64_t i = 0; i < _phdr_num; i++) {
        switch (_phdr[i].p_type) {
            case PT_NULL: {
                debug((CHAR16*)L"  NULL\t\t");
                break;
            }

            case PT_LOAD: {
                debug((CHAR16*)L"  LOAD\t\t");
                break;
            }
            case PT_DYNAMIC: {
                debug((CHAR16*)L"  DYNAMIC\t");
                break;
            }
            case PT_INTERP: {
                debug((CHAR16*)L"  INTERP\t\t");
                break;
            }
            case PT_NOTE: {
                debug((CHAR16*)L"  NOTE\t\t");
                break;
            }
            case PT_SHLIB: {
                debug((CHAR16*)L"  SHLIB\t\t");
                break;
            }
            case PT_PHDR: {
                debug((CHAR16*)L"  PHDR\t\t");
                break;
            }
            case PT_TLS: {
                debug((CHAR16*)L"  TLS\t\t");
                break;
            }
            case PT_NUM: {
                debug((CHAR16*)L"  NUM\t\t");
                break;
            }
            case PT_LOOS: {
                debug((CHAR16*)L"  LOOS\t\t");
                break;
            }
            case PT_GNU_EH_FRAME: {
                debug((CHAR16*)L"  GNU_EH_FRAME\t");
                break;
            }
            case PT_GNU_STACK: {
                debug((CHAR16*)L"  GNU_STACK\t");
                break;
            }
            case PT_GNU_RELRO: {
                debug((CHAR16*)L"  GNU_RELRO\t");
                break;
            }
            case PT_GNU_PROPERTY: {
                debug((CHAR16*)L"  GNU_PROPERTY\t");
                break;
            }
            case PT_SUNWBSS: {
                debug((CHAR16*)L"  SUNWBSS\t\t");
                break;
            }
            case PT_SUNWSTACK: {
                debug((CHAR16*)L"  SUNWSTACK\t");
                break;
            }
            case PT_HIOS: {
                debug((CHAR16*)L"  HIOS\t\t");
                break;
            }
            case PT_LOPROC: {
                debug((CHAR16*)L"  LOPROC\t\t");
                break;
            }
            case PT_HIPROC: {
                debug((CHAR16*)L"  HIPROC\t\t");
                break;
            }
            default: {
                debug((CHAR16*)L"  Unknown 0x%X\t", _phdr[i].p_type);
                break;
            }
        }

        debug((CHAR16*)L"0x%X\t", _phdr[i].p_offset);
        debug((CHAR16*)L"0x%X\t", _phdr[i].p_vaddr);
        debug((CHAR16*)L"0x%X\t", _phdr[i].p_paddr);
        debug((CHAR16*)L"0x%X\t", _phdr[i].p_filesz);
        debug((CHAR16*)L"0x%X\t", _phdr[i].p_memsz);

        switch (_phdr[i].p_flags) {
            case PF_X: {
                debug((CHAR16*)L"E\t");
                break;
            }
            case PF_W: {
                debug((CHAR16*)L"W\t");
                break;
            }
            case PF_R: {
                debug((CHAR16*)L"R\t");
                break;
            }
            case PF_MASKOS: {
                debug((CHAR16*)L"OS-specific\t");
                break;
            }
            case PF_MASKPROC: {
                debug((CHAR16*)L"Processor-specific\t");
                break;
            }
            case (PF_X | PF_R): {
                debug((CHAR16*)L"R E\t");
                break;
            }
            case (PF_W | PF_R): {
                debug((CHAR16*)L"RW\t");
                break;
            }
            default: {
                debug((CHAR16*)L"Unknown 0x%x\t", _phdr[i].p_flags);
                break;
            }
        }

        debug((CHAR16*)L"0x%x\n", _phdr[i].p_align);
    }
}

void print_shdr(const Elf64_Shdr* const _shdr, size_t _shdr_num) {
    debug((CHAR16*)L"\nSection Headers:\n");
    debug((CHAR16*)L"  [Nr] "
                   L"Name\t\tType\t\tAddress\t\tOffset\t\tSize\t\tEntSize\t\tFl"
                   L"ags\tLink\tInfo\tAlign\n");
    for (uint64_t i = 0; i < _shdr_num; i++) {
        debug((CHAR16*)L"  [%2d] ", i);
        debug((CHAR16*)L"%d\t\t", _shdr[i].sh_name);
        switch (_shdr[i].sh_type) {
            case SHT_NULL: {
                debug((CHAR16*)L"NULL\t\t");
                break;
            }
            case SHT_PROGBITS: {
                debug((CHAR16*)L"PROGBITS\t");
                break;
            }
            case SHT_SYMTAB: {
                debug((CHAR16*)L"SYMTAB\t\t");
                break;
            }
            case SHT_STRTAB: {
                debug((CHAR16*)L"STRTAB\t\t");
                break;
            }
            case SHT_RELA: {
                debug((CHAR16*)L"RELA\t\t");
                break;
            }
            case SHT_HASH: {
                debug((CHAR16*)L"HASH\t\t");
                break;
            }
            case SHT_DYNAMIC: {
                debug((CHAR16*)L"DYNAMIC\t\t");
                break;
            }
            case SHT_NOTE: {
                debug((CHAR16*)L"NOTE\t\t");
                break;
            }
            case SHT_NOBITS: {
                debug((CHAR16*)L"NOBITS\t\t");
                break;
            }
            case SHT_REL: {
                debug((CHAR16*)L"REL\t\t");
                break;
            }
            case SHT_SHLIB: {
                debug((CHAR16*)L"SHLIB\t\t");
                break;
            }
            case SHT_DYNSYM: {
                debug((CHAR16*)L"DYNSYM\t\t");
                break;
            }
            case SHT_INIT_ARRAY: {
                debug((CHAR16*)L"INIT_ARRAY\t\t");
                break;
            }
            case SHT_FINI_ARRAY: {
                debug((CHAR16*)L"FINI_ARRAY\t\t");
                break;
            }
            case SHT_PREINIT_ARRAY: {
                debug((CHAR16*)L"PREINIT_ARRAY\t\t");
                break;
            }
            case SHT_GROUP: {
                debug((CHAR16*)L"GROUP\t\t");
                break;
            }
            case SHT_SYMTAB_SHNDX: {
                debug((CHAR16*)L"SYMTAB_SHNDX\t\t");
                break;
            }
            case SHT_RELR: {
                debug((CHAR16*)L"RELR\t\t");
                break;
            }
            case SHT_NUM: {
                debug((CHAR16*)L"NUM\t\t");
                break;
            }
            case SHT_LOOS: {
                debug((CHAR16*)L"LOOS\t\t");
                break;
            }
            case SHT_GNU_ATTRIBUTES: {
                debug((CHAR16*)L"GNU_ATTRIBUTE\t\t");
                break;
            }
            case SHT_GNU_HASH: {
                debug((CHAR16*)L"GNU_HASH\t");
                break;
            }
            case SHT_GNU_LIBLIST: {
                debug((CHAR16*)L"GNU_LIBLIST\t\t");
                break;
            }
            case SHT_CHECKSUM: {
                debug((CHAR16*)L"CHECKSUM\t\t");
                break;
            }
            case SHT_SUNW_move: {
                debug((CHAR16*)L"SUNW_move\t\t");
                break;
            }
            case SHT_SUNW_COMDAT: {
                debug((CHAR16*)L"SUNW_COMDAT\t\t");
                break;
            }
            case SHT_SUNW_syminfo: {
                debug((CHAR16*)L"SUNW_syminfo\t\t");
                break;
            }
            case SHT_GNU_verdef: {
                debug((CHAR16*)L"GNU_verdef\t\t");
                break;
            }
            case SHT_GNU_verneed: {
                debug((CHAR16*)L"GNU_verneed\t\t");
                break;
            }
            case SHT_GNU_versym: {
                debug((CHAR16*)L"GNU_versym\t\t");
                break;
            }
            case SHT_LOPROC: {
                debug((CHAR16*)L"LOPROC\t\t");
                break;
            }
            case SHT_HIPROC: {
                debug((CHAR16*)L"HIPROC\t\t");
                break;
            }
            case SHT_LOUSER: {
                debug((CHAR16*)L"LOUSER\t\t");
                break;
            }
            case SHT_HIUSER: {
                debug((CHAR16*)L"HIUSER\t\t");
                break;
            }
            default: {
                debug((CHAR16*)L"Unknown 0x%X\t", _shdr[i].sh_type);
                break;
            }
        }

        debug((CHAR16*)L"0x%X\t", _shdr[i].sh_addr);
        debug((CHAR16*)L"0x%x\t\t", _shdr[i].sh_offset);
        debug((CHAR16*)L"0x%X\t", _shdr[i].sh_size);
        debug((CHAR16*)L"0x%X\t", _shdr[i].sh_entsize);
        debug((CHAR16*)L"0x%x\t", _shdr[i].sh_flags);
        debug((CHAR16*)L"%d\t", _shdr[i].sh_link);
        debug((CHAR16*)L"%d\t", _shdr[i].sh_info);
        debug((CHAR16*)L"%d\t", _shdr[i].sh_addralign);
        debug((CHAR16*)L"\n");
    }
}

#ifdef __cplusplus
}
#endif
