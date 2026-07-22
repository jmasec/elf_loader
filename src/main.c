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

    return 0;
}