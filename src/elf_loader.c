#include "elf_loader.h"
#include <string.h>


int page_align_up(int addr, int boundary){
    if(addr % boundary != 0){
        return (addr + boundary - 1) &  ~(boundary - 1);
    }
    return addr;
}


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
    // add a functio on the elf parser to return me a elfdata_s struct so the loader knows how to handle

    return loaderctx;
}

void* mmap_target_process(size_t total_size){
    void* base_mem_region = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return base_mem_region;
}


void load_segment_to_memory(void* base_mem, elfinternal_s* elf_internal, loaderctx_s* loaderctx, size_t index){
    elf64programheader_s pgm_hdr = elf_internal->pgm_hdrs[index];

    off_t mem_offset = pgm_hdr.p_offset;
    size_t bss_size = pgm_hdr.p_memsz - pgm_hdr.p_filesz;
    uintptr_t vaddr = (void *)(pgm_hdr.p_vaddr);

    int prot = 0;
    if(pgm_hdr.p_flags & PF_R) prot |= PROT_READ;
    if(pgm_hdr.p_flags & PF_W) prot |= PROT_WRITE;
    if(pgm_hdr.p_flags & PF_X) prot |= PROT_EXEC;

    if(elf_internal->elf_ptr->type == ELF_FILE_DESCRIPTOR){
        ssize_t bytes_read = pread(elf_internal->elf_ptr->edata.e_fd, (char *)base_mem + (vaddr - loaderctx->min_vaddr), pgm_hdr.p_filesz, mem_offset);

        if(bytes_read < 0){
        printf("Error while loading PT_LOAD segment");
        }
    }
    else if(elf_internal->elf_ptr->type == ELF_MEMORY_POINTER){
        memcpy((char *)base_mem + (vaddr - loaderctx->min_vaddr), (elf_internal->elf_ptr->edata.e_ptr + mem_offset), pgm_hdr.p_filesz);
    }

    if(bss_size > 0){
        memset(((char *)base_mem + (vaddr - loaderctx->min_vaddr) + pgm_hdr.p_filesz), 0, bss_size);
    }

    mprotect((char *)base_mem + (vaddr - loaderctx->min_vaddr), (pgm_hdr.p_filesz + bss_size), prot);
}

void load_ptload_segments(void* base_mem, elfinternal_s* elf_internal, loaderctx_s* loaderctx){
// void load_ptload_segments(int fd, void* base_mem, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, loaderctx_s* loaderctx){
    if(elf_internal->elf_ptr->type == ELF_FILE_DESCRIPTOR){
        for(int i = 0; i < elf_internal->elf_hdr->e_phnum; i++){
            if(elf_internal->pgm_hdrs[i].p_type == PT_LOAD){
                load_segment_to_memory(base_mem, elf_internal, loaderctx, i);
            }
        }
    }

    else if((elf_internal->elf_ptr->type == ELF_MEMORY_POINTER)){
        for(int i = 0; i < elf_internal->elf_hdr->e_phnum; i++){
            if(elf_internal->pgm_hdrs[i].p_type == PT_LOAD){
                load_segment_to_memory(base_mem, elf_internal, loaderctx, i);
            }
        }
    }
}

bool vaddr_to_offset(){
    return true;
}

bool resolve_relocations( loaderctx_s* loaderctx,elfinternal_s* elf_internal, void* base_mem){
    // vaddr_to_offset doesnt do anything right now
    if(!vaddr_to_offset(loaderctx->rela_offset, elf_internal->pgm_hdrs)){
        printf("Failed to Translate Virtual Mem to Offset");
        return false;
    }

    loaderctx->total_relocations = loaderctx->relasz/loaderctx->relaent;


    Elf64_Rela* relocation_entry = (Elf64_Rela*)malloc(sizeof(Elf64_Rela));

    // loop for the total size of relas, an increment size of a rela
    if(elf_internal->elf_ptr->type == ELF_FILE_DESCRIPTOR){
        for(size_t i = 0; i < loaderctx->relasz; i+=loaderctx->relaent){

            // TODO: need to add relocation types switch here, maybe a function for this so I dont have 
            // two switches for fd and blob
            // pread the relocation entry into the rela struct
            ssize_t bytes_read = pread(elf_internal->elf_ptr->edata.e_fd, relocation_entry, sizeof(*relocation_entry), loaderctx->rela_offset + i);

            // then set *(base_mem + r_offset) = base_mem + r_addend;
            uint64_t *where = (uint64_t *)((uint8_t *)base_mem + (relocation_entry->r_offset - loaderctx->min_vaddr));

            uint64_t value = (uint64_t)((uint8_t *)base_mem + relocation_entry->r_addend);
            
            *where = value;
        }

    }
    else if((elf_internal->elf_ptr->type == ELF_MEMORY_POINTER)){
        for(size_t i = 0; i < loaderctx->relasz; i+=loaderctx->relaent){

            memcpy(relocation_entry, (elf_internal->elf_ptr->edata.e_ptr + loaderctx->rela_offset + i), sizeof(*relocation_entry));

            // then set *(base_mem + r_offset) = base_mem + r_addend;
            uint64_t *where = (uint64_t *)((uint8_t *)base_mem + (relocation_entry->r_offset - loaderctx->min_vaddr));

            uint64_t value = (uint64_t)((uint8_t *)base_mem + relocation_entry->r_addend);
            
            *where = value;
        }
    }

    return true;
}

// TODO Implement a dynamic linker for fun, low priority for now
bool dynamic_linker(){
    return true;
}


bool read_dynamic_entries(void* base_mem, elfinternal_s* elf_internal, loaderctx_s* loaderctx){
    bool end = false;
    // Elf64_Dyn* dyn = malloc(sizeof(Elf64_Dyn));
    Elf64_Dyn dyn; 

    if(ELF_FILE_DESCRIPTOR == elf_internal->elf_ptr->type){
        while(!end){
            
            ssize_t bytes_read = pread(elf_internal->elf_ptr->edata.e_fd, &dyn, sizeof(Elf64_Dyn), (off_t)loaderctx->dyn_offset);
            if(bytes_read < 0){
                return false;
            }

            switch (dyn.d_tag)
            {
            // end of array of dyns
            case DT_NULL:
                end = true;
                break;

            case DT_RELA:
                loaderctx->rela_offset = dyn.d_un.d_ptr;
                break;

            case DT_RELASZ:
                loaderctx->relasz = dyn.d_un.d_val;   
                break;

            case DT_RELAENT:
                loaderctx->relaent = dyn.d_un.d_val;
                break;

            default:
                // might not even need to read in the dyns at all, just free the dyns after each look adn not store them in a data struct
                break;
            }

            loaderctx->dyn_offset += sizeof(Elf64_Dyn);
            }
    }
    else if(ELF_MEMORY_POINTER == elf_internal->elf_ptr->type){
        while(!end){
            
            //size_t bytes_read = pread(elf_internal->elf_ptr->edata.e_fd, &dyn, sizeof(Elf64_Dyn), (off_t)loaderctx->dyn_offset);
            memcpy(&dyn, (elf_internal->elf_ptr->edata.e_ptr + loaderctx->dyn_offset), sizeof(Elf64_Dyn));

            switch (dyn.d_tag)
            {
            // end of array of dyns
            case DT_NULL:
                end = true;
                break;

            case DT_RELA:
                loaderctx->rela_offset = dyn.d_un.d_ptr;
                break;

            case DT_RELASZ:
                loaderctx->relasz = dyn.d_un.d_val;   
                break;

            case DT_RELAENT:
                loaderctx->relaent = dyn.d_un.d_val;
                break;

            default:
                // might not even need to read in the dyns at all, just free the dyns after each look adn not store them in a data struct
                break;
            }

            loaderctx->dyn_offset += sizeof(Elf64_Dyn);
            }
    }

    if(!resolve_relocations(loaderctx, elf_internal, base_mem)){
        return false;
    }

    if(!dynamic_linker()){
        return false;
    }

    return true;
}

void load_into_new_process(elfinternal_s* elf_internal){
    pid_t child_pid; 

    child_pid = fork();

    if(child_pid < 0){
        perror("Fork Failed");
        exit(1);
    }
    else if (child_pid == 0){
        loaderctx_s* loaderctx = get_loader_info(elf_internal->pgm_hdrs, elf_internal->elf_hdr->e_phnum);
        printf("Total size: %ld\n", loaderctx->load_size);

        // virtual mem ptr in the forked process = base address 
        void* mmap_mem = mmap_target_process(loaderctx->load_size);

        // load_ptload_segments(elf_internal->elf_ptr, mmap_mem, elf_internal->pgm_hdrs, elf_internal->elf_hdr->e_phnum, loaderctx);
        load_ptload_segments(mmap_mem, elf_internal, loaderctx);

        if(!read_dynamic_entries(mmap_mem, elf_internal, loaderctx)){
            printf("Dynamic/Relocations Failed\n");
            return;
        }


        void (*entry)(void);

        entry = (void(*)(void))((char *)mmap_mem + (elf_internal->elf_hdr->e_entry - loaderctx->min_vaddr));

        free(loaderctx);

        entry();
    }
}

void load_into_current_process(elfinternal_s* elf_internal){
    loaderctx_s* loaderctx = get_loader_info(elf_internal->pgm_hdrs, elf_internal->elf_hdr->e_phnum);
    printf("Total size: %ld\n", loaderctx->load_size);

    // virtual mem ptr in the forked process = base address 
    void* mmap_mem = mmap_target_process(loaderctx->load_size);

    // load_ptload_segments(elf_internal->elf_ptr->edata.e_fd, mmap_mem, elf_internal->pgm_hdrs, elf_internal->elf_hdr->e_phnum, loaderctx);
    load_ptload_segments(mmap_mem, elf_internal, loaderctx);

    if(!read_dynamic_entries(mmap_mem, elf_internal, loaderctx)){
        printf("Dynamic/Relocations Failed\n");
        return;
    }


    void (*entry)(void);

    entry = (void(*)(void))((char *)mmap_mem + (elf_internal->elf_hdr->e_entry - loaderctx->min_vaddr));

    free(loaderctx);

    entry();
}


void inject_target_process(elfinternal_s* elf_internal, enum which_process w_proc){
    // no remote process yet
    switch (w_proc)
    {
    case NEW_PROCESS: {
        load_into_new_process(elf_internal);
        break;
    }
    case SAME_PROCESS: {
        load_into_current_process(elf_internal);
        break;
    }
    case REMOTE_PROCESS: {
        break;
    }
    default:
        printf("Not Supported Target Process Option");
        break;
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
    if(elf_hdr->e_type != ET_REL && elf_hdr->e_type != ET_EXEC  && elf_hdr->e_type != ET_DYN){
        printf("Unsupported ELF file type.\n");
        return false;
    }
    return true;
}


