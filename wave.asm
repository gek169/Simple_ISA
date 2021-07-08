#!/usr/local/bin/sisa16_asm -run

.ZERO_STACK_POINTER:		astp;popa;
.POP_FARPTR_VARIABLE:		blpop;apop;ca;
.PUSH_FARPTR_VARIABLE:		ac;apush;blpush;

//global variables.
.ld_iteration_count:	farllda %&0x30000%;
.st_iteration_count:	farstla %&0x30000%;

section 0x40000;
:ascii_greyscale:
..asciz: .:-=+*#%@%#*+=-:.


.ITER_REGION:3
..(ITER_REGION):
bytes 0,0,0,0;

..include"libc.hasm"


.line_length:			90
.line_length_plus_1:	91
.wait_time:				16

//section 0xB00000;
//fill 153600,0x2E

section 0x10000;
asm_begin_region_restriction;
la line_length;
alpush;
asm_print @&
st_iteration_count;
nop;
nop;
asciifun_looptop: //Comment.
	//increment the counter.
		alpop;
			aincr;
			llb %line_length_plus_1%; 
			mod;
		alpush;
		sc %iter_is_endval%;llb %line_length%;cmp;jmpifeq;
		sc %iter_is_not_endval%;jmp;

	iter_is_endval:
		la '\r';putchar;
		ld_iteration_count; 
			adecr;
		st_iteration_count;
		//If we are using the SDL driver, display the screen.
			//la 0; interrupt;
			//the return value tells us if we are currently using SDL.
			//rx0a;
			//poll for the quit event.
			//la 1; interrupt;
			//llb %0xffFF%; cmp; sc %asciifun_loopout%; jmpifeq;
			//la 3; interrupt;
			//la 2;interrupt;
			//lb0x10;cmp;
			//sc %asciifun_loopout%; jmpifeq;
			//are we using SDL?
			//lb1;
			//rx1b;
			//rxcmp;
			//sc %asciifun_looptop%;
			//jmpifeq;
		la %~wait_time%;
		alpush;
			proc_wait;
		alpop;
		proc_clear_terminal;
	sc %asciifun_looptop%;jmp;

	iter_is_not_endval:
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
asciifun_loopout:
la 7; putchar;
halt;
asm_end_restriction;

..zero:
	//used for testing the SDL2 driver.
	//You can just delete this.
	//L_make_tri_top:
		//what sample are we on?
		//arx0;
		//do some computation.


		//triangle wave.
		//lb1;rsh;
		//llb %0x4%;
		//mod;
		//lb 0xF0;
		//mul;

		
		//square wave.
		//lb 8;
		//and;
		//lb3;
		//rsh;
		//llb %0x200%;
		//mul;
		//sc %0xB5%;brx0;faristla;
		//rxincr;rxincr;lrx1 %/0x10000%; rxcmp;sc %L_make_tri_top%; jmpifneq;
	//	la 3; interrupt;
	la 1;lfarpc;
