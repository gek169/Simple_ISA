#!/usr/bin/sisa16_asm -run
//create a whole-memory image.

.ZERO_STACK_POINTER:		astp;popa;
.POP_FARPTR_VARIABLE:	blpop;apop;ca;
.PUSH_FARPTR_VARIABLE:	ac;apush;blpush;



..include"libc.hasm"
//..include"libc_pre.hasm"
//..(2):
//..dinclude"libc_pre.bin"

.BENCH_SECONDS:256

//Variable Section.
section 0xAE0000;
.ld_iter:farldrx0 %&0xAE0000%;
.st_iter:farstrx0 %&0xAE0000%;
bytes %/0%;
.ld_secs:farllda %&0xAE0004%;
.st_secs:farstla %&0xAE0004%;
.st_secs_b:farstlb %&0xAE0004%;
bytes %0%;

section 0xAF0000;
..asciz:~~~~~Bench finished.~~~~~~~~
:seconds_strings:
bytes 0xd, 0xa;
..asciz: Seconds:
:iterations_strings:
..asciz: Iterations:
:STP_STRING:
..asciz: Stack Pointer:
:LENGTH_OF_STRING_STRING:
..asciz: length of this string:

//main routine.

//rx2 holds our time in seconds.
..main:
	//containerize execution of the clock.
	lrx0 %/Lbl_clock_start%;
	lb 0xEEEE;
	proc_emulate_seg;
	la 0xAF;apush;la 0;alpush;
		proc_puts;
	pop %3%;
	halt;

	
	:Lbl_clock_start:
	clock;
	st_secs_b;
	rx3b;
	:main_looptop:
		//during iteration counting, use
		rxincr;
		clock;
		arx3;
		//jump to the top if the number of seconds elapsed is the same.
		sc %main_looptop%;cmp;jmpifeq;

	rx2_0;
	st_iter;
	//the time elapsed is different! it is still in b.
	st_secs_b
//print seconds string.
	la 0xAF;apush;lla %seconds_strings%;alpush;
		proc_puts;
	pop %3%;
	ld_secs
	apush
	//testing 8 bit split syntax
	lb %~8%;
	rsh;
	apush;
	//print the number of seconds.
	proc_printbytehex;
	apop;
	proc_printbytehex;
	apop;
	//print the iterations string.
	la 0xAF;apush;lla %iterations_strings%;alpush;
		proc_puts;
	pop %3%;
	//print the number of iterations.
	ld_iter;arx0;apush;
			lb8; rx1b;rxrsh;arx0;apush;
	ld_iter;lb16;rx1b;rxrsh;arx0;apush;
	ld_iter;lb24;rx1b;rxrsh;arx0;apush;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;
	proc_printbytehex;
		apop;

	
//print the stack pointer.
	la 0xAF;apush;lla %STP_STRING%;alpush;
	proc_puts;
	pop %3%;
	astp;apush;proc_printbytehex;apop;
	//print the length of string string.
	//print the length of that very same string.
	la 0xAF;apush;lla %LENGTH_OF_STRING_STRING%;alpush;
	proc_puts;
	proc_strlen;
	proc_printbytehex;
	pop %3%;
		//Reset iteration counter.
	la0
	rx0a
	st_iter
	//exit the loop if we have have passed the limit.
	sc %main_loopout%;
	ld_secs;rx3a;llb %BENCH_SECONDS%;cmp;lb0;cmp;jmpifneq;
	la '\n'; interrupt;
	sc %main_looptop%;jmp;
:main_loopout:
lb0;mod;
halt;



//bootloader
..zero:
la 1;lfarpc;
