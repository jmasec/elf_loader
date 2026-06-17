#include "elf_loader.h"
#include <string.h>

loaderctx_s* get_loader_info(elf64programheader_s* prog_hdr_arr, uint16_t num_entries){
    loaderctx_s* loaderctx = (loaderctx_s*)calloc(1, sizeof(loaderctx_s));
    size_t total_size = 0;
    size_t max_size = 0;
    Elf64_Addr max_vaddr = 0;
    Elf64_Addr min_vaddr = prog_hdr_arr[0].p_vaddr;

    for(int i = 0; i < num_entries; i++){
        if(prog_hdr_arr[i].p_type == PT_LOAD){
            if(prog_hdr_arr[i].p_vaddr > max_vaddr){
                max_vaddr = prog_hdr_arr[i].p_vaddr;
                max_size = prog_hdr_arr[i].p_filesz;
            }
            if(prog_hdr_arr[i].p_vaddr < min_vaddr){
                min_vaddr = prog_hdr_arr[i].p_vaddr;
            }
        }
        if(prog_hdr_arr[i].p_type == PT_DYNAMIC){
            loaderctx->dyn_offset = prog_hdr_arr[i].p_offset;
        }
    }

    loaderctx->max_vaddr = max_vaddr;
    loaderctx->min_vaddr = min_vaddr;
    loaderctx->load_size = page_align_up((max_vaddr - min_vaddr) + max_size, 0x1000);

    return loaderctx;
}


int page_align_up(int addr, int boundary){
    if(addr % boundary != 0){
        return (addr + boundary - 1) &  ~(boundary - 1);
    }
    return addr;
}

void* mmap_target_process(size_t total_size){
    void* base_mem_region = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return base_mem_region;
}


void load_segment_to_memory(int fd, void* base_mem, elf64programheader_s prog_hdr, loaderctx_s* loaderctx){
    off_t mem_offset = prog_hdr.p_offset;
    size_t bss_size = prog_hdr.p_memsz - prog_hdr.p_filesz;
    uintptr_t vaddr = (void *)(prog_hdr.p_vaddr);

    int prot = 0;
    if(prog_hdr.p_flags & PF_R) prot |= PROT_READ;
    if(prog_hdr.p_flags & PF_W) prot |= PROT_WRITE;
    if(prog_hdr.p_flags & PF_X) prot |= PROT_EXEC;

    size_t bytes_read = pread(fd, (char *)base_mem + (vaddr - loaderctx->min_vaddr), prog_hdr.p_filesz, mem_offset);

    if(bss_size > 0){
        memset(((char *)base_mem + (vaddr - loaderctx->min_vaddr) + prog_hdr.p_filesz), 0, bss_size);
    }

    if(bytes_read < 0){
        printf("Error while loading PT_LOAD segment");
    }

    mprotect((char *)base_mem + (vaddr - loaderctx->min_vaddr), (prog_hdr.p_filesz + bss_size), prot);
}


void load_ptload_segments(int fd, void* base_mem, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, loaderctx_s* loaderctx){
    for(int i = 0; i < num_entries; i++){
        if(prog_hdr_arr[i].p_type == PT_LOAD){
            load_segment_to_memory(fd, base_mem, prog_hdr_arr[i], loaderctx);
        }
    }
}

bool vaddr_to_offset(){
    return true;
}

bool resolve_relocations( loaderctx_s* loaderctx,elf64programheader_s* prog_hdr_arr, void* base_mem){
    // vaddr_to_offset doesnt do anything right now
    if(!vaddr_to_offset(loaderctx->rela_offset, prog_hdr_arr)){
        printf("Failed to Translate Virtual Mem to Offset");
        return false;
    }

    loaderctx->total_relocations = loaderctx->relasz/loaderctx->relaent;
    // might not need array, why do I need to store them? I could just handle them on the fly
    rela_array[loaderctx->total_relocations];

    for(size_t i = 0; i < loaderctx->total_relocations; i++){

    }
}


bool dynamic_linker(){
    return;
}


bool handle_dynamic_entries(int fd, loaderctx_s* loaderctx, void* base_mem, elf64programheader_s* prog_hdr_arr){
    // for now lets just read in the relavant ones we want, RELA, RELASZ, RELAENT
    // define rela_array[relasz/relaent]
    while(1){
        Elf64_Dyn* dyn = malloc(sizeof(Elf64_Dyn));
        size_t bytes_read = pread(fd, dyn, sizeof(Elf64_Dyn), (off_t)loaderctx->dyn_offset);

        switch (dyn->d_tag)
        {
        // end of array of dyns
        case DT_NULL:
            return;

        case DT_RELA:
            loaderctx->rela_offset = dyn->d_un.d_ptr;
            break;

        case DT_RELASZ:
            loaderctx->relasz = dyn->d_un.d_val;   
            break;

        case DT_RELAENT:
            loaderctx->relaent = dyn->d_un.d_val;
            break;

        default:
            // might not even need to read in the dyns at all, just free the dyns after each look adn not store them in a data struct
            break;
        }

        loaderctx->dyn_offset += sizeof(Elf64_Dyn);
    }

    if(!resolve_relocations(loaderctx, prog_hdr_arr, base_mem)){
        return false;
    }

    if(!dynamic_linker()){
        return false;
    }
}


void inject_target_process(int fd, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, uintptr_t entry_offset){
    pid_t child_pid; 

    child_pid = fork();

    if(child_pid < 0){
        perror("Fork Failed");
        exit(1);
    }
    else if (child_pid == 0){
        loaderctx_s* loaderctx = get_loader_info(prog_hdr_arr, num_entries);
        printf("Total size: %ld\n", loaderctx->load_size);

        void* mmap_mem = mmap_target_process(loaderctx->load_size);

        load_ptload_segments(fd, mmap_mem, prog_hdr_arr, num_entries, loaderctx);

        if(!handle_dynamic_entries(fd, loaderctx, mmap_mem, prog_hdr_arr)){
            printf("Dynamic/Relocations Failed\n");
            return;
        }


        void (*entry)(void);

        entry = (void(*)(void))((char *)mmap_mem + (entry_offset - loaderctx->min_vaddr));

        free(loaderctx);

        entry();
    }
}

bool elf_check_valid_file(elf64header_s* elf_hdr){
    if(elf_hdr->e_ident[EI_MAG0] != ELFMAG0){
        return false;
    }
    if(elf_hdr->e_ident[EI_MAG1] != ELFMAG1){
        printf("ELF header EI_MAG1 incorrect...\n");
        return false;
    }
    if(elf_hdr->e_ident[EI_MAG2] != ELFMAG2){
        printf("ELF header EI_MAG2 incorrect...\n");
        return false;
    }
    if(elf_hdr->e_ident[EI_MAG3] != ELFMAG3){
        printf("ELF header EI_MAG3 incorrect...\n");
        return false;
    }
    return true;

}

bool elf_check_support(elf64header_s* elf_hdr){
    if(elf_check_valid_file(elf_hdr) != true){
        printf("Invalid ELF file\n");
        return false;
    }
    if(elf_hdr->e_ident[EI_CLASS] != ELFCLASS64){
        printf("Unsupported ELF file class.\n");
        return false;
    }
    if(elf_hdr->e_ident[EI_DATA] != ELFDATA2LSB){
        printf("Unsupported ELF file byte order.\n");
        return false;
    }
    if(elf_hdr->e_machine != AMD_x86_64){
        printf("Unsupported ELF file target.\n");
        return false;
    }
    if(elf_hdr->e_ident[EI_VERSION] != EV_CURRENT){
        printf("Unsupported ELF file version.\n");
        return false;
    }
    if(elf_hdr->e_type != ET_REL && elf_hdr->e_type != ET_EXEC){
        printf("Unsupported ELF file type.\n");
        return false;
    }
    return true;
}


