#include "utils.h"

void memset(void * dst, unsigned char c, unsigned int len)
{
    unsigned char * p = (unsigned char *) dst;

    while(len--)
        *p++ = c;
}

int memcmp(void * dst, void * src, unsigned int len)
{
    unsigned char * d = (unsigned char *) dst;
    unsigned char * s = (unsigned char *) src;

    while(len-- > 0)
        if(*d++ != *s++)
            return 1;

    return 0;
}

void memcpy(void *dst, void *src, unsigned int len)
{
    unsigned char * d = (unsigned char *) dst;
    unsigned char * s = (unsigned char *) src;

    while(len--)
        *d++ = *s++;
}

int strlen(unsigned char *str)
{
    int n = 0;

    while(*str++)
        n++;

    return n;
}

void printf(char *str, ...)
{
    int     len;
    va_list vl;
    char    buf[4096];

    va_start (vl, str);
    len = vsnprintf (buf, sizeof (buf), str, vl);
    va_end (vl);
    buf[sizeof (buf) - 1] = '\0';

    _write(1, buf, len);

    return;
}


// MBH malloc
// use mmap for every required block
// block header = [unsigned long user_size] [unsigned long allocd_size]
#define MBM_PAGE_SIZE 0x1000
#define MBM_SIZE_USER(ptr) (*(unsigned long *) ((unsigned long)(ptr) - 2 * sizeof(unsigned long)))
#define MBM_SIZE_ALLOC(ptr) (*(unsigned long *) ((unsigned long)(ptr) - 1 * sizeof(unsigned long)))
#define MBM_SIZE_HDR (2 * sizeof(unsigned long))

void * realloc(void * addr, size_t size)
{
    size_t alloc_size;
    unsigned long mem;

    if(!size)
        return NULL;

    if(addr && size < 0x1000 - MBM_SIZE_HDR)
    {
        MBM_SIZE_USER(addr) = size;
        return addr;
    }

    alloc_size = size + MBM_SIZE_HDR;
    if(alloc_size % MBM_PAGE_SIZE)
        alloc_size = ((alloc_size / MBM_PAGE_SIZE) + 1) * MBM_PAGE_SIZE;

    mem = _mmap(NULL, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(mem < 0)
    {
        printf("> memory allocation error (0x%x bytes)\n", alloc_size);
        return NULL;
    }

    mem += MBM_SIZE_HDR;
    MBM_SIZE_USER(mem) = size;
    MBM_SIZE_ALLOC(mem) = alloc_size;

    if(addr && MBM_SIZE_USER(addr))
        memcpy((void*)mem, addr, MBM_SIZE_USER(addr));
    if(addr)
        free(addr);

    return (void*)mem;
}

void * malloc(size_t len)
{
    return realloc(NULL, len);
}

void free(void * ptr)
{
    char * page = (char *)(ptr) - MBM_SIZE_HDR;

    if(!ptr)
        return;

    _munmap(page, MBM_SIZE_ALLOC(ptr));
}


