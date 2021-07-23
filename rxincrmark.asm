#!/usr/bin/sisa16_asm -run

..include"libc.hasm"
..main:
lrx0 %/0x70000%;
proc_krenel;
la 'd'; putchar;
halt;

..(7):
lb0;rx1b;rx0b;
sc %looptop%;
looptop:
rxincr;
rxcmp;
jmpifneq;
halt;
