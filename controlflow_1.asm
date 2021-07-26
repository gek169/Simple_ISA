#!/usr/bin/sisa16_asm -run
//Example program from the manual to demonstrate basic control flow.

..zero:
getchar; 
putchar;
lb '1'; cmp;
sc %Lbl_false%; jmpifneq;
sc %Lbl_true%; jmp;

:Lbl_false:
	halt;
:Lbl_true:
	la '1';
	cpc; 
	putchar;
	jmp;
