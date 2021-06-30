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
		la 0xa; putchar;
		la 0xd; putchar;
		ld_iteration_count; 
			aincr;
		st_iteration_count;
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
asm_end_restriction;

section 0;
	la 1;lfarpc;
