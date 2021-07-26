#!/usr/bin/sisa16_asm -run
//create a whole-memory image.


..include"libc.hasm"

.looptop:1,0xf0
.GOTO_TOP:			sc looptop;jmp;
.IS_A_NEWLINE:		sta%50%;lb0xa;cmp;
.IS_A_FF:			sta%50%;lb0xff;cmp;
.IS_A_CR:			sta%50%;lb0xd;cmp;
.IS_A_Q:			sta%50%;lb0x51;cmp;
.IS_A_R:			sta%50%;lb0x52;cmp;
.GET_A_BACK:		lda%50%;


asm_print %/32%;

#right at the beginning.
//..z:
section 0;
push %5000%;
lrx0 %/true_start%;
proc_krenel;
la 'f'; putchar;
la 'i'; putchar;
la 'n'; putchar;
la 'i'; putchar;
la 's'; putchar;
la 'h'; putchar;
la 'e'; putchar;
la 'd'; putchar;
la '\r'; putchar;
la '\n'; putchar; interrupt;
halt;
true_start:
nop;nop;
GOTO_TOP;
#purposefully redefining here.

#the in-between will be filled with zeroes.


section 0x1F0
//set C to jump to the top of our big for loop
	cpc
	la '\n'; interrupt;
//Read character from stdin into A and echo it backIgnore newlines.
	getchar;
	IS_A_NEWLINE;					jmpifeq;GET_A_BACK;
	IS_A_FF;						jmpifeq;GET_A_BACK;
	sc %Lbl_BONUSDUCKS%;	IS_A_Q;jmpifeq;GET_A_BACK;
	sc %Lbl_Emulator%;		IS_A_R;jmpifeq;GET_A_BACK;
//prepare the loop by preparing our looping variable at 59,
//and the address register C to repeatedly jump to the top.
	lb 0;
	stb %59%;
	sta %50%;
//small loop.
	:miniLoopTop:
		//Perform wizardry
			lda %50%;
				lb 1;
				add;
			sta %50%;
		//if it is exactly equal to 0xff, then we go back.
				lb 0xff;cmp
		//if this is less than 127, then we go to dont manip.
				lb 127;cmp;lb0;cmp;sc %dont_manip%;jmpifeq;
		:still_above_127:
		//generate negative one, twos complement, and put it in b.
			la 1;compl;lb1;add;ba;
			lda %50%;
				add;
			sta %50%;
		//jump to still_above if the value is >= 127.
			lb 127;cmp;lb0;cmp;sc %still_above_127%;jmpifneq;
		:dont_manip:
			//if a is not invalid, we dont subtract from it.
			lda %50%;IS_A_Q;sc %A_IS_notQ%;jmpifneq;
			lda %50%;
			lb5;
			sub;
			sta %50%;
		:A_IS_notQ:
			lda %50%;putchar;
		//perform our variable increment.
			lb1;lda%59%;add;sta %59%;
		//if A is less than 3 value, jump to the small loop
			sc %miniLoopTop%;llb %2%;cmp;lb 0;cmp;jmpifeq;
		//signify the end of the loop by printing some newlines.
		GOTO_TOP

//if you type in a q, you get ten Qs.
:Lbl_BONUSDUCKS:
	la 0xa;putchar;
	la 0xd;putchar;
	la 0;rx0a;
	:Bonusducks_looptop:
		//how many Qs to print.
		.numQs:7
			la 0x51;putchar;
			arx0;lb1;add;rx0a;
			sc %Bonusducks_looptop%;arx0;lb numQs;cmp;lb0;cmp;jmpifeq;
		lrx0 %/12%;
		lrx1 %/0%;
		rxidiv;

:Lbl_BROKE_EMU:
	la0xd;putchar;
	la 'E';putchar;
	la 'R';putchar;
	la 'R';putchar;
	la '0';putchar;
	la 'R';putchar;
	la0; alpush;
	space_printer_looptop:
		la ' ';putchar;
		alpop;aincr;alpush;
		lb 20; cmp; lb 0; cmp; sc %space_printer_looptop%; jmpifeq;
	lrx1 %/0%;
	rximod;
	nop;nop;nop;
:Lbl_Emulator:
	//print an R.
	la0x52;asm_print;
	putchar;
	la 0xff;
	sta %0x1000%;
					lb 11;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	lda %0x1000%;	lb 12;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	lda %0x1000%;	lb  4;cmp;sc %Lbl_BROKE_EMU%;jmpifeq;
	sc %0x1f0%;jmp;
	//So that a disassembly will not show beyond this point.
	halt;halt;
	halt;halt;
