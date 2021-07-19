#!/usr/bin/sisa16_asm -run
//Example program from the manual to demonstrate basic control flow.

..main:
getchar; 
putchar;
lb '1'; cmp;
sc %Lbl_val_is_lt%; jmpifneq;
sc %Lbl_val_is_gte%; jmp;

:Lbl_val_is_lt:
	halt;
:Lbl_val_is_gte:
	la '1';
	cpc; 
	putchar; 
	jmp;
