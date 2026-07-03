# ELF LOADER
- [x] create a child process
- [x] mmap section in the process for our binary
- [x] load the program segments into memory

## Improvments
- [x] handle relocations
- [X] use target process or our own process instead of child
- [x] make the elf parser code a submodule, make all changes to it in that for that repo, the code is not owned by this one, I just need the functions
- [ ] handle dynamic resolutions
    - [ ] delagate to existing dynamic linker
        - loader maps the program
        - loader maps ld-linux
        - loader builds the right initial stack/auxv
        - loader jumps into ld-linux entry
        - ld-linux finishes the real dynamic linking
    - [ ] implement dynamic linker
- [ ] no binary file, use a byte array of code
- [ ] improve the software design, too many params, better struct organization of related data, etc
- [ ] add unit testing

TODO
1. need to parse out dyn sections now into my dynamic arrays 
2. fix up the relocation
3. Look at how to fix up my dynamic resolutions

NOTES
- have the parser return mallocd structures of the elf that we read in if from a file descriptor, else it returns a ptr to where the start if that structure is in memory, -1 if fails. Then whoever needs it can call those parser functions and get usable structures back, so you can use the functions in a util or this loader for example. For now I will continue with a just having main call and alocate things for the parser to use.
- added structs in elf.h to handle future elf files and in memory elfs

gcc -fPIE -pie -nostdlib tests/test_pie_static_write_reloc.c -o bin/test_relo
gcc -fPIE -pie -nostdlib test_reloc.c -o test_reloc
gcc -static -fPIE -pie -nostdlib test.c -o test
set follow-fork-mode child
set detach-on-fork off
break elf_loader.c:27
