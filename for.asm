#!/usr/bin/sisa16_asm -run

//This small program demonstrates a few essential features of Sisa16 and KRENEL
//Notably...
//1) usage of putchar and libc proc_wait
//2) the exec syscall
//3) Multitasking.

.ZERO_STACK_POINTER:		astp;popa;
..include"libc.hasm"

//..include"libc_pre.hasm"
//..(2):
//..dinclude"libc_pre.bin"

.wait_time:				33

..main(3):
lrx0 %/krenel_boot%;
proc_krenel;
halt;

..(4):
krenel_boot:
	lla %0xDE04%;//code for the exec region syscall.
	lb 5;		//What region to exec?
	syscall;	//Make the call
	halt;


..(5):
asm_begin_region_restriction;
sc %fun_start%; 
jmp;

//Space for variables.
:fun_i:
bytes 0,0; // location of the i variable
:fun_end_i:
bytes 2,0; // location of the end

fun_start:
//initialize variables
la 0; stla %fun_i%;
lla %0x301%; stla %fun_end_i%;
fun_loop:
llda %fun_i%;
lldb %fun_end_i%;
cmp;
sc %fun_exit_loop%; jmpifeq;

la '\r'; putchar;
la '\n'; putchar;
llda %fun_i%;

alpush;
apop;
proc_printbytelchex;
apop;
llda %fun_i%;
apush;
proc_printbytelchex;
apop;



//i++
llda %fun_i%; 
aincr;  
stla %fun_i%;
sc %fun_loop%; jmp;
fun_exit_loop:

asm_end_restriction;
