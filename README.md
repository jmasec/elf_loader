# ELF LOADER
- [x] create a child process
- [x] mmap section in the process for our binary
- [x] load the program segments into memory

## Improvments
- [ ] no binary file, use a byte array of code
- [ ] use target process or our own process instead of child
- [ ] handle relocations
- [ ] handle dynamic resolutions


gcc -fPIE -pie -nostdlib test_reloc.c -o test_reloc
gcc -static -fPIE -pie -nostdlib test.c -o test
set follow-fork-mode child
set detach-on-fork off
break elf_loader.c:27
