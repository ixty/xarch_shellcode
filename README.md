# Cross Arch Shellcode Compiler

2016 - ixty

## Information
This program allows to build portable, architecture independant shellcode from C code.
It currently supports the following architectures:
- x86
- x86_64
- arm
- arm_64

It works by:
- compiling the same C code for each architecture
- linking it to arch specific syscall implementation
- using a polyglot dispatching shellcode

The final layout of the output binary is:
    [ DISPATCHER   ]
    [ X86 BLOCK    ]
    [ X86_64 BLOCK ]
    [ ARM BLOCK    ]
    [ ARM_64 BLOCK ]

The dispatcher is in stage0
Open [stage0/README](./stage0/README) for information on how it works

Each arch specific block has the following layout:

    [ LOADER    ]
    [ RELOC NUM ]
    [ RELOC 0   ]
    [ RELOC 1   ]
        ...
    [ RELOC N   ]
    [ START OFF ]
    [ CODE      ]

Open [stage1/README](./stage1/README) for information on loaders

The final payload code is the stage2.
Open [stage2/README](./stage2/README) for information on the payload


## Dependencies

- python2.7
- nasm
- gcc
- pyelftools (pip install pyelftools)
- qemu-user-static
- qemu-utils
- arm chroot with gcc
- arm64 chroot with gcc

#### Assuming you use debian:

```
# apt-get install gcc nasm python2.7 python-pip
# apt-get install qemu qemu-user-static qemu-utils binfmt-support debootstrap
# qemu-debootstrap --arch=arm64 jessie /opt/arm64/ http://ftp.debian.org/debian
# qemu-debootstrap --arch=armhf jessie /opt/armhfxx/ http://ftp.debian.org/debian

# chroot /opt/arm64
# apt-get install gcc
# exit

# chroot /opt/armhf
# apt-get install gcc
# exit
```

## Running & testing

`$ ./build.py`

If everything goes well, it creates ./ouput which is the portable multi-arch shellcode.

To test that everything works, use the provided 'sc' utility:
```
On the local x86_64 machine
user@x86_64-box   $ ./sc_86     ./output
user@x86_64-box   $ ./sc_x86_64 ./output
... And in the chroots for arm/arm64
user@armhf-chroot $ ./sc_arm    ./output
user@arm64-chroot $ ./sc_arm_64 ./output
```

## Credits
Thanks to feliam
> https://github.com/feliam/mkShellcode

> http://blog.binamuse.com/2013/01/about-shellcodes-in-c.html
 
The x86 / x86_64 loader code is taken from this project and the shellcode extraction technique is based upon his work aswell.


