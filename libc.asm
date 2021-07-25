#!/usr/bin/sisa16_asm -run
//The libc *is* actually a valid, bootable program.
//see "libc_booter" in emulation.hasm.
.LIBC_EXPORT_DCL:region 0;
..include"libc.hasm"
ASM_EXPORT_HEADER
