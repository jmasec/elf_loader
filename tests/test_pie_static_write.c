// No stdlib, no libc, static PIE test
// Exercises: .text, .data, .bss, syscalls, entry correctness

typedef unsigned long u64;

// initialized data (.data)
volatile int counter = 3;

// uninitialized data (.bss)
volatile int accumulator;

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

void _start(void) {    
    // touch .bss
    accumulator = 0;

    // loop uses .data + .bss
    for (int i = 0; i < 5; i++) {
        accumulator += counter;
        counter++;
    }

    // visible proof of execution
    const char msg[] = "PIE loader test OK\n";
    sys_write(1, msg, sizeof(msg) - 1);

    // exit code reflects memory correctness
    // expected: counter = 8, accumulator = 25
    sys_exit(accumulator);
}
