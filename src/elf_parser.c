#include "elf_parser.h"

int open_elf_file(const char* elf_path){
    int fd = open(elf_path, O_RDONLY);

    if(fd < 0){
        perror("Error opening ELF file");
        return -1;
    }

    return fd;
}

int read_elf_header(int fd, elf64header_s* elf_hdr){
    size_t num_bytes = pread(fd, elf_hdr, sizeof(elf64header_s), 0);

    if (num_bytes < 0 || num_bytes == 0){
        printf("Nothing was read, ELF read failed");
        return -1;
    }

    return 0;
}

int read_program_headers(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, size_t prog_hdr_offset, uint16_t phent_size){

    for(int i = 0; i < num_entries; i++){
        size_t num_bytes = pread(fd, prog_hdr_arr, phent_size, prog_hdr_offset);
        if(num_bytes < 0){
            perror("Error reading program headers\n");
            return -1;
        }
        prog_hdr_arr++;
        prog_hdr_offset += sizeof(elf64programheader_s);
    }

    return 0;
}

int read_section_headers(int fd, elf64sectionheader_s* section_hdr_arr, uint16_t num_entries, size_t section_hdr_offset, uint16_t sh_entsize){
    
    for(int i = 0; i < num_entries; i++){
        size_t num_bytes = pread(fd, section_hdr_arr, sh_entsize, section_hdr_offset);
        if(num_bytes < 0){
            perror("Error reading section headers\n");
            return -1;
        }
        section_hdr_arr++;
        section_hdr_offset += sizeof(elf64sectionheader_s);
    }
    
    return 0;
}

