#!/usr/bin/sisa16_asm -run



//use a separately compiled libc.
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"

//use the normal libc.
//..include"libc.hasm"

..(5):
length_of_input_string:
bytes %/0%;

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
	farstrx0 %&length_of_input_string%;
	rx2_0;
	//The length of the string (plus one, for the null byte) is now in RX2, the src and dest arguments must be setup.
	lrx0 %/0xd0000%
	lrx1 %/0xc0000%
	proc_memcpy;
	la 0xd; apush; la 0; alpush;
	proc_puts;
	la '\n'; putchar; la '\r'; putchar;
	pop %3%;
	//We would now like to write to disk.
	farldrx0 %&length_of_input_string%;	//The number of bytes to be read.
	lrx1 %/0%							//the destination.
	lrx2 %/0xd0000%						//the source in memory.
	proc_fwrite
	lrx0 %/0xd0000%
	proc_atoi_dec
	proc_rxsqrt
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
	lrx0 %/0xF00%;
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

