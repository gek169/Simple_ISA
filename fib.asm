#!/usr/bin/sisa16_asm -run



//use a separately compiled libc.
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"

//use the normal libc.
//..include"libc.hasm"

..(3):
..decl_farproc:proc_fib
	//if it is less than two...
	lb2; rx1b;
	rxcmp;
	lb 0; cmp;
	sc %fib_return_one%; jmpifeq;
	rx0push;
	rxdecr;
	proc_fib;
	rx1pop;
	rx0push;
	rx0_1;rxdecr;rxdecr;
	proc_fib;
	rx1pop;
	rxadd;
	farret;
	fib_return_one:
		lb1; 
		rx0b;
		farret

..main(1):
	la 0xc; apush; la 0; alpush;
	proc_gets
	la '\n'; putchar; la '\r'; putchar;
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
	lrx0 %/0xd0000%
	lrx1 %/0xc0000%
	proc_memcpy;
	la 0xd; apush; la 0; alpush;
	proc_puts;
	la '\n'; putchar; la '\r'; putchar;
	pop %3%;
	lrx0 %/0xd0000%
	proc_atoi_dec
	proc_fib
	rx2_0
		//print the number!
	rx0_2;arx0;apush;
	lb8; rx1b;rxrsh;arx0;apush;
	rx0_2;lb16;rx1b;rxrsh;arx0;apush;
	rx0_2;lb24;rx1b;rxrsh;arx0;apush;
	proc_printbytelchex;
		apop;
	proc_printbytelchex;
		apop;
	proc_printbytelchex;
		apop;
	proc_printbytelchex;
		apop;
	la '\n'; putchar;
	la '\r'; putchar;
	halt;

