#!/usr/local/bin/sisa16_asm -run


//Macros!

VAR#myComptimeVar#2

//All the code before here was entirely meaningless. Ignore it.

#Compile this program using asm. ./asm -i program.asm -o program.bin
section 0
#fills 65536 bytes with zeroes.
#notice the region restriction....
asm_begin_region_restriction
fill 65536,0
asm_end_region_restriction

section 0x3000
#demonstrating a data include.
asm_begin_region_restriction

ASM_data_include README.md

asm_end_region_restriction

section 0xF000
!hello world!!! You should see this print.
bytes 0xA, 0xD;
!Another string.
bytes 0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD;
bytes 0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD;
bytes 0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD;
bytes 0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD,0xA,0xD;

section 0
#loop control
lda %0x1e8%
lb 1
sc %%;
add
#using split syntax
sta %0x1e8%
#grab our character from our array of characters
llb %2%;sub;
llb %0xF000%
add
illdaa;
llb %255%;and;
putchar



#asm_fix_outputcounter+2;
sc %_myLabel_%;


#					asm_vars


jmp;
VAR#_myLabel_#@
sc 0,0

#perform the loop check
lda 1,0xe8
lb 0x41
cmp;lb 0;cmp;jmpifeq




//zero the stack pointer and then set it to a sensible value
astp;popa;lla %0xE000%;pusha;
lda %0x1e8%;putchar;

sc %0xA0D0%;
lla %0x1%
//these operations should do nothing.
alpush;alpush;
lla %0x0%;
alpop;alpop;
farcall;
#we should see this when we return.
la 0x41
putchar;putchar;putchar;
//enable these two lines to dump the memory of the emulator after execution.
lla %0xFFff%;
//transfer code from the segment.
lrx0 %/2000%;seg_realloc;
lrx0 %/0x0%;
lrx1 %/0%;
seg_st;
//transfer back from the segment.
lrx0 %/0xffff%;
seg_ld;
interrupt;
lrx0 %-20%;
lrx1 %/5%;
rxicmp
halt



section 0xB000
!QQQQQQQQQQQQQQQQQQ
section 0xC000
!FFFFFFFFFFFFFFFFF

//defining a function out here.
//this function prints Qs to the screen.
section 0x1A0D0
la 0xa
putchar;
la 0xd
putchar;
lla %0xB000%;illdaa;
putchar;putchar;
putchar;putchar;

VAR#goto_myLabel2#sc %_myLabel2_%;jmp;
goto_myLabel2
VAR#_myLabel2_#@

//test far memory indexing functionality
lla %0xffea%;
sc %0xff%;
lb 0x59
faristlb;
lb 0xa
farilldb;
ab;
putchar;putchar;
putchar;


//test far memory indexing functionality for bytes
lla %0xeeff%;
sc %0xed%;
lb 0x57
faristb;
lb 0xa
farildb;
ab;
putchar;putchar;
putchar;


//test farpagel and farpagest
lla%0xee%;
sc %0xffff%
farpagel;
//Copy that page around some so we can see it.
sc %0xfffe%
farpagest;
sc %0xfffd%
farpagest;
sc %0xfffc%
farpagest;
sc %0xfffb%
farpagest;
la 0xa
putchar;putchar;

farret;//return back to the zero page.


//Testing variable addressing.
//This code is never executed.
section @+1+
sc $+1+;sc $+1+;sc $+1+;sc $+1+;sc $+1+;sc $+1+;
section 0xFFFF00
!KAKAKAIII
imul3.13

VAR#somethingg#$;asm_print;


bytes somethingg
sc %5%;
bytes somethingg
