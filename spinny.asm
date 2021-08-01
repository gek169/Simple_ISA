#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;

//..include"libc.hasm"

..include"libc_pre.hasm"
..zero:
..dinclude"libc_pre.bin"
.wait_time:				50

..(4):
asm_begin_region_restriction;
la '\r'; putchar; 
alpush;
	//our loop!
	.ascii_spinny_len:4
	asciifun_looptop: 
		la ' '; putchar;
		alpop; 
			aincr; 
		alpush;
		
		lb ascii_spinny_len;mod; 
		sc %1%; lb %~ascii_spinny%; 
		add; ba;
		farilda;
		putchar;
		la '\r'; putchar;
		la '\n'; interrupt;
		la %~wait_time%;
		alpush; proc_wait; alpop;
		sc %asciifun_looptop%;jmp;
	:ascii_spinny:
	..asciz:-\|/
	
asm_end_restriction;
