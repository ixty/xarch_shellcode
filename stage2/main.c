// #include "utils.h"

void _start()
{
    // simple /bin/sh shellcode
    _execve("/bin/sh", NULL, NULL);

    // // test a few of the syscalls
    // printf("> _start @ 0x%llx\n", _start);
    // void * mem = _mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // printf("> hello, mem @ 0x%llx\n", mem);
    // *(int*)mem = 1;
    // _exit(3);
}
