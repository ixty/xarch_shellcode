# x86 Loader

# getpc
.section .text
    jmp getpc1
getpc2:
    jmp begin
getpc1:
    call getpc2

# loader code
begin:
    popl %esi
    subl $(begin-relocs), %esi

    # esi now relocs
    movl (%esi), %ecx           # get num relocs
    leal 8(%esi,%ecx,4), %edi   # start of code
    andl %ecx, %ecx
    jz done

# fix relocs loop
fix_reloc:
    movl (%esi,%ecx,4), %eax
    addl %edi, (%edi,%eax,1)
    dec %ecx
    jne fix_reloc

# start shellcode now
done:
    addl -4(%edi), %edi
    jmp *%edi

.align 4

relocs:
# [num_relocs 4b] [relocs 4b * N] [start 4b] [code xB]
