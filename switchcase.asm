#!/usr/local/bin/sisa16_asm -run
..zero:
	sc %10%; jmp;
section 10;
	getchar;
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
halt;
