#!/usr/bin/sisa16_asm -run

..include"libc.hasm"



..main:
	lla %0xE000%; interrupt; //enable non-blocking IO.
	lla %0xE001%; interrupt; //disable non-blocking IO.
	lla %0xE000%; interrupt; //enable non-blocking IO.
//	lla %0xE001%; interrupt; //disable non-blocking IO.
	:filt_looptop:
//	lla %500%; alpush; proc_wait; alpop;
	getchar;
	sc %filt_printbytehex%
	apush;lb 0; cmp; bpop;				jmpifeq;
	ab;apush;lb 1; cmp; bpop;			jmpifeq;
	ab;apush;lb 2; cmp; bpop;			jmpifeq;
	ab;apush;lb 3; cmp; bpop;			jmpifeq;
	ab;apush;lb 4; cmp; bpop;			jmpifeq;
	ab;apush;lb 5; cmp; bpop;			jmpifeq;
	ab;apush;lb 6; cmp; bpop;			jmpifeq;
	ab;apush;lb 7; cmp; bpop;			jmpifeq;
	ab;apush;lb 8; cmp; bpop;			jmpifeq;
	ab;apush;lb 11; cmp; bpop;			jmpifeq;
	ab;apush;lb 12; cmp; bpop;			jmpifeq;
	ab;apush;lb 14; cmp; bpop;			jmpifeq;
	ab;apush;lb 15; cmp; bpop;			jmpifeq;
	ab;apush;lb 16; cmp; bpop;			jmpifeq;
	ab;apush;lb 17; cmp; bpop;			jmpifeq;
	ab;apush;lb 18; cmp; bpop;			jmpifeq;
	ab;apush;lb 19; cmp; bpop;			jmpifeq;
	ab;apush;lb 20; cmp; bpop;			jmpifeq;
	ab;apush;lb 21; cmp; bpop;			jmpifeq;
	ab;apush;lb 22; cmp; bpop;			jmpifeq;
	ab;apush;lb 22; cmp; bpop;			jmpifeq;
	ab;apush;lb 23; cmp; bpop;			jmpifeq;
	ab;apush;lb 24; cmp; bpop;			jmpifeq;
	ab;apush;lb 25; cmp; bpop;			jmpifeq;
	ab;apush;lb 26; cmp; bpop;			jmpifeq;
	ab;apush;lb 27; cmp; bpop;			jmpifeq;
	ab;apush;lb 28; cmp; bpop;			jmpifeq;
	ab;apush;lb 29; cmp; bpop;			jmpifeq;
	ab;apush;lb 30; cmp; bpop;			jmpifeq;
	ab;apush;lb 31; cmp; bpop;			jmpifeq;
//	ab;apush;lb 82; cmp; bpop;			jmpifeq;
//	ab;apush;lb 81; cmp; bpop;			jmpifeq;
	ab;apush;lb '\n'; cmp; bpop;			sc %filt_newline%;		jmpifeq;
	ab;apush;lb '\r'; cmp; bpop;			jmpifeq;
	ab;apush;lb 0xff; cmp; bpop;		sc %filt_end%;			jmpifeq;
	ab;apush;lb 126;cmp;lb2;cmp;bpop;	sc %filt_printbytehex%;	jmpifeq;
	ab;putchar;
	sc %filt_looptop%; jmp;
	:filt_newline:
	la 0xa; putchar;
	la 0xd; putchar;
	sc %filt_looptop%; jmp;
	:filt_printbytehex:
	bpush
	la 0xd; putchar
	la 0xa; putchar
	proc_printbytehex
	bpop
	sc %filt_looptop%; jmp;
	:filt_end:
	bpush
	proc_printbytehex
	bpop
	lla %0xE001%; interrupt;
	halt
