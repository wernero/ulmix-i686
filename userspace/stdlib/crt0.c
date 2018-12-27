#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int main(void);

void _start(void)
{
    int ret = main();
    // setup standard I/O  -- temporary, the
    //stdout = fopen("/dev/tty", "rw");
    //stdin = stderr = stdout;

    __asm__("mov $0x1, %%eax;"
            "mov %0, %%ebx;"
            "int $0x80"
            : : "g"(ret));
}
