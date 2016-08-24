#include "syscall_arm_64.h"
#include "linuxdefs.h"

ssize_t _read(int fd, void *buf, size_t size)
{
    ssize_t ret;
    register long x0 asm ("x0") = (long)fd;
    register long x1 asm ("x1") = (long)buf;
    register long x2 asm ("x2") = (long)size;
    register long x8 asm ("x8") = __NR_read;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}

ssize_t _write(int fd, const void *buf, size_t size)
{
    ssize_t ret;
    register long x0 asm ("x0") = (long)fd;
    register long x1 asm ("x1") = (long)buf;
    register long x2 asm ("x2") = (long)size;
    register long x8 asm ("x8") = __NR_write;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}

int _open(char * path, int mode, int flags)
{
    long ret;
    register long x0 asm ("x0") = (long)path;
    register long x1 asm ("x1") = (long)mode;
    register long x2 asm ("x2") = (long)flags;
    register long x8 asm ("x8") = __NR_open;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}

int _close(int fd)
{
    long ret;
    register long x0 asm ("x0") = (long)fd;
    register long x8 asm ("x8") = __NR_close;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0)
    );
    return ret;
}

long _lseek(int fd, long offset, int whence)
{
    long ret;
    register long x0 asm ("x0") = (long)fd;
    register long x1 asm ("x1") = (long)offset;
    register long x2 asm ("x2") = (long)whence;
    register long x8 asm ("x8") = __NR_lseek;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}

void * _mmap(void * start, long length, int prot, int flags, int fd, long offset)
{
    void * ret;
    register long x0 asm ("x0") = (long)start;
    register long x1 asm ("x1") = (long)length;
    register long x2 asm ("x2") = (long)prot;
    register long x3 asm ("x3") = (long)flags;
    register long x4 asm ("x4") = (long)fd;
    register long x5 asm ("x5") = (long)offset;
    register long x8 asm ("x8") = __NR_mmap;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
    );
    return ret;
}

long _mprotect(void * addr, long len, int prot)
{
    long ret;
    register long x0 asm ("x0") = (long)addr;
    register long x1 asm ("x1") = (long)len;
    register long x2 asm ("x2") = (long)prot;
    register long x8 asm ("x8") = __NR_mprotect;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}

long _munmap(char * start, int length)
{
    long ret;
    register long x0 asm ("x0") = (long)start;
    register long x1 asm ("x1") = (long)length;
    register long x8 asm ("x8") = __NR_munmap;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1)
    );
    return ret;
}

long _brk(unsigned long addr)
{
    long ret;
    register long x0 asm ("x0") = (long)addr;
    register long x8 asm ("x8") = __NR_brk;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0)
    );
    return ret;
}

int _exit(int level)
{
    long    ret;
    register long x0 asm ("x0") = (long)level;
    register long x8 asm ("x8") = __NR_exit;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0)
    );
    return ret;
}

void raise(void)
{
    _exit(-1);
}

long _execve(char * filename, char ** argv, char ** envp)
{
    long ret;
    register long x0 asm ("x0") = (long)filename;
    register long x1 asm ("x1") = (long)argv;
    register long x2 asm ("x2") = (long)envp;
    register long x8 asm ("x8") = __NR_execve;

    asm volatile
    (
        "svc #0; mov %0, x0"
        : "=r" (ret)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
    );
    return ret;
}
