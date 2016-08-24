# ARM Loader

# getpc
.section .text

    # ptr to reloc num into r0
    adrl r0, relocs
    # number of relocs into r1
    ldr r1, [r0]

    # begining of relocs in r2
    add r2, r0, #4

    # start addr in r3
    mov r5, #4
    mul r4, r1, r5
    add r4, r2, r4
    ldr r3, [r4]

    # begining of code in r4
    add r4, r4, #4

# fix relocs loop
loop:
    cmp r1, #0
    beq done

    # reloc addr in r0
    ldr r0, [r2]
    # reloc data ptr in r0
    add r0, r4, r0
    # data in r5
    ldr r5, [r0]
    # remap reloc
    add r5, r4, r5
    # store remapped reloc data
    str r5, [r0]

    # decrement number of relocs to process
    sub r1, r1, #1
    # go to next reloc
    add r2, r2, #4
    b loop

done:
    add r4, r3, r4
    bx r4
    .align 4

relocs:
