#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;
..(2):
..include"libc.hasm"

//..include"libc_pre.hasm"
//..(2):
//..dinclude"libc_pre.bin"
.wait_time:				50

..main(3):
lrx0 %/krenel_boot%;
proc_krenel;
halt;

..(4):
krenel_boot:
	lla %0xDE04%;
	lb 5;
	syscall;
	sc %bruh%;
	halt;
	bruh:
	jmp;


..(10):
asm_begin_region_restriction;
nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;

la '\r'; putchar; 
la 'q'; putchar; 
la '\n'; interrupt;
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
		add; ca;
		ilda;
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
