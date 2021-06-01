#!/usr/bin/sisa16_asm -run
//create a whole-memory image.
section 0xFFffFF;bytes 0,0;

VAR#ZERO_STACK_POINTER#		astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;

//global variables.
VAR#ld_iteration_count#	farllda %&0x440000%;
VAR#st_iteration_count#	farstla %&0x440000%;



section 0x20000;asm_begin_region_restriction;
VAR#proc_printbytehex#sc 0,0;la 2;farcall; 
//retrieve our argument.
astp;lb4;sub;ca;ilda;
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

//we now have our far pointer! put it on the top of the stack for easy access.
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


//Variable Section.
section 0xAE0000;
VAR#ld_iter#farldrx0 %&0xAE0000%;
VAR#st_iter#farstrx0 %&0xAE0000%;
bytes %/0%;

section 0xAF0000;
!Your patience is exemplary.
bytes 0;
VAR#seconds_strings#@
bytes 0xd, 0xa;
!Seconds:
bytes 0;
VAR#iterations_strings#@
! Iterations:
bytes 0;
VAR#STP_STRING#@
! Stack Pointer:
bytes 0;

section 0x10000;
//rx2 holds our time in seconds.
clock;rx2b;
VAR#main_looptop#@
//iter++;
	ld_iter;rxincr;st_iter;
	clock;
	arx2;
	sc %main_looptop%;cmp;jmpifeq;
//the time elapsed is different! it is still in b.
	rx2b;
//print our string that says <newline>Secs:
	la 0xAF;apush;lla %seconds_strings%;alpush;
		proc_puts;
	alpop;apop;

	arx2;apush;
	lb8;rsh;apush;
//print the number of seconds.
	proc_printbytehex;
	apop;
	proc_printbytehex;
	apop;
//print the iterations string.
la 0xAF;apush;lla %iterations_strings%;alpush;
		proc_puts;
	alpop;apop;
//print the number of iterations.
ld_iter;arx0;apush;
		lrx1 %/8%;rxrsh;arx0;apush;
ld_iter;lrx1 %/16%;rxrsh;arx0;apush;
ld_iter;lrx1 %/24%;rxrsh;arx0;apush;
//ld_iter;rx0push;rx0pop;rx0push;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;
//Reset iteration counter.
	lrx0 %/0%;
	st_iter;
//exit the loop if we have have passed the limit.
	sc %main_loopout%;
	arx2;llb %0x0FFF%;cmp;lb2;cmp;jmpifeq
	
//print the stack pointer.
	la 0xAF;apush;lla %STP_STRING%;alpush;
	proc_puts;alpop;apop;
	astp;apush;proc_printbytehex;apop;
	sc %main_looptop%;jmp;
VAR#main_loopout#@
la 0xd;putchar;la 0xa;putchar;
la 0xAF;apush;lla %0%;alpush;
	proc_puts;
alpop;apop;
halt;



//bootloader
section 0;la 1;lfarpc;asm_print;
