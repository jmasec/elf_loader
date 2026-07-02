#include <stdio.h>


typedef unsigned long u64;

// initialized data (.data)
volatile int counter = 3;

// uninitialized data (.bss)
volatile int accumulator;

long x = 7;
long *p = &x;

// raw syscall: write(fd, buf, len)
static inline long sys_write(int fd, const void *buf, u64 len) {
    long ret;
    asm volatile (
        "mov $1, %%rax\n"
        "syscall\n"
        : "=a"(ret)
        : "D"(fd), "S"(buf), "d"(len)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// raw syscall: exit(code)
static inline void sys_exit(int code) {
    asm volatile (
        "mov $60, %%rax\n"
        "syscall\n"
        :
        : "D"(code)
        : "rcx", "r11"
    );
    __builtin_unreachable();
}

void main(void) {    
    // touch .bss
    accumulator = 0;

    *p = 42;

    // loop uses .data + .bss
    for (int i = 0; i < 5; i++) {
        accumulator += counter;
        counter++;
    }

    // visible proof of execution
    const char msg[] = "PIE loader test OK\n";
    sys_write(1, msg, sizeof(msg) - 1);
    printf("Printf Print!");

    if (p != &x)
        sys_exit(100);

    if (x != 42)
        sys_exit(101);

    if (counter != 8)
        sys_exit(102);

    if (accumulator != 25)
        sys_exit(103);

    sys_exit(0);
}
