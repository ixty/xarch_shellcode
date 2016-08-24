# x86_64 Loader

# getpc
.section .text
    jmp getpc1
getpc2:
    jmp begin
getpc1:
    call getpc2

# loader code
begin:
    popq %rsi
    subq $(begin-relocs), %rsi

    # esi now relocs
    movq (%rsi), %rcx           # get num relocs
    leaq 16(%rsi,%rcx,8), %rdi  # start of code
    andq %rcx, %rcx
    jz done

# fix relocs loop
fix_reloc:
    movq (%rsi,%rcx,8), %rax
    addq %rdi, (%rdi,%rax,1)
    dec %rcx
    jne fix_reloc

# start shellcode now
done:
    addq -8(%rdi), %rdi
    jmpq *%rdi

.align 8

relocs:
# [num_relocs 4b] [relocs 4b * N] [start 4b] [code xB]
