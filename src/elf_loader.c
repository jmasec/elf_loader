#include "elf_loader.h"
#include <string.h>


elfsize_s* program_section_sizes(elf64programheader_s* prog_hdr_arr, uint16_t num_entries){
    elfsize_s* binary_sizes = (elfsize_s*)malloc(sizeof(elfsize_s));
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
    }

    binary_sizes->max_vaddr = max_vaddr;
    binary_sizes->min_vaddr = min_vaddr;
    binary_sizes->total_size = page_align_up((max_vaddr - min_vaddr) + max_size, 0x1000);

    return binary_sizes;
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


void load_segment_to_memory(int fd, void* base_mem, elf64programheader_s prog_hdr, elfsize_s* binary_sizes){
    off_t mem_offset = prog_hdr.p_offset;
    size_t bss_size = prog_hdr.p_memsz - prog_hdr.p_filesz;
    uintptr_t vaddr = (void *)(prog_hdr.p_vaddr);

    int prot = 0;
    if(prog_hdr.p_flags & PF_R) prot |= PROT_READ;
    if(prog_hdr.p_flags & PF_W) prot |= PROT_WRITE;
    if(prog_hdr.p_flags & PF_X) prot |= PROT_EXEC;

    size_t bytes_read = pread(fd, (char *)base_mem + (vaddr - binary_sizes->min_vaddr), prog_hdr.p_filesz, mem_offset);

    if(bss_size > 0){
        memset(((char *)base_mem + (vaddr - binary_sizes->min_vaddr) + prog_hdr.p_filesz), 0, bss_size);
    }

    if(bytes_read < 0){
        printf("Error while loading PT_LOAD segment");
    }

    mprotect((char *)base_mem + (vaddr - binary_sizes->min_vaddr), (prog_hdr.p_filesz + bss_size), prot);
}


void load_ptload_segments(int fd, void* base_mem, elf64programheader_s* prog_hdr_arr, uint16_t num_entries, elfsize_s* binary_sizes){
    for(int i = 0; i < num_entries; i++){
        if(prog_hdr_arr[i].p_type == PT_LOAD){
            load_segment_to_memory(fd, base_mem, prog_hdr_arr[i], binary_sizes);
        }
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
        elfsize_s* binary_sizes = program_section_sizes(prog_hdr_arr, num_entries);
        printf("Total size: %ld\n", binary_sizes->total_size);

        void* mmap_mem = mmap_target_process(binary_sizes->total_size);

        load_ptload_segments(fd, mmap_mem, prog_hdr_arr, num_entries, binary_sizes);

        void (*entry)(void);

        entry = (void(*)(void))((char *)mmap_mem + (entry_offset - binary_sizes->min_vaddr));

        free(binary_sizes);

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


