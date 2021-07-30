#!/usr/bin/sisa16_asm -run



//use a separately compiled libc.
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"

//use the normal libc.
//..include"libc.hasm"

..main:
	lrx0 %/0x70000%;
	proc_krenel;
	halt;
..(7):
	lrx0 %0xc%,%0%;
	proc_gets
	la '\n'; putchar; la '\r'; putchar;
	lrx0 %0xc%,%0%;
	proc_stringlen;
	rxincr;
	rx2_0;
	//The length of the string (plus one, for the null byte) is now in RX2, the src and dest arguments must be setup.
	lrx0 %/0xd0000%
	lrx1 %/0xc0000%
	proc_memcpy;
//	la 0xd; apush; la 0; alpush;
	lrx0 %0xd%,%0%;
	proc_prints;
	la '\n'; putchar; la '\r'; putchar;
	lrx0 %/0xd0000%
	proc_atof_dec
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
	lrx0 %/0x1000%;
	proc_alloc;
	rx2_0
	la '\n'; putchar; la '\r'; putchar;
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
	rx0_2;
	//lrx1 %/256%;rxadd;
	proc_free;
	la '\n'; putchar; la '\r'; putchar;
	lrx0 %/0xE10%;
		proc_alloc;
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
	la '\n'; putchar; la '\r'; putchar;
	lrx0 %/0xBAFF00%;
			proc_alloc;
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

