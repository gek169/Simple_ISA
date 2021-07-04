#!/usr/local/bin/sisa16_asm -run

..include"libc.hasm"

..(1):
	la 0xc; apush; la 0; alpush;
	proc_gets
	la 0xa; putchar; la 0xd; putchar;
	proc_puts;
	la 0xa; putchar; la 0xd; putchar;
	proc_puts;
	la 0xa; putchar; la 0xd; putchar;
	halt;

..(0):
	la 1; lfarpc;
