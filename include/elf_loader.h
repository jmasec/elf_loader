
// #include "elf_parser.h"

#ifndef ELF_LOADER
#define ELF_LOADER

#include <stdint.h>
#include <stdbool.h>
#include "elf.h"
#include "array.h"

typedef struct loaderinfo_s{
    Elf64_Addr max_vaddr;
    Elf64_Addr min_vaddr;
    size_t total_size;
    Elf64_Addr dyn_ptr;
} loaderinfo_s;

typedef struct dyn_array{
    Elf64_Dyn **items;
    size_t count;
    size_t capacity;
}dyn_array;

extern dyn_array dyn_sections;

void inject_target_process(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, uintptr_t entry_offset);
loaderinfo_s* get_loader_info(elf64programheader_s* prog_hdr_arr, uint16_t num_entries);
void load_ptload_segments(int fd, void* base_mem, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, loaderinfo_s* loaderinfo);
void load_segment_to_memory(int fd, void* base_mem, elf64programheader_s prog_hdr, loaderinfo_s* loaderinfo);
int page_align_up(int addr, int boundary);
void* mmap_target_process(size_t total_size);
bool elf_check_valid_file(elf64header_s* elf_hdr);
bool elf_check_support(elf64header_s* elf_hdr);

#endif