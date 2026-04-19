#ifndef ELF_PARSER
#define ELF_PARSER


#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "elf.h"

int open_elf_file(const char* elf_path);
int read_elf_header(int fd, elf64header_s* elf_hdr);
int read_section_headers(int fd, elf64sectionheader_s* section_hdr_arr, uint16_t num_entries, size_t section_hdr_offset, uint16_t sh_entsize);
int read_program_headers(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, size_t prog_hdr_offset, uint16_t phent_size);

#endif