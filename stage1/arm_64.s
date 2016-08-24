# ARM64 Loader

# getpc
.section .text

    # ptr to reloc num into x0
    adr x0, relocs
    # number of relocs into x1
    ldr x1, [x0]

    # begining of relocs in x2
    add x2, x0, #8

    # start addr in x3
    mov x5, #8
    mul x4, x1, x5
    add x4, x2, x4
    ldr x3, [x4]

    # begining of code in x4
    add x4, x4, #8

# fix relocs loop
loop:
    cmp x1, #0
    beq done

    # reloc addr in x0
    ldr x0, [x2]
    # reloc data ptr in x0
    add x0, x4, x0
    # data in x5
    ldr x5, [x0]
    # remap reloc
    add x5, x4, x5
    # store remapped reloc data
    str x5, [x0]

    # decrement number of relocs to process
    sub x1, x1, #1
    # go to next reloc
    add x2, x2, #8
    b loop

done:
    add x4, x3, x4
    br x4
    .align 4

relocs:
