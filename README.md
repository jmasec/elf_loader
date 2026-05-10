# ELF LOADER
- [x] create a child process
- [x] mmap section in the process for our binary
- [x] load the program segments into memory

## Improvments
- [ ] no binary file, use a byte array of code
- [ ] use target process or our own process instead of child
- [ ] handle relocations
- [ ] handle dynamic resolutions
- [ ] improve the software design, too many params, better struct organization of related data, etc
- [ ] where to put the arrays of the structs at? In the headers or C files? 

TODO
1. need to parse out dyn sections now into my dynamic arrays
2. fix up the relocation
3. Look at how to fix up my dynamic resolutions

gcc -fPIE -pie -nostdlib test_reloc.c -o test_reloc
gcc -static -fPIE -pie -nostdlib test.c -o test
set follow-fork-mode child
set detach-on-fork off
break elf_loader.c:27
