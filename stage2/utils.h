#ifndef _SYMB_UTILS_H
#define _SYMB_UTILS_H

#include "linuxdefs.h"
#include <stdarg.h>

void    memset(void * dst, unsigned char c, unsigned int len);
int     memcmp(void * dst, void * src, unsigned int len);
void    memcpy(void *dst, void *src, unsigned int len);
int     strlen(unsigned char *str);
void    printf(char *str, ...);

// mem alloc
void *  realloc(void * addr, size_t size);
void *  malloc(size_t len);
void    free(void * ptr);

// avoid int truncation issue
void * _mmap(void * start, long length, int prot, int flags, int fd, long offset);

#endif
