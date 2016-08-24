// 2016 - ixty
// shellcode testing utility
// compile with:
// $ gcc -o sc sc.c

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

int main(int ac, char ** av)
{
    FILE * f;
    size_t l, ml;

    if(ac < 2 || !(f = fopen(av[1], "rb")))
    {
        printf("> usage %s <shellcodefile>\n", av[0]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    l = ftell(f);
    fseek(f, 0, SEEK_SET);

    ml = 0x1000;
    while(ml < l)
        ml += 0x1000;

	char * mem = (char*)mmap(NULL, ml, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(!mem)
        return 1;

    fread(mem, l, 1, f);
    fclose(f);
    mprotect(mem, ml, PROT_READ | PROT_WRITE | PROT_EXEC);

    printf("> sc len 0x%x allocated 0x%x bytes @ 0x%x\n", l, ml, mem);
	(*(void(*)()) mem)();

	return 0;
}

