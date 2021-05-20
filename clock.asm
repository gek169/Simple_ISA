#!/usr/bin/sisa16_asm -run

VAR#ZERO_STACK_POINTER#		astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;

//global variables.
VAR#ld_iteration_count#	sc%0x44%;lb 0;farillda;
VAR#st_iteration_count#	sc%0x44%;lb 0;faristla;



section 0x20000;asm_begin_region_restriction;
VAR#proc_printbytehex#sc 0,0;la 2;farcall; 
//retrieve our argument.
astp;lb4;sub;ca;ilda;
apush;
lb 4;rsh;
lb0xf;and;
lb 7;mul;
llb%proc_printbytehex_jmptable_1%;add;ca;jmp;
VAR#proc_printbytehex_jmptable_1#@
la0x30;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x31;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x32;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x33;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x34;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x35;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x36;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x37;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x38;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x39;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x41;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x42;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x43;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x44;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x45;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
la0x46;putchar;sc%proc_printbytehex_jmptable_1_end%;jmp
VAR#proc_printbytehex_jmptable_1_end#@
apop;lb 15;and;lb7;mul;
llb%proc_printbytehex_jmptable_2%;add;ca;jmp;
VAR#proc_printbytehex_jmptable_2#@
la0x30;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x31;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x32;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x33;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x34;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x35;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x36;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x37;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x38;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x39;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x41;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x42;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x43;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x44;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x45;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
la0x46;putchar;sc%proc_printbytehex_jmptable_2_end%;jmp
VAR#proc_printbytehex_jmptable_2_end#@
farret;


VAR#proc_puts#sc%@%;la2;farcall;
//move the stack pointer back.
astp;lb6;sub;
illdaa;lb8;rsh;lb255;and;ca;
//load the thing thats there. But we also loaded the high byte of the
//next short, so we need to get rid of that. Goes in C
astp;lb5;sub;illdaa;//we need to grab that short.

//we now have our far pointer! put it on the top of the stack for easy access.
bc;bpush;alpush;
VAR#puts_looptop#@
POP_FARPTR_VARIABLE;
PUSH_FARPTR_VARIABLE;
farilda;
putchar;
lb0;cmp;sc%puts_loopend%;jmpifeq;
POP_FARPTR_VARIABLE;
ab;lb1;add;ba;
PUSH_FARPTR_VARIABLE;
sc %puts_looptop%;jmp;
VAR#puts_loopend#@
POP_FARPTR_VARIABLE;
farret;


asm_end_region_restriction;

section 0xAE0000;
VAR#ld_iter#sc %0xAE%;lla %0%;farildrx0;
VAR#st_iter#sc %0xAE%;lla %0%;faristrx0;
bytes %/0%;

section 0xAF0000;
!Your patience is exemplary.
bytes 0;
VAR#seconds_strings#@
bytes 0xd, 0xa;
!Secs:
bytes 0;
VAR#iterations_strings#@
!  Iterations:
bytes 0;
VAR#STP_STRING#@
! Stack Pointer:
bytes 0;

section 0x10000;
//rx2 holds our time in seconds.
clock;rx2b;
VAR#main_looptop#@
//iter++;
	ld_iter;lrx1 %/1%;rxadd;st_iter;
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
	arx2;
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
