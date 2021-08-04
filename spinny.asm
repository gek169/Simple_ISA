#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;

//..include"libc.hasm"

..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"
.wait_time:				50

..main(3):

lrx0 %/krenel_boot%;
proc_krenel;
halt;

..(4):
asm_begin_region_restriction;
krenel_boot:
la '\r'; putchar; 
alpush;
	//our loop!
	
	asciifun_looptop: 
		la ' '; putchar;
		alpop; 
			aincr; 
		alpush;
		
		lb %~ascii_spinny_len%;mod; 
		cpcr;
		 lb %~ascii_spinny%; 
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
	.ascii_spinny_len:4
asm_end_restriction;
