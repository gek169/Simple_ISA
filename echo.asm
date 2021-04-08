VAR#looptop#1,0xf0
VAR#GOTO_TOP#sc looptop;jmp;
VAR#jumpc#jmp;
VAR#jumpc#jmp;
VAR#jumpc#jmp;
VAR#jumpc#jmp;
VAR#IS_A_NEWLINE#sta0,0;lb0xa0;cmp;
VAR#GET_A_BACK#lda0,0;

#right at the beginning.
section 0
GOTO_TOP

#the in-between will be filled with zeroes.


section 0x1F0
sc 1,0xF0
#Read character from stdin into A and echo it back. Ignore newlines.
getchar
IS_A_NEWLINE;jmpifeq;GET_A_BACK;
putchar

#Simple loop

//prepare the loop by preparing our looping variable at 5,
//and the address register C to repeatedly jump to the top.
sta 0,0;lb 0;stb 0,5;

sc $;asm_print
lda 0,0;llb 0,2;compl;add;sta 0,0;
putchar;
lda 0,5;add;sta 0,5;
#if A is less than this value, jump to the loop
llb 0,15;
cmp;
lb 0;
cmp;
jmpifeq;

#jumpc


#signify the end of the loop by printing some newlines.
GOTO_TOP
#Unreachable HALT insn
halt

