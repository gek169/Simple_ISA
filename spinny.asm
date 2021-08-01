#!/usr/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;


section 0x40000;


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
..main(3):
	lrx0 0, %~LIBC_REGION%, %libc_COMMAND_COM%;
	proc_krenel;
	halt;
..(4):
asm_begin_region_restriction;
la '\r'; putchar; 
la 0;
st_iter;
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
