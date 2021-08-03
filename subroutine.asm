#!/usr/bin/sisa16_asm -run

..zero:
	pop %1%;
	sc %L_main%; jmp;

section 1000;
.proc_printbytehex:sc %1000%;call;
	//retrieve our argument.
	pop %2%; apop; push %3%;
	//push it
	apush;
	lb 4;rsh;
	lb0xf;and;
	lb 7;mul;
	llb %printbytehex_jmptable_1%; add;ca;jmp;
	:printbytehex_jmptable_1:
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
	:printbytehex_jmptable_1_end:
	apop;lb 15;and;lb7;mul;
	llb%printbytehex_jmptable_2%;add;ca;jmp;
	:printbytehex_jmptable_2:
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
	:printbytehex_jmptable_2_end:
ret;


section 500;
	:L_main:
		getchar;putchar;
		lb 0x30;sub;
		apush;
		la 0xa; putchar; la 0xd; putchar;interrupt;
		getchar;putchar;
		lb 0x30;sub;
		bpop;
		mul;
		apush;
		la 0xa; putchar; la 0xd; putchar;interrupt
		proc_printbytehex;
		la 0xa; putchar; la 0xd; putchar;
	halt;
