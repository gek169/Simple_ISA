#!/usr/bin/sisa16_asm -run
//create a whole-memory image.



VAR#looptop#1,0xf0
VAR#GOTO_TOP#sc looptop;jmp;
VAR#jumpc#jmp;
VAR#IS_A_NEWLINE#			sta%50%;lb0xa;cmp;
VAR#IS_A_CR#				sta%50%;lb0xd;cmp;
VAR#IS_A_Q#					sta%50%;lb0x51;cmp;
VAR#IS_A_R#					sta%50%;lb0x52;cmp;
VAR#GET_A_BACK#				lda%50%;


asm_print %/32%;

#right at the beginning.
section 0
nop;nop;
GOTO_TOP;
#purposefully redefining here.

#the in-between will be filled with zeroes.


section 0x1F0
//set C to jump to the top of our big for loop
	sc 1,0xF0;
//Read character from stdin into A and echo it backIgnore newlines.
	getchar;
	IS_A_NEWLINE;	jmpifeq;GET_A_BACK;
	sc %Lbl_BONUSDUCKS%;	IS_A_Q;jmpifeq;GET_A_BACK;
	sc %Lbl_Emulator%;		IS_A_R;jmpifeq;GET_A_BACK;
	sc 1,0xF0;

#Simple loop


	
//prepare the loop by preparing our looping variable at 59,
//and the address register C to repeatedly jump to the top.
	lb 0;
	stb %59%;
	sta %50%;
//small loop.
VAR#miniLoopTop#@
	//Perform wizardry
		lda %50%;
			lb 8;
			//lb 1;
			//add;
			imul13.3;
		sta %50%;
	//if this is less than 127, then we go to dont manip.
			lb 127;cmp;lb0;cmp;sc %dont_manip%;jmpifeq;
	VAR#still_above_127#@
	//generate negative one, twos complement, and put it in b.
		la 1;compl;lb1;add;ba;
		lda %50%;
			add;
		sta %50%;
	//jump to still_above if the value is >= 127.
		lb 127;cmp;lb0;cmp;sc %dont_manip%;jmpifneq;
	VAR#dont_manip#@
		//if a is not invalid, we dont subtract from it.
		lda %50%;IS_A_Q;sc %A_IS_notQ%;jmpifneq;
		lda %50%;
		lb5;
		sub;
		sta %50%;

	VAR#A_IS_notQ#@
		
		lda %50%;putchar;
	//perform our variable increment.
		lb1;lda%59%;add;sta %59%;
	//if A is less than 3 value, jump to the small loop
		sc %miniLoopTop%;llb %2%;cmp;lb 0;cmp;jmpifeq;


//signify the end of the loop by printing some newlines.
	GOTO_TOP
//separating nops.
nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;
nop;nop;nop;nop;nop;

//if you type in a q, you get ten Qs.
VAR#Lbl_BONUSDUCKS#@
	la 0xa;putchar;
	la 0xd;putchar;
	la 0;rx0a;
	VAR#Bonusducks_looptop#@
		//how many Qs to print.
		VAR#numQs#5
			la 0x51;putchar;
			arx0;lb1;add;rx0a;
			sc %Bonusducks_looptop%;arx0;lb numQs;cmp;lb0;cmp;jmpifeq;
		lrx0 %/12%;
		lrx1 %/0%;
		rxidiv;

VAR#Lbl_BROKE_EMU#@
	la0xd;putchar;
	la69;putchar;
	la82;putchar;
	la82;putchar;
	la0x30;putchar;
	la82;putchar;
	la0x20;putchar;
	la0x20;putchar;
	la0x20;putchar;
	la0x20;putchar;
	lrx1 %/0%;
	rximod;
	nop;nop;nop;
VAR#Lbl_Emulator#@
	//print an R.
	la0x52;
	putchar;
	la 0xff;
	//execute the emulator!
	emulate;
	sta %0x1000%;
					lb 11;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	lda %0x1000%;	lb 12;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	lda %0x1000%;	lb  4;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	sc %0x1f0%;jmp;
	//So that a disassembly will not show beyond this point.
	halt;halt;
	halt;halt;
