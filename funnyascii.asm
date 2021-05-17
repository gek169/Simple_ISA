#!/usr/bin/sisa16_asm -run

VAR#ZERO_STACK_POINTER#		astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;

//global variables.
VAR#ld_iteration_count#	sc%0x44%;lb 0;farillda;
VAR#st_iteration_count#	sc%0x44%;lb 0;faristla;

section 0xAF0000;
!Did you enjoy that print?
bytes 0;


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



section 0x10000;
VAR#proc_asciifun#sc 0,0;la 1;farcall;
la 0;alpush;
VAR#asciifun_looptop#@
	proc_printbytehex;
//print spaces.
	la 0x20;putchar;
//print new lines occasionally.
	alpop;alpush;rx0a;arx0;
	lb 0xf;and;
	cmp;sc%asciifun_skipnewline%;jmpifneq;
		la 0xA;putchar;la 0xD;putchar;
		clock;alpush;proc_printbytehex;alpop;
		la 0xA;putchar;la 0xD;putchar;
		clock;blpush;proc_printbytehex;blpop;
		la 0xA;putchar;la 0xD;putchar;
		//astp;alpush;proc_printbytehex;la0x20;putchar;apop;proc_printbytehex;apop;
		//la 0xA;putchar;la 0xD;putchar;
	VAR#asciifun_skipnewline#@
	//i++
	alpop;lb1;add;alpush;
	//check to see if we have reached or surpassed the ending point.
	llb %0xEFFF%;cmp;lb2;cmp;sc%asciifun_looptop%;jmpifneq;
VAR#asciifun_loopout#@
alpop;
farret;



//MAIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

section 0;
ZERO_STACK_POINTER;
la0;st_iteration_count;
astp;alpush;proc_printbytehex;la0x20;putchar;apop;proc_printbytehex;apop;

VAR#main_looptop#@
proc_asciifun;
proc_asciifun;
ld_iteration_count;lb1;add;st_iteration_count;
ld_iteration_count;
lb 0xFF;cmp;lb 0x2;cmp;sc %main_looptop%;jmpifneq;

la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xaf;	lb0;
apush;		blpush;
proc_puts;
la 0xA;putchar;la 0xD;putchar;
//ld_iteration_count;apush;proc_printbytehex;

halt;
