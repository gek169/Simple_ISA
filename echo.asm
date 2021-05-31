#!/usr/bin/sisa16_asm -run
//create a whole-memory image.
section 0xFFffFF;bytes 0,0;


VAR#looptop#1,0xf0
VAR#GOTO_TOP#sc looptop;jmp;
VAR#jumpc#jmp;
VAR#IS_A_NEWLINE#			sta0,0;lb0xa;cmp;
VAR#IS_A_CR#				sta0,0;lb0xd;cmp;
VAR#IS_A_Q#					sta0,0;lb0x51;cmp;
VAR#GET_A_BACK#				lda0,0;



asm_print %/32%;

#right at the beginning.
section 0
//lrx0 %/65536%;seg_realloc;
GOTO_TOP;
#purposefully redefining here.

#the in-between will be filled with zeroes.


section 0x1F0
//set C to jump to the top of our big for loop
	sc 1,0xF0;
//Read character from stdin into A and echo it backIgnore newlines.
	getchar;
	IS_A_NEWLINE;	jmpifeq;GET_A_BACK;
//	IS_A_CR;		jmpifeq;GET_A_BACK;
	sc %Lbl_BONUSDUCKS%;IS_A_Q;jmpifeq;GET_A_BACK;
	sc 1,0xF0;

#Simple loop


	
//prepare the loop by preparing our looping variable at 5,
//and the address register C to repeatedly jump to the top.
	lb 0;stb 0,5;
	sta 0,0;
//small loop.
VAR#miniLoopTop#@
//Perform wizardry
	lda %0%;
		lb 8;
		//lb 1;
		//add;
		imul14.2;
	sta %0%;
//if this is less than 127, then we go to dont manip.
		lb 127;cmp;lb0;cmp;sc %dont_manip%;jmpifeq;
VAR#still_above_127#@
//generate negative one, twos complement, and put it in b.
	la 1;compl;lb1;add;ba;
	lda %0%;
		add;
	sta %0%;
//jump to still_above if the value is >= 127.
	lb 127;cmp;lb0;cmp;sc %dont_manip%;jmpifneq;
VAR#dont_manip#@	
	lda %0%;putchar;
//perform our variable increment.
	lb1;lda%5%;add;sta %5%;
//if A is less than 3 value, jump to the small loop
	sc %miniLoopTop%;llb %4%;cmp;lb 0;cmp;jmpifeq;


//signify the end of the loop by printing some newlines.
	GOTO_TOP
#Unreachable HALT insn
halt

//if you type in a q, you get ten Qs.
VAR#Lbl_BONUSDUCKS#@
	la 0xa;putchar;
	la 0xd;putchar;
	la 0;rx0a;

VAR#Bonusducks_looptop#@
//how many Qs to print.
VAR#numQs#10
	la 0x51;putchar;
	arx0;lb1;add;rx0a;
	sc %Bonusducks_looptop%;arx0;lb numQs;cmp;lb0;cmp;jmpifeq;
