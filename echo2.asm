#!/usr/bin/sisa16_asm -run
//create a whole-memory image.


ASM_header libc.hasm

section 0x30000;
VAR#loc_iter#@
VAR#ld_iter#farldrx0 %&loc_iter%;
VAR#st_iter#farstrx0 %&loc_iter%;
bytes %/0%;
//storage for the current character.
VAR#loc_current_char#@
VAR#ld_current_char#farllda %&loc_current_char%;
VAR#st_current_char#farstla %&loc_current_char%;
bytes 0,0;

section 0x40000;
bytes %/0%;
bytes %/0%;
bytes %/0%;
bytes %/0%;
bytes %/0%;

..main(1): asm_begin_region_restriction;
lrx0 %/0%;st_iter;
la0;st_current_char;
VAR#echo_looptop#@
	getchar;st_current_char;
	sc %echo_loopend%;
		//disabled: terminate on ascii pipe.
		//ld_current_char;	lb 124;cmp;jmpifeq;
		//terminate on null:
		ld_current_char;	lb 0;cmp;jmpifeq;
		//terminate on non-ascii:
		ld_current_char;	lb126;cmp;lb2;cmp;jmpifeq;

	sc %after_newline_processing%;
	//Do not process characters greater than the 0xd character or less than the 0xa character.
		ld_current_char;	lb 0xd;cmp;lb2;cmp;jmpifeq;
		ld_current_char;	lb 0xa;cmp;lb0;cmp;jmpifeq;
	//Do not process 0xc or 0xb.
		ld_current_char;	lb 0xc;cmp;jmpifeq;
		ld_current_char;	lb 0xb;cmp;jmpifeq;
	//the character is now either 0xa or 0xd.
		la 0xa;st_current_char;
		ld_iter;lrx1 %/0x40000%;rxadd;
		cbrx0;
		farista;
	//iter++
		ld_iter;rxincr;st_iter;
	//this character is now 0xd.
		la 0xd;st_current_char;
VAR#after_newline_processing#@
	ld_current_char;
	ld_iter;lrx1 %/0x40000%;rxadd;
	cbrx0;
	farista;
	//iter++
	ld_iter;rxincr;st_iter;
sc %echo_looptop%;jmp;


VAR#echo_loopend#@

//write a null byte.
ld_iter;lrx1 %/0x40000%;rxadd;
cbrx0;la0;
farista;


la4;apush;
lla%0%;alpush;
lrx0 %/0x40000%;
proc_prints;
halt;
asm_end_region_restriction;
//|
