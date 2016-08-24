#!/usr/bin/python
# -*- coding: utf-8 -*-

# 2016 - ixty
#
# this python program will build all stages of our code and manually
# link everything together to produce a single binary blob
# this blob can be ran on linux on either x86 x86_64 arm arm_64
# the final payload is written in C in stage2

import os, sys, struct, subprocess
from elftools.elf.elffile import ELFFile

# gcc compilation options that are common for all architectures
# basically, we want an executable with no BS and relocation information
common_opts = ' -nostdlib                           \
                -fno-builtin                        \
                -fno-common                         \
                -fno-stack-protector                \
                -fomit-frame-pointer                \
                -fno-exceptions                     \
                -fno-asynchronous-unwind-tables     \
                -fno-unwind-tables                  \
                -s                                  \
                -pie                                \
                -I./hdr '

# list of files compiled into our portable polyglot
# src/syscall_(ARCH)_.c is automatically added to that list
common_files = [
    'stage2/main.c',
    # 'stage2/snprintf.c',
    # 'stage2/utils.c',
]

# supported architectures
# with:
#   enabled/disabled
#   specific compilation flags
#   command to build loader
#   chroot to cross compile ( https://wiki.debian.org/Arm64Qemu )
#   extra gcc options at the end of the cmd
archs   = {
    'x86': {
        'enabled':  1,
        'opts':     '-m32',
        'as':       [ 'as', '-32',  '-o', 'stage1_bins/x86.o',      'stage1/x86.s' ],
        'chroot':   None,
        'extra':    ''
    },
    'x86_64': {
        'enabled':  1,
        'opts':     '-mcmodel=large',
        'as':       [ 'as', '-64',  '-o', 'stage1_bins/x86_64.o',   'stage1/x86_64.s' ],
        'chroot':   None,
        'extra':    ''
    },
    'arm': {
        'enabled':  1,
        'opts':     '-mword-relocations',
        'as':       [ 'as',         '-o', 'stage1_bins/arm.o',      'stage1/arm.s' ],
        'chroot':   '/opt/armhf/',
        'extra':    ' -lgcc'
    },
    'arm_64': {
        'enabled':  1,
        'opts':     '-mcmodel=large',
        'as':       [ 'as',         '-o', 'stage1_bins/arm_64.o',   'stage1/arm_64.s' ],
        'chroot':   '/opt/arm64/',
        'extra':    ''
    }
}

# globals to store bytecode, elfs, data, ...
loaders     = {}    # stage1
bins        = {}    # stage2
payloads    = {}    # stage1 + stage2
todelete    = []    # list of directories to delete at exit

# cleanup files
def make_clean():
    print '> cleaning up'
    os.system('rm -rf output stage0_bins stage1_bins stage2_bins')

# assemble loaders & get their bytecode
def make_loaders():
    global todelete
    for a in sorted(archs.keys()):
        if not archs[a]['enabled']:
            continue
        if not archs[a]['chroot']:
            err, out = exec_cmd(archs[a]['as'])
            if err:
                fail('> [%-6s] error assembling loader' % a)
        else:
            path = archs[a]['chroot'] + '/tmp/cc/'
            os.system('mkdir -p %s' % path)
            os.system('cp -r ./* %s' % path)
            todelete += [path]

            if exec_chroot(archs[a]['chroot'], 'cd /tmp/cc && %s' % ' '.join(archs[a]['as'])):
                fail('> [%-6s] error assembling loader' % a)
            os.system('cp %sstage1_bins/%s.o ./stage1_bins/' % (path, a))

        loaders[a] = ELFFile(file('stage1_bins/%s.o' % a)).get_section_by_name('.text').data()
        print '> [%-6s] loader size %d bytes' % (a, len(loaders[a]))

# build elf payload
def make_elfs():
    for a in sorted(archs.keys()):
        if not archs[a]['enabled']:
            continue

        gcc_cmd = 'gcc ' + common_opts + archs[a]['opts'] + ' -o stage2_bins/%s ' % a + ' '.join(common_files) + ' stage2/syscall_%s.c' % a + archs[a]['extra']
        gcc_cmd = clean_spaces(gcc_cmd)
        print '> [%-6s] %s' % (a, gcc_cmd)

        if not archs[a]['chroot']:
            err, out = exec_cmd(gcc_cmd.split(' '))
            if err:
                print out
                fail('> [%-6s] error compiling elf' % a)
        else:
            path = archs[a]['chroot'] + '/tmp/cc/'
            # folder is already copied for loaders building

            if exec_chroot(archs[a]['chroot'], 'cd /tmp/cc && %s' % gcc_cmd):
                fail('> [%-6s] error compiling elf' % a)
            os.system('cp %sstage2_bins/%s ./stage2_bins/' % (path, a))

# for each arch, merge loader, relocs & code into a standalone arch specific 'payload'
def make_payloads():
    for a in sorted(archs.keys()):
        if not archs[a]['enabled']:
            continue
        get_payload(a)
        payloads[a] = loaders[a] + bins[a]
        with open('stage0_bins/%s' % a, 'wb+') as f:
            f.write(payloads[a])

# this function loads an ELF file,
# extracts code, data & relocs from it
# remaps sections (to prevent empty zero-pad spaces)
# concatenates relocation offsets, start address & elf code/data
def get_payload(a):
    print '> [%-6s] loading elf file' % a
    elf     = ELFFile(file('stage2_bins/%s' % a))
    secs    = []
    relocs  = []
    vaddr_min = -1
    vaddr_remap = 0
    bytebuf = ''

    # parse elf sections
    for s in elf.iter_sections():
        if s.name in [ '.text', '.data', '.bss', '.rodata' ]:
            # add interesting sections to our list
            secs += [{
                'addr': s.header.sh_addr,
                'name': s.name,
                'data': s.data(),
                'size': len(s.data()),
                'remap': vaddr_remap
            }]
            # get min section vaddr
            if vaddr_min < 0 or s.header.sh_addr < vaddr_min:
                vaddr_min = s.header.sh_addr
            # next section remap
            vaddr_remap += len(s.data())

        # get relocation info
        if 'iter_relocations' in dir(s):
            for r in s.iter_relocations():
                # arm 64 relocs handle differently
                if r['r_info_type'] == 1027:
                    relocs += [ (r['r_offset'], r['r_addend']) ]
                else:
                    relocs += [ (r['r_offset'], 0) ]

    # list selected sections
    for s in secs:
        print '    [%-8s] addr 0x%x size 0x%x remapping to 0x%x' % (s['name'], s['addr'], s['size'], s['remap'])

    # patch relocs to our new mapping & add relocs to bytebuff
    bytebuf += pack_word(elf.elfclass, len(relocs))
    for (r, addend) in relocs:
        addr    = get_word(secs, r, elf.elfclass)   # address pointed to by reloc
        if not addr and addend:                     # only happens in arm_64 for now
            addr = addend
        sec     = get_section(secs, addr)           # section pointed to by reloc
        naddr   = remap_addr(secs, addr)            # new address after our remapping
        rr      = remap_addr(secs, r)               # new reloc address after our remapping
        put_word(secs, r, elf.elfclass, naddr)
        # print 'reloc @ 0x%x (0x%x) in %s 0x%x (0x%x)' % (r, rr, sec['name'], addr, naddr)
        bytebuf += pack_word(elf.elfclass, rr)

    # add entry point
    print '    > entry point 0x%x (0x%x)' % (elf.header.e_entry, remap_addr(secs, elf.header.e_entry))
    bytebuf += pack_word(elf.elfclass, remap_addr(secs, elf.header.e_entry))

    # add sections data
    for s in secs:
        bytebuf += s['data']

    bins[a] = bytebuf

# make the final binary with the differents archs payloads
def make_final():
    asm = ''
    with open('stage0/init.asm', 'rb') as f:
        asm = f.read()

    for a in sorted(archs.keys()):
        if archs[a]['enabled']:
            pay, buf = payloads[a], 'db '
        else:
            pay, buf = '', ''

        for i in range(len(pay)):
            buf += '0x%.2x' % ord(pay[i])
            if i != len(pay) - 1:
                buf += ', '

        asm = asm.replace('__payload_%s__' % a, buf)

    with open('stage0_bins/final.asm', 'wb+') as f:
        f.write(asm)

    print '> assembling final binary'
    err, out = exec_cmd(['nasm', '-o', 'stage0_bins/final', 'stage0_bins/final.asm'])
    if err:
        print out
        fail('> error assembling final binary')

    os.system('cp stage0_bins/final ./output')
    print '> saving final binary to ./output'
    print '> test with:'
    print '   ./tools/sc_x86    ./output'
    print '   ./tools/sc_x86_64 ./output'
    print '   ./tools/sc_arm    ./output'
    print '   ./tools/sc_arm_64 ./output'

# get word size & int packing format based on arch class
def arch_fmt(cls):
    if cls == 32:
        size = 4
        fmt  = '<L'
    elif cls == 64:
        size = 8
        fmt  = '<Q'
    else:
        print '> unknown elf class'
        sys.exit(1)
    return (size, fmt)

# find a section that contains specified vaddr
def get_section(secs, addr):
    for s in secs:
        if addr >= s['addr'] and addr < s['addr'] + s['size']:
            return s
    return None

# read word from section at specified vaddr
def get_word(secs, addr, cls):
    size, fmt = arch_fmt(cls)
    b = get_bytes(secs, addr, size)
    return struct.unpack(fmt, b)[0]

# write word from section at specified vaddr
def put_word(secs, addr, cls, word):
    size, fmt = arch_fmt(cls)
    s = get_section(secs, addr)
    s['data'] = s['data'][0: addr - s['addr']] + struct.pack(fmt, word) + s['data'][addr - s['addr'] + size : ]

# pack an int for specific arch
def pack_word(cls, word):
    size, fmt = arch_fmt(cls)
    return struct.pack(fmt, word)

# get bytes from vaddr
def get_bytes(secs, addr, size):
    for s in secs:
        if addr >= s['addr'] and addr < s['addr'] + s['size']:
            return s['data'][addr - s['addr'] : addr - s['addr'] + size]
    return None

# calculate the new vaddr after our section packing
def remap_addr(secs, addr):
    s = get_section(secs, addr)
    return addr + s['remap'] - s['addr']

# abort
def fail(msg):
    print msg
    sys.exit(-1)

# utility to launch a cmd & get its output
def exec_cmd(cmd, env=None, cwd=None):
    try:
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd, env=env)
        p.wait()
        return (p.returncode, p.stdout.read() + p.stderr.read())
    except:
        return (-1, '')

# utility to launch a cmd in a chroot
def exec_chroot(chroot, cmd, input=None):
    if input is not None:
        if subprocess.call('echo "%s" | chroot %s /bin/sh -c \'%s\'' % (input, chroot, cmd), shell=True):
            return -1
        return 0
    else:
        if subprocess.call('chroot %s /bin/sh -c \'%s\'' % (chroot, cmd), shell=True):
            return -1
        return 0

# remove extra spaces
def clean_spaces(s):
    while s.find('  ') >= 0:
        s = s.replace('  ', ' ')
    return s

# main
if __name__ == '__main__':
    arg = '' if len(sys.argv) == 1 else sys.argv[1]

    if arg == 'clean':
        make_clean()
    else:
        os.system('mkdir -p stage0_bins')
        os.system('mkdir -p stage1_bins')
        os.system('mkdir -p stage2_bins')
        make_loaders()
        make_elfs()
        make_payloads()
        make_final()
        # delete temp files in chroots
        os.system('rm -rf %s' % ' '.join(todelete))

