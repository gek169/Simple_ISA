//define the regions to use.
//we also need to define these ADDR variables, just set them to zero.
VAR#DATA_REGION_1#0x2A
VAR#DATA_REGION_1_ADDR#0
VAR#__START__#0x1
VAR#__START___ADDR#0
VAR#LIBC_START#0x12
VAR#LIBC_START_ADDR#0


VAR#CALCULATE_REGION_LOCATION#asm_muleq#\_arg1_ADDR#0#;|asm_pleq#\_arg1_ADDR#0x10000#;|asm_muleq#\_arg1_ADDR#\_arg1#;

asm_call#CALCULATE_REGION_LOCATION#DATA_REGION_1##;
asm_call#CALCULATE_REGION_LOCATION#__START__##;
asm_call#CALCULATE_REGION_LOCATION#LIBC_START##;
asm_vars;


//macros
VAR#ZERO_STACK_POINTER#astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;
//generic loads and stores for far variables at constant addresses.
VAR#fetch_far_short#sc _arg1;llb _arg2; farillda;
VAR#fetch_far_byte#sc _arg1;llb _arg2; farilda;
VAR#put_far_short#sc _arg1;llb _arg2; faristla;
VAR#put_far_byte#sc _arg1;llb _arg2; farista;
//Index into an array whose pointer you know. Argument 3 is the macro to load the indexing variable.
VAR#fetch_far_short_array#load__arg3;lb2;mul;sc_arg1;llb_arg2;add;farilldb;ab;
VAR#fetch_far_byte_array#load__arg3;sc_arg1;llb_arg2;add;ba;farilda;
//There are now two arguments.
//Argument 3 is the index, argument 4 is the variable name to store.
VAR#put_far_short_array#	load__arg4;alpush;\
							load__arg3;lb2;mul;sc_arg1;llb_arg2;add;ba;alpop;faristla;
VAR#put_far_byte_array#	load__arg4;apush;\
						load__arg3;sc_arg1;llb_arg2;add;ba;apop;farista;

VAR#ISEQ_CONST#	load__arg1;llb _arg2;cmp;
VAR#goto_ifeq_const#		sc _arg2;load__arg1;llb%_arg3%;cmp;jmpifeq;
VAR#goto_ifneq_const#		sc _arg2;load__arg1;llb%_arg3%;cmp;jmpifneq;
VAR#goto_ifgreater_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%2%;cmp;jmpifeq;
VAR#goto_iflesser_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%0%;cmp;jmpifeq;
VAR#goto_ifgreatereq_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%0%;cmp;jmpifneq;
VAR#goto_iflesser_const#	sc _arg2;load__arg1;llb%_arg3%;cmp;llb%2%;cmp;jmpifneq;


section LIBC_START_ADDR; asm_begin_region_restriction;
VAR#proc_puts#sc%@%;lla%LIBC_START%;farcall;
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
POP_FARPTR_VARIABLE;
farret;
asm_end_region_restriction;






//DATA SECTION~~~~~~~~~~~~~~~~~~~~~~~~~~~
section DATA_REGION_1_ADDR;
VAR#msg_req_string#@
!Enter a string, you dork!:
bytes 0xd,0xa,0;




//MAIN FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
section __START___ADDR; asm_begin_region_restriction;
VAR#proc_main#sc%__START___ADDR%;lla %__START__%;farcall;
ZERO_STACK_POINTER;


VAR#load_main_i#sc%0x2%;llb%0x0000%;farillda;
VAR#store_main_i#sc%0x2%;llb%0x0000%;faristla;

lla %0%;store_main_i;
laDATA_REGION_1;apush;lla%msg_req_string%;alpush;
proc_puts;

halt;
asm_end_region_restriction;










//BOOTLOADER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
section 0;proc_main;
