#!/usr/bin/sisa16_asm -run
//global variables.
VAR#ld_iteration_count#	sc %0x4%;lb 0;farillda;
VAR#st_iteration_count#	sc %0x4%;lb 0;faristla;

section 0x20000;
VAR#proc_printbytehex#sc 0,0;la 2;farcall; 
//retrieve our argument.
astp;lb4;sub;ca;ilda;
alpush;
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
alpop;lb 15;and;lb7;mul;
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





section 0x10000;
la 0;alpush;st_iteration_count;


//looptop
VAR#looptop#@
	
	proc_printbytehex;
//print spaces.
	la 0x20;putchar;
//print new lines occasionally.
	alpop;alpush;
	lb 0x0F;and;
	//lb 0xf;
	cmp;sc%skipnewline%;jmpifneq;
		la 0xA;putchar;la 0xD;putchar;
	VAR#skipnewline#@
//increment


//check to see if we have reached or surpassed the ending point.
//we want to go to the top.
alpop;lb 1;add;alpush;asm_print;
llb %0xEFFF%;cmp;lb2;cmp;sc%looptop%;jmpifneq;
//increment

VAR#loopout#@
farret;

section 0;
astp;popa;
sc 0,0;
la 1;
farcall;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
la 0xA;putchar;la 0xD;putchar;
