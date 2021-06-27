#!/usr/local/bin/sisa16_asm -run
//create a whole-memory image.

.ZERO_STACK_POINTER:		astp;popa;
.POP_FARPTR_VARIABLE:	blpop;apop;ca;
.PUSH_FARPTR_VARIABLE:	ac;apush;blpush;

//global variables.
.ld_iteration_count:	farllda %&0x440000%;
.st_iteration_count:	farstla %&0x440000%;



ASM_header libc.hasm


//Variable Section.
section 0xAE0000;
.ld_iter:farldrx0 %&0xAE0000%;
.st_iter:farstrx0 %&0xAE0000%;
bytes %/0%;

section 0xAF0000;
!Your patience is exemplary.
bytes 0;
:seconds_strings:
bytes 0xd, 0xa;
!Seconds:
bytes 0;
:iterations_strings:
! Iterations:
bytes 0;
:STP_STRING:
! Stack Pointer:
bytes 0;

section 0x10000;
//rx2 holds our time in seconds.
clock;rx2b;
:main_looptop:
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
:main_loopout:
la 0xd;putchar;la 0xa;putchar;
la 0xAF;apush;lla %0%;alpush;
	proc_puts;
alpop;apop;
halt;



//bootloader
section 0;la 1;lfarpc;
