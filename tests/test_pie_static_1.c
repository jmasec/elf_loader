int main();


void _start() {
    int ret = main();
    register int code asm("rbx") = ret;
    asm volatile(
        "mov $60, %rax\n"  // System call number for exit (60)
        "mov %rbx, %rdi\n"   // Exit status (0 for success)
        "syscall\n"        // Execute the system call
    );
}

int main(){
    int test = 1;

    for(int i = 0; i < 40; i ++){
        test+=i;
    }

    return 0;
}

// gcc -static -fPIE -pie -nostdlib test.c -o test