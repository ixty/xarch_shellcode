#ifndef _SYMB_LINUX_DEFS_H
#define _SYMB_LINUX_DEFS_H

#include <stddef.h>
#include <stdint.h>

#define ssize_t long
#define size_t unsigned long

#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT   0100
#define O_TRUNC  01000
#define O_APPEND 02000


#define F_DUPFD     0
#define F_GETFD     1
#define F_SETFD     2
#define F_GETFL     3
#define F_SETFL     4


#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2


#define PROT_READ   0x1
#define PROT_WRITE  0x2
#define PROT_EXEC   0x4
#define PROT_NONE   0x0
#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02
#define MAP_TYPE    0x0f
#define MAP_FIXED   0x10
#define MAP_ANONYMOUS   0x20

#endif
