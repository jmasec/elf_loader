
// #include "elf_parser.h"

#ifndef ELF_LOADER
#define ELF_LOADER

#include <stdint.h>
#include <stdbool.h>
#include "elf.h"
#include "elf_parser.h"

// add dynamic symbol things here as well
// could have elfdata struct in here as well
typedef struct loaderctx_s{
    Elf64_Addr max_vaddr;
    Elf64_Addr min_vaddr;
    size_t load_size;
    Elf64_Off dyn_offset;
    Elf64_Off rela_offset;
    size_t relasz; 
    size_t relaent;
    size_t total_relocations; 
} loaderctx_s;

extern Elf64_Rela rela_array[];

void inject_target_process(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, uintptr_t entry_offset);



#endif