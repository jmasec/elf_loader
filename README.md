# ELF LOADER
- [x] create a child process
- [x] mmap section in the process for our binary
- [x] load the program segments into memory

## Improvments
- [x] handle relocations
- [X] use target process or our own process instead of child
- [x] make the elf parser code a submodule, make all changes to it in that for that repo, the code is not owned by this one, I just need the functions
- [X] no binary file, use a byte array of code.
- [X] update code to use elfinternel from the parser code
- [ ] add unit testing, test cases for elf files
- [ ] implement dynamic linker (larger task, first with ELF file)
    - DT_NEEDED parsing
    - dynamic symbol and string tables
    - eager external symbol resolution
    - R_X86_64_RELATIVE
    - R_X86_64_GLOB_DAT
    - R_X86_64_JUMP_SLOT
    - R_X86_64_64
    - .init_array
    - malformed-input and unsupported-feature handling
- [ ] improve the software design, too many params, better struct organization of related data, etc (continuous)

## Future
- Obfuscated ELF bytes
- using memfd_create (wont use the actual loader at that point)
    - stream bytes in from the network
    - copy bytes from 

gcc -fPIE -pie -nostdlib tests/test_pie_static_write_reloc.c -o bin/test_relo
gcc -fPIE -pie -nostdlib test_reloc.c -o test_reloc
gcc -static -fPIE -pie -nostdlib test.c -o test
set follow-fork-mode child
set detach-on-fork off
break elf_loader.c:27

SYNC SUBMODULE
cd elf_parser
git pull origin main
cd ../..
git add elf_parser
git commit -m "update submodule"
git push