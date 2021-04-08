VAR#looptop#1,0xf0
VAR#GOTO_TOP#sc looptop;la1;jmpifeq

#right at the beginning.
section 0
GOTO_TOP

#the in-between will be filled with zeroes.


section 0x1F0
sc 1,0xF0
#Read character from stdin into A
getchar
#Echo it back
putchar
asm_print
#some nops for room.
nop;nop;nop;nop;nop;nop;
GOTO_TOP
#Unreachable HALT insn
asm_print
halt

