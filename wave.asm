#!/usr/local/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;
.POP_FARPTR_VARIABLE:	blpop;apop;ca;
.PUSH_FARPTR_VARIABLE:	ac;apush;blpush;

//global variables.
.ld_iteration_count:	sc%0x3%;lb 0;farillda;
.st_iteration_count:	sc%0x3%;lb 0;faristla;

section 0x40000;
:ascii_greyscale:
..asciz: .:-=+*#%@%#*+=-:.

..(3):
bytes 0,0,0,5;

ASM_header libc.hasm


.line_length:			79
.line_length_plus_1:	80
.wait_time:				20

section 0xB00000;
fill 153600,0x2E

//section 0xB50000;


section 0x10000;
asm_begin_region_restriction;
la line_length;
alpush;
st_iteration_count;
nop;
nop;
:asciifun_looptop:
	//increment the counter.
		alpop;
			aincr;
			llb %line_length_plus_1%; 
			mod;
		alpush;
		sc %iter_is_endval%;llb %line_length%;cmp;jmpifeq;
		sc %iter_is_not_endval%;jmp;

	:iter_is_endval:
		la 0xd; putchar;
		ld_iteration_count; 
			adecr;
		st_iteration_count;
		//If we are using the SDL driver, display the screen.
		la 0; interrupt;
		//the return value tells us if we are currently using 
		rx0a;
		//poll for the quit event.
		la 1; interrupt;
		llb %0xffFF%; cmp; sc %asciifun_loopout%; jmpifeq;
		la 3; interrupt;
		la 2;interrupt;
		lb0x10;cmp;
		sc %asciifun_loopout%; jmpifeq;
		//are we using SDL?
		:asciifun_back:
		lb1;rx1b;rxcmp;sc %asciifun_looptop%;jmpifeq;
		la %~wait_time%;
		alpush;
			proc_wait;
		alpop;
	sc %asciifun_looptop%;jmp;

	:iter_is_not_endval:
		ld_iteration_count;
		blpop;blpush;
		add;lb 18;mod;
		//we now have the offset calculated in register a.
		sc %0x4%; 
		lb %~ascii_greyscale%;
		add;
		ba;
		farilda;
		putchar;
	sc %asciifun_looptop%;jmp;
:asciifun_loopout:
halt;
asm_end_restriction;

section 0;
//fill a section of memory with something
	:L_make_tri_top:
		//what sample are we on?
		arx0;
		
		lb 8;
		and;
		lb3;
		rsh;
		llb %0x200%;
		mul;
		sc %0xB5%; 
		brx0; 
		faristla;
		rxincr;
		rxincr;
		lrx1 %/0x10000%; 
		rxcmp;
		sc %L_make_tri_top%; jmpifneq;
	la 3; interrupt;
	la 1;lfarpc;
