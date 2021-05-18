#!/usr/bin/sisa16_asm -run
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
	sta 0,0;lb 0;stb 0,5;

	sc $;asm_print

	lda 0,0;lb 1;add;sta 0,0;
	putchar;
	lda 0,5;add;sta 0,5;
#if A is less than 3 value, jump to the loop
	llb %3%;cmp;lb 0;cmp;jmpifeq;


//signify the end of the loop by printing some newlines.
	GOTO_TOP
#Unreachable HALT insn
halt

//if you type in a q, you get ten Qs.
VAR#Lbl_BONUSDUCKS#@
	la 0;rx0a;

VAR#Bonusducks_looptop#@
	la 0x51;putchar;
	arx0;lb1;add;rx0a;
	sc %Bonusducks_looptop%;arx0;lb10;cmp;lb0;cmp;jmpifeq;
