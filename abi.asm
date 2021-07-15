#!/usr/local/bin/sisa16_asm -run
//create a whole-memory image.
section 0xFFffFF;bytes 0,0;
//define the regions to use.
//we also need to define these ADDR variables, just set them to zero.

VAR#DATA_REGION_1#0x2A
VAR#DATA_REGION_1_ADDR#0x2A0000
VAR#__START__#0x1
VAR#__START___ADDR#0x10000

//macros
.ZERO_STACK_POINTER:astp;popa;
.POP_FARPTR_VARIABLE:	blpop;apop;ca;
.PUSH_FARPTR_VARIABLE:	ac;apush;blpush;


..include"libc.hasm"






//DATA SECTION~~~~~~~~~~~~~~~~~~~~~~~~~~~
section DATA_REGION_1_ADDR;
VAR#msg_req_string#@
!Hello World!
bytes 0xd,0xa;
!Another line.
bytes 0;
VAR#filename#@
..asciz:f.txt
bytes 0;
VAR#failed_to_load_file#@
!could not load file f.txt! Error!
bytes 0;


.BOOT_REGION:__START__
..zero:la BOOT_REGION;lfarpc;asm_print;


//MAIN FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

section __START___ADDR; asm_begin_region_restriction;
VAR#proc_main#sc%__START___ADDR%;lla %__START__%;farcall;
ZERO_STACK_POINTER;
	la 10;putchar;
	la 13;putchar;	
	la DATA_REGION_1; apush;
	lla%msg_req_string%;alpush;
	proc_puts;
	la 10;putchar;
	la 13;putchar;	
	lrx0 %/3%;
//	lrx0 %?3%;
	rxitof;
	lrx1 %?0.59%;
	fltdiv;
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

//Load a file.
	sc %DATA_REGION_1%;llb%filename%;
	lla %0xFFfE%;interrupt;

//Test if the file was loaded correctly.
	sc %file_txt_was_good%;lb1;cmp;jmpifeq;
//file was bad
		laDATA_REGION_1;apush;lla%failed_to_load_file%;alpush;
		proc_puts;
		halt;

VAR#file_txt_was_good#@
//load a single page.
	lrx0 %/0xBB00%;
	lrx1 %/0%;
	seg_ld;
//print the contents of the file.
	la 0xBB;apush;lla %0%;alpush;
	proc_puts;
	
halt;
asm_end_region_restriction;
