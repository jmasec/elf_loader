
// #include "elf_parser.h"

#ifndef ELF_LOADER
#define ELF_LOADER

#include <stdint.h>
#include <stdbool.h>
#include "elf.h"

typedef struct elfsize_s{
    Elf64_Addr max_vaddr;
    Elf64_Addr min_vaddr;
    size_t total_size;
} elfsize_s;

void inject_target_process(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, uintptr_t entry_offset);
elfsize_s* program_section_sizes(elf64programheader_s* prog_hdr_arr, uint16_t num_entries);
void load_ptload_segments(int fd, void* base_mem, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, elfsize_s* binary_sizes);
void load_segment_to_memory(int fd, void* base_mem, elf64programheader_s prog_hdr, elfsize_s* binary_sizes);
int page_align_up(int addr, int boundary);
void* mmap_target_process(size_t total_size);
bool elf_check_valid_file(elf64header_s* elf_hdr);
bool elf_check_support(elf64header_s* elf_hdr);

#endif