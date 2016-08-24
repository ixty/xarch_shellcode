#include "syscall_x86_64.h"
#include "linuxdefs.h"

ssize_t _read(int fd, void *buf, size_t size)
{
    ssize_t ret;
    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_read), "D"(fd), "S"(buf), "d"(size)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}


ssize_t _write(int fd, const void *buf, size_t size)
{
    ssize_t ret;
    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

int _open(char *path, int mode, int flags)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_open), "D"(path), "S"(mode), "d"(flags)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

int _close(int fd)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_close), "D"(fd)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

long _lseek(int fd, long offset, int whence)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_lseek), "D"(fd), "S"(offset), "d"(whence)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

void * _mmap(void * start, long length, int prot, int flags, int fd, long offset)
{
    register int r10 asm("r10") = flags;
    register int r8 asm("r8") = fd;
    register int r9 asm("r9") = offset;
    void * ret = 0;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_mmap), "D"(start), "S"(length), "d"(prot), "r"(r10), "r"(r8), "r"(r9)
        : "cc", "rcx", "r11", "memory"
    );

    return ret;
}

long _mprotect(void * addr, long len, int prot)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_mprotect), "D"(addr), "S"(len), "d"(prot)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

long _munmap(char * start, int length)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_munmap), "D"(start), "S"(length)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

long _brk(unsigned long addr)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_brk), "D"(addr)
        : "cc", "rcx", "r11", "memory"
    );

    return ret;
}

int _exit(int level)
{
    long    ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_exit), "D"(level)
        : "cc", "rcx", "r11", "memory"
    );
    return (ret);
}

long _execve(char * filename, char ** argv, char ** envp)
{
    long ret;

    asm volatile
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_execve), "D"(filename), "S"(argv), "d"(envp)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}
