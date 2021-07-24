#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;


section 0x40000;
	:ascii_spinny:
	..asciz:-\|/
	.ascii_spinny_len:4

.ITER_REGION:3
..(ITER_REGION):
	bytes 0,0,0,0;
.ld_iter: farllda %&@&%
.ld_iterb: farlldb %&@&%
.st_iter: farstla %&@&%

//..include"libc.hasm"
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"


.line_length:			90
.line_length_plus_1:	91
.num_lines:				20
.wait_time:				50



..main(5):
	lrx0 %/0x70000%;
	proc_krenel;
	halt;
..(7):
asm_begin_region_restriction;
la '\r'; putchar; 
la 0;
st_iter;
alpush;
	//our loop!
	asciifun_looptop: 
		la ' '; putchar;
		alpop; 
			aincr; 
		alpush;
		lb ascii_spinny_len;mod; 
		sc %4%; lb %~ascii_spinny%; 
		add; ba;
		farilda;
		putchar;
		la '\r'; putchar;
		la '\n'; interrupt;
		la %~wait_time%;
		alpush; proc_wait; alpop;
		sc %asciifun_looptop%;jmp;
	asciifun_loopout:
	la 7; putchar;
	halt;



asm_end_restriction;
