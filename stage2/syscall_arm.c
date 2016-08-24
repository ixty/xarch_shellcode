#include "syscall_arm.h"
#include "linuxdefs.h"

ssize_t _read(int fd, void *buf, size_t size)
{
    ssize_t ret;
    register int r0 asm ("r0") = (int)fd;
    register int r1 asm ("r1") = (int)buf;
    register int r2 asm ("r2") = (int)size;
    register int r7 asm ("r7") = __NR_read;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}

ssize_t _write(int fd, const void *buf, size_t size)
{
    ssize_t ret;
    register int r0 asm ("r0") = (int)fd;
    register int r1 asm ("r1") = (int)buf;
    register int r2 asm ("r2") = (int)size;
    register int r7 asm ("r7") = __NR_write;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}

int _open(char * path, int mode, int flags)
{
    long ret;
    register int r0 asm ("r0") = (int)path;
    register int r1 asm ("r1") = (int)mode;
    register int r2 asm ("r2") = (int)flags;
    register int r7 asm ("r7") = __NR_open;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}

int _close(int fd)
{
    long ret;
    register int r0 asm ("r0") = (int)fd;
    register int r7 asm ("r7") = __NR_close;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0)
    );
    return ret;
}

long _lseek(int fd, long offset, int whence)
{
    long ret;
    register int r0 asm ("r0") = (int)fd;
    register int r1 asm ("r1") = (int)offset;
    register int r2 asm ("r2") = (int)whence;
    register int r7 asm ("r7") = __NR_lseek;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}

void * _mmap(void * start, long length, int prot, int flags, int fd, long offset)
{
    void * ret;
    register int r0 asm ("r0") = (int)start;
    register int r1 asm ("r1") = (int)length;
    register int r2 asm ("r2") = (int)prot;
    register int r3 asm ("r3") = (int)flags;
    register int r4 asm ("r4") = (int)fd;
    register int r5 asm ("r5") = (int)offset;
    register int r7 asm ("r7") = __NR_mmap2;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
    );
    return ret;
}

long _mprotect(void * addr, long len, int prot)
{
    long ret;
    register int r0 asm ("r0") = (int)addr;
    register int r1 asm ("r1") = (int)len;
    register int r2 asm ("r2") = (int)prot;
    register int r7 asm ("r7") = __NR_mprotect;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}

long _munmap(char * start, int length)
{
    long ret;
    register int r0 asm ("r0") = (int)start;
    register int r1 asm ("r1") = (int)length;
    register int r7 asm ("r7") = __NR_munmap;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1)
    );
    return ret;
}

long _brk(unsigned long addr)
{
    long ret;
    register int r0 asm ("r0") = (int)addr;
    register int r7 asm ("r7") = __NR_brk;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0)
    );
    return ret;
}

int _exit(int level)
{
    long    ret;
    register int r0 asm ("r0") = (int)level;
    register int r7 asm ("r7") = __NR_exit;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0)
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
    register int r0 asm ("r0") = (int)filename;
    register int r1 asm ("r1") = (int)argv;
    register int r2 asm ("r2") = (int)envp;
    register int r7 asm ("r7") = __NR_execve;

    asm volatile
    (
        "swi #0; mov %0, r0"
        : "=r" (ret)
        : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
    );
    return ret;
}
