//library functions.
//What region should the library start on?
VAR#LIBC_START#0x12
VAR#LIBC_ADDR#0x10000
asm_muleq#\LIBC_ADDR#\LIBC_START#;
//asm_vars;


//macros
VAR#goto#sc _arg1;jmp;
VAR#ZERO_STACK_POINTER#astp;popa;



//we assume that register a is being used.
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;
VAR#ISEQ_CONST#	load__arg1;llb _arg2;cmp;

//if the variable referenced by argument 1 is 
VAR#goto_ifeq_const#		sc _arg2;load__arg1;llb%_arg3%;cmp;jmpifeq;
VAR#goto_ifneq_const#		sc _arg2;load__arg1;llb%_arg3%;cmp;jmpifneq;
VAR#goto_ifgreater_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%2%;cmp;jmpifeq;
VAR#goto_iflesser_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%0%;cmp;jmpifeq;
VAR#goto_ifgreatereq_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%0%;cmp;jmpifneq;
VAR#goto_iflesser_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%2%;cmp;jmpifneq;


section LIBC_ADDR; asm_begin_region_restriction;
VAR#proc_puts#sc%0%;lla %LIBC_START%;farcall;
//a far pointer has been placed on the stack. 
//| we need this
//V
//FARPTR|FARPTR|
//			   ^stp
astp;lb6;sub;//move the stack pointer back.
illdaa;lb8;rsh;lb255;and;ca;//load the thing that's there. But we also loaded the high byte of the
							//next short, so we need to get rid of that. Goes in C.
astp;lb5;sub;illdaa;//we need to grab that short.

//we now have our far pointer! put it on the top of the stack for easy access.
bc;bpush;alpush;
//FARPTR1|FARPTR2|FARPTRPROC
//				  (upper)[lower]
VAR#puts_looptop#@
POP_FARPTR_VARIABLE;
PUSH_FARPTR_VARIABLE;
farilda;
putchar;
lb0;cmp;sc%puts_loopend%;jmpifeq;
POP_FARPTR_VARIABLE;
ab;lb1;add;ba;
PUSH_FARPTR_VARIABLE;
sc %puts_looptop%;jmp;

VAR#puts_loopend#@
farret;

asm_end_region_restriction;






//DATA SECTION~~~~~~~~~~~~~~~~~~~~~~~~~~~
section 0x1A0000;
VAR#msg_req_string#@
!Enter a string, you dork!:
bytes 10,0;




//MAIN FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
section 0x10000; asm_begin_region_restriction;
VAR#proc_main#sc%0%;lla %1%;farcall;
ZERO_STACK_POINTER;

//we need an i for this loop
VAR#load_main_i#sc%0xF1%;llb %0x0000%;farillda;
VAR#store_main_i#sc%0xF1%;llb %0x0000%;faristla;
lla %0%;store_main_i;
la0x1A;apush;lla%msg_req_string%;alpush;
proc_puts;
//end 
halt;
asm_end_region_restriction;


//BOOTLOADER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
section 0
proc_main;





