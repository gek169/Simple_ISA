#!/usr/local/bin/sisa16_asm -run

..include"libc.hasm"

..(1):
	la 0xc; apush; la 0; alpush;
	proc_gets
	la 0xa; putchar; la 0xd; putchar;
	proc_strlen;
	//we now have the length of the string stored, we have to get it into RX0.
	alpop; rx2a;
	apop; rx0a;
	lrx1 %/16%; rxlsh;
	rx1_2;
	rxadd;
	rxincr;
	rx2_0;
	//The length of the string (plus one, for the null byte) is now in RX2, the src and dest arguments must be setup.
	lrx0 %/0xd0000%;
	lrx1 %/0xc0000%;
	proc_memcpy;
	la 0xd; apush; la 0; alpush;
	proc_puts;
	la 0xa; putchar; la 0xd; putchar;
	pop %3%;
	halt;

..zero:
	la 1; lfarpc;
