#!/usr/bin/sisa16_asm -run

..include"libc.hasm"

..main(1):
	lrx0 %/0xc0000%
	proc_gets_noecho
	la 0xa; putchar; la 0xd; putchar;
	lrx0 %/0xc0000%
	proc_stringlen;
	//we now have the length of the string in RX0.
	rxincr;
	rx2_0;
	//The length of the string (plus one, for the null byte) is now in RX2, the src and dest arguments must be setup.
	lrx0 %/0xd0000%;
	lrx1 %/0xc0000%;
	proc_memcpy;
	lrx0 %/0xd0000%
	proc_prints;
	la 0xa; putchar; la 0xd; putchar;
	halt;

