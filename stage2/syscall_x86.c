#include "syscall_x86.h"
#include "linuxdefs.h"

ssize_t _read(int fd, void *buf, size_t size)
{
    ssize_t ret;
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_read), "bx"(fd), "c"(buf), "d"(size)
    );
    return ret;
}

ssize_t _write(int fd, const void *buf, size_t size)
{
    ssize_t ret;
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_write), "bx"(fd), "c"(buf), "d"(size)
    );
    return ret;
}

int _open(char *path, int mode, int flags)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_open), "bx"(path), "c"(mode), "d"(flags)
    );
    return ret;
}

int _close(int fd)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_close), "bx"(fd)
    );
    return ret;
}

long _lseek(int fd, long offset, int whence)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_lseek), "bx"(fd), "c"(offset), "d"(whence)
    );
    return ret;
}

void * _mmap(void * start, long length, int prot, int flags, int fd, long offset)
{
    register long rebp asm("ebp") = offset;
    void * ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_mmap), "b"(&start)
    );
    return ret;
}

long _mprotect(void * addr, long len, int prot)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_mprotect), "bx"(addr), "c"(len), "d"(prot)
    );
    return ret;
}

long _munmap(char * start, int length)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_munmap), "bx"(start), "c"(length)
    );
    return ret;
}

long _brk(unsigned long addr)
{
    long ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_brk), "bx"(addr)
    );

    return ret;
}

int _exit(int level)
{
    long    ret;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_exit), "bx"(level)
    );
    return (ret);
}
