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

In header.h: extern int my_array[3]; (This tells the compiler "this exists somewhere else").In source.c: int my_array[3] = {1, 2, 3}; (This creates the actual memory once).The Result: Every file shares the exact same memory address. This is the most efficient and common method.

Should these be in the parser, the loader just needs the arrays filled to load, its technically parsing, how do I connect my parser lib to link to my github and I can make changes there and push and pull from there

gcc -fPIE -pie -nostdlib test_reloc.c -o test_reloc
gcc -static -fPIE -pie -nostdlib test.c -o test
set follow-fork-mode child
set detach-on-fork off
break elf_loader.c:27
