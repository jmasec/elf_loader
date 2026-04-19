long x = 7;
long *p = &x;

void _start() {
    *p = 42;
    while (1) {}
}