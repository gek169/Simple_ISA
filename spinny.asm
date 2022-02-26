#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;
..(2):
..include"libc.hasm"

//..include"libc_pre.hasm"
//..(2):
//..dinclude"libc_pre.bin"
.wait_time:				33

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
	bruh:
	jmp;


..(5):
asm_begin_region_restriction;
sc %asciifun_start%; jmp;
la '\r'; putchar; 
la '\n'; interrupt;
asciifun_start:
la 0;alpush;
	//our loop!
	asciifun_looptop: 
		la ' '; putchar;putchar;putchar;
		alpop; 
			aincr; 
			lb %~ascii_spinny_len%;mod;
		alpush;
		lb %~ascii_spinny%;
		add; ba;
		cpcr;
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
