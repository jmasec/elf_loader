
// #include "elf_parser.h"

#ifndef ELF_LOADER
#define ELF_LOADER

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "elf.h"
#include "elf_parser.h"

#define DEFAULT_SO_PATH1 "/lib"
#define DEFAULT_SO_PATH2 "/usr/lib"

// add dynamic symbol things here as well
// could have elfdata struct in here as well
// need to add strtab and strsz
typedef struct loaderctx_s{
    Elf64_Addr max_vaddr;
    Elf64_Addr min_vaddr;
    size_t load_size;
    Elf64_Off dyn_offset;
    Elf64_Off rela_offset;
    Elf64_Off strtable_offset;
    Elf64_Off dyn_str;
    size_t strtable_size; // to check that valid offsets into string table
    size_t relasz; 
    size_t relaent;
    size_t total_relocations;
    char* strtable;
} loaderctx_s;



enum which_process{
    NEW_PROCESS,
    SAME_PROCESS,
    REMOTE_PROCESS
};

// void inject_target_process(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, uintptr_t entry_offset, bool new_process);
void inject_target_process(elfinternal_s* elf_internal, enum which_process w_proc);
bool elf_check_support(elf64header_s* elf_hdr);

#endif