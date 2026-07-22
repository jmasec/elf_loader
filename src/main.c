#include "elf_loader.h"
#include "elf_parser.h"
#include "payload.h"

#define ELF_TYPE 0
#define ELF_PATH "test_reloc_write"

int main(){

    enum elf_ptr_type_s type;

    if(ELF_TYPE){
        type = ELF_FILE_DESCRIPTOR;
    }
    else{
        type = ELF_MEMORY_POINTER;
    }

     elfinternal_s* elf_internal = calloc(1, sizeof(elfinternal_s));

     if(type == ELF_FILE_DESCRIPTOR){
        if(!parse_elf_internal(ELF_PATH, type, elf_internal)){
            printf("Failed parsing ELF file\n");
            return -1;
        }
    }
    else{
        if(!parse_elf_internal((char *)bin_test_reloc_write, type, elf_internal)){
            printf("Failed parsing ELF memory pointer\n");
            return -1;
        }
    }


    inject_target_process(elf_internal, NEW_PROCESS);
    // int fd = open_elf_file("test_reloc");
    // if(fd < 0){
    //     return -1;
    // }
    // printf("Opened ELF\n");

    // elf64header_s* elf_hdr = calloc(1, sizeof(elf64header_s));


    // if(read_elf_header(fd, elf_hdr) != 0){
    //     perror("Failed to read in ELF header\n");
    //     return -1;
    // }

    // printf("Read ELF Header\n");

    // if(elf_check_support(elf_hdr) != true){
    //     return -1;
    // }

    // printf("ELF is Supported\n");

    // uint16_t num_prog_entries = elf_hdr->e_phnum;

    // elf64programheader_s prog_hdr_arr[num_prog_entries];

    // if(read_program_headers(fd, prog_hdr_arr, num_prog_entries, elf_hdr->e_phoff, elf_hdr->e_phentsize) < 0){
    //     printf("Failed to read in program headers\n");
    //     return -1;
    // }

    // size_t num_section_entries = elf_hdr->e_shnum;

    // elf64sectionheader_s section_hdr_arr[num_section_entries];

    // if(read_section_headers(fd, section_hdr_arr, num_section_entries, elf_hdr->e_shoff, elf_hdr->e_shentsize) < 0){
    //     printf("Failed to read in section headers\n");
    //     return -1;
    // }

    // inject_target_process(fd, prog_hdr_arr, num_prog_entries, elf_hdr->e_entry, 0);

    // free(elf_hdr);
    // close(fd);

    return 0;
}