#!/usr/local/bin/sisa16_asm -run

VAR#ZERO_STACK_POINTER#		astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;

//global variables.
VAR#ld_iteration_count#	sc%0x3%;lb 0;farillda;
VAR#st_iteration_count#	sc%0x3%;lb 0;faristla;

section 0x40000;
VAR#ascii_greyscale#@
! .:-=+*#%@%#*+=-:.
bytes 0


ASM_header libc.hasm


VAR#line_length#120
VAR#line_length_plus_1#121
VAR#wait_time#20

section 0x10000;
asm_begin_region_restriction;
la 0;
alpush;
st_iteration_count;
nop;
nop;
VAR#asciifun_looptop#@
	//increment the counter.
		alpop;
			aincr;
			llb %line_length_plus_1%; 
			mod;
		alpush;
		sc %iter_is_endval%;llb %line_length%;cmp;jmpifeq;
		sc %iter_is_not_endval%;jmp;

	VAR#iter_is_endval#@
		la 0xa; putchar; 
		la 0xd; putchar;
		ld_iteration_count; 
			aincr;
		st_iteration_count;
		lla %wait_time%;alpush;
		proc_wait;
		alpop;
	sc %asciifun_looptop%;jmp;

	VAR#iter_is_not_endval#@
		ld_iteration_count;
		blpop;blpush;
		add;
		lb 18;mod;
		//we now have the offset calculated in register a.
		sc %0x4%; 
		llb %ascii_greyscale%;
		add;
		ba;
		farilda;
		putchar;
	sc %asciifun_looptop%;jmp;
VAR#asciifun_loopout#@
alpop;
halt;halt;halt;halt;halt;halt;halt;halt;
halt;halt;halt;halt;halt;halt;halt;halt;
halt;halt;halt;halt;halt;halt;halt;halt;
asm_end_region_restriction;


//MAIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

section 0;
la 0xa;putchar;
la 0xd;putchar;
la 1;lfarpc;
