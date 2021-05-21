#!/usr/bin/sisa16_asm -run
//define the regions to use.
//we also need to define these ADDR variables, just set them to zero.
VAR#DATA_REGION_1#0x2A
VAR#DATA_REGION_1_ADDR#0
VAR#__START__#0x1
VAR#__START___ADDR#0
VAR#LIBC_START#0x12
VAR#LIBC_START_ADDR#0


VAR#CALCULATE_REGION_LOCATION#asm_muleq#\_arg1__ADDR#0#;|asm_pleq#\_arg1__ADDR#0x10000#;|asm_muleq#\_arg1__ADDR#\_arg1_#;

asm_call#CALCULATE_REGION_LOCATION#DATA_REGION_1##;
asm_call#CALCULATE_REGION_LOCATION#__START__##;
asm_call#CALCULATE_REGION_LOCATION#LIBC_START##;


//macros
VAR#ZERO_STACK_POINTER#astp;popa;
VAR#POP_FARPTR_VARIABLE#	blpop;apop;ca;
VAR#PUSH_FARPTR_VARIABLE#	ac;apush;blpush;
//generic loads and stores for far variables at constant addresses.
VAR#fetch_far_short#sc _arg1_;llb _arg2_; farillda;
VAR#fetch_far_byte#sc _arg1_;llb _arg2_; farilda;
VAR#put_far_short#sc _arg1_;llb _arg2_; faristla;
VAR#put_far_byte#sc _arg1_;llb _arg2_; farista;
//Index into an array whose pointer you know. Argument 3 is the macro to load the indexing variable.
VAR#fetch_far_short_array#load__arg3_;lb2;mul;sc_arg1;llb_arg2_;add;farilldb;ab;
VAR#fetch_far_byte_array#load__arg3_;sc_arg1_;llb_arg2_;add;ba;farilda;
//There are now two arguments.
//Argument 3 is the index, argument 4 is the variable name to store.
VAR#put_far_short_array#	load__arg4_;alpush;\
							load__arg3_;lb2;mul;sc_arg1_;llb_arg2_;add;ba;alpop;faristla;
VAR#put_far_byte_array#	load__arg4_;apush;\
						load__arg3_;sc_arg1_;llb_arg2_;add;ba;apop;farista;

VAR#ISEQ_CONST#	load__arg1_;llb _arg2_;cmp;
VAR#goto_ifeq_const#		sc _arg2_;load__arg1_;llb%_arg3_%;cmp;jmpifeq;
VAR#goto_ifneq_const#		sc _arg2_;load__arg1_;llb%_arg3_%;cmp;jmpifneq;
VAR#goto_ifgreater_const#	sc _arg2_;load__arg1_;llb%_arg3_%;cmp;llb%2%;cmp;jmpifeq;
VAR#goto_iflesser_const#	sc _arg2_;load__arg1_;llb%_arg3_%;cmp;llb%0%;cmp;jmpifeq;
VAR#goto_ifgreatereq_const#	sc _arg2_;load__arg1_;llb%_arg3_%;cmp;llb%0%;cmp;jmpifneq;
VAR#goto_iflesser_const#	sc _arg2_;load__arg1_;llb%_arg3_%;cmp;llb%2%;cmp;jmpifneq;


section LIBC_START_ADDR; asm_begin_region_restriction;
VAR#proc_puts#sc%@%;lla%LIBC_START%;farcall;
	//move the stack pointer back.
	astp;lb6;sub;
	illdaa;lb8;rsh;lb255;and;ca;
	//load the thing thats there. But we also loaded the high byte of the
	//next short, so we need to get rid of that. Goes in C
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
nop;nop;nop;nop;
VAR#proc_printbytehex#sc%@%;lla%LIBC_START%;farcall;
//retrieve our argument.
astp;lb4;sub;ca;ilda;
apush;
lb 4;rsh;
lb0xf;and;
lb 7;mul;
llb %printbytehex_jmptable_1%; 
add; 
ca; 
jmp;
VAR#printbytehex_jmptable_1#@
la0x30;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x31;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x32;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x33;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x34;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x35;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x36;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x37;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x38;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x39;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x41;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x42;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x43;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x44;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x45;putchar;sc%printbytehex_jmptable_1_end%;jmp
la0x46;putchar;sc%printbytehex_jmptable_1_end%;jmp
VAR#printbytehex_jmptable_1_end#@
apop;lb 15;and;lb7;mul;
llb%printbytehex_jmptable_2%;
add;ca;jmp;
VAR#printbytehex_jmptable_2#@
la0x30;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x31;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x32;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x33;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x34;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x35;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x36;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x37;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x38;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x39;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x41;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x42;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x43;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x44;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x45;putchar;sc%printbytehex_jmptable_2_end%;jmp
la0x46;putchar;sc%printbytehex_jmptable_2_end%;jmp
VAR#printbytehex_jmptable_2_end#@
farret;
asm_end_region_restriction;






//DATA SECTION~~~~~~~~~~~~~~~~~~~~~~~~~~~
section DATA_REGION_1_ADDR;
VAR#msg_req_string#@
!Hello World!
bytes 0xd,0xa;
!Another line.
bytes 0;



VAR#BOOT_REGION#__START__
ASM_header bootdol.hasm


//MAIN FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

section __START___ADDR; asm_begin_region_restriction;
VAR#proc_main#sc%__START___ADDR%;lla %__START__%;farcall;
ZERO_STACK_POINTER;


VAR#load_main_i#sc%0x2%;llb%0x0000%;farillda;
VAR#store_main_i#sc%0x2%;llb%0x0000%;faristla;
	la 10;putchar;
	la 13;putchar;	
lla %0%;store_main_i;
laDATA_REGION_1;apush;lla%msg_req_string%;alpush;
proc_puts;
	la 10;putchar;
	la 13;putchar;	
	lrx0 %/0x30%;asm_print;
	lrx1 %/0x700006%;asm_print;rxadd;
	rx0push;rx0pop;
	rx2_0;
	arx0;apush;
	lrx1 %/8%;rxrsh;arx0;apush;
	rx0_2;lrx1 %/16%;rxrsh;arx0;apush;
	rx0_2;lrx1 %/24%;rxrsh;arx0;apush;
		proc_printbytehex;
		apop;
		proc_printbytehex;
		apop;
		proc_printbytehex;
		apop;
		proc_printbytehex;
		apop;
	la 10;putchar;
	la 13;putchar;
	arx2;putchar;
	la 10;putchar;
	la 13;putchar;
halt;
asm_end_region_restriction;




