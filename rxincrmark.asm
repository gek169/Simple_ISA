#!/usr/bin/sisa16_asm -run

..include"libc.hasm"
..main:
lrx0 %/0x10000%;
proc_krenel;
halt;

..(1):
lb0;rx1b;rx0b;
sc %looptop%;
looptop:
rxincr;
rxcmp;
jmpifneq;
halt;
