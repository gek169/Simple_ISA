#!/usr/bin/sisa16_asm -run
//create a whole-memory image.

section 0;
la 1; lfarpc;


section 0x20000;asm_begin_region_restriction;
VAR#proc_printbytehex#sc 0,0;la 2;farcall; 
//retrieve our argument.
	astp;lb4;sub;ca;ilda;
//push it
	apush;
	lb 4;rsh;
	lb0xf;and;
	lb 7;mul;
llb %printbytehex_jmptable_1%; add;ca;jmp;
VAR#printbytehex_jmptable_1#@
	la0x30;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x31;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x32;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x33;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x34;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x35;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x36;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x37;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x38;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x39;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x41;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x42;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x43;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x44;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x45;putchar;sc%printbytehex_jmptable_1_end%;jmp
	la0x46;putchar;sc%printbytehex_jmptable_1_end%;jmp
VAR#printbytehex_jmptable_1_end#@
apop;lb 15;and;lb7;mul;
llb%printbytehex_jmptable_2%;add;ca;jmp;
VAR#printbytehex_jmptable_2#@
	la0x30;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x31;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x32;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x33;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x34;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x35;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x36;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x37;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x38;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x39;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x41;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x42;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x43;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x44;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x45;putchar;sc%printbytehex_jmptable_2_end%;jmp
	la0x46;putchar;sc%printbytehex_jmptable_2_end%;jmp
VAR#printbytehex_jmptable_2_end#@
farret;


VAR#proc_puts#sc%@%;la2;farcall;
	//move the stack pointer back.
		astp;lb6;sub;
		illdaa;lb8;rsh;lb255;and;ca;
	//load the thing thats there. But we also loaded the high byte of the
	//next short, so we need to get rid of that. Goes in C
		astp;lb5;sub;illdaa;//we need to grab that short.

	//we now have our far pointer! put it on the top of the stack
	//we actually push 4 bytes, so we can rx0pop.
		lb0;bpush;bc;bpush;alpush;
	VAR#puts_looptop#@
		//setup far pointer.
		rx0pop;		rx0push;	cbrx0;
		//load through far pointer and print
		farilda;		putchar;
		//if the character was zero, jump to the end of the loop
		lb0;cmp;sc%puts_loopend%;jmpifeq;
		//else, increment
		rx0pop;rxincr;rx0push;
	sc %puts_looptop%;jmp;
	VAR#puts_loopend#@
		blpop;blpop;
farret;
asm_end_region_restriction;

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

section 0x10000;asm_begin_region_restriction;
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


la4;apush;lla%0%;alpush;
proc_puts;
halt;
asm_end_region_restriction;
//|
