#!/usr/bin/sisa16_asm -run

//..include"libc.hasm"
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"
..(0x25):
	my_page_o_crap:
	fill 256, '7'
..(7):
:STR_my_string:
..ascii:Enter some text:
bytes '\r' ,'\n', 0;

bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
bytes 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;

:STR_my_other_string:
..ascii:Krenel shut down...
bytes '\r' ,'\n', 0;
:STR_yet_other_string:
..ascii:End of Program.
bytes '\r' ,'\n', 0;


..main(3): //three actually goes unused by krenel, so it is safe to put code there. Dont use 1, or just ..main
	lrx0 %/krenel_boot%;
	proc_krenel;
	halt;
..(55):
	side_process_looptop:
	la 'V';
	putchar;
	la '\n'; syscall;
//	lla %0xDE02%; syscall; //sleep!
	la 100; alpush; proc_wait; alpop;
	farllda %&0xAABBCC%; 
	nota;
	sc %side_process_looptop%; jmpifeq; //continue looping while zero.

	//Shutdown.
	//lla %0xDEAD%; syscall;

	//write to disk! See if it makes it through at all.


	
	llb %0x2500%;
	lla %0xDE01%; //Disk write.
	lrx0 %/0x3001%;
	syscall;

	lrx0 %/0x3001%;
	llb %0x2600%;
	lla %0xDE03%; //Disk read.
	syscall;
	la '\r'; putchar;
	la '\n'; putchar; 
	la 'q'; putchar;
	la 'u'; putchar;
	la 'i'; putchar;
	la 't'; putchar;
	la '\r'; putchar;
	la '\n'; putchar; interrupt;
	lrx0 %/0x260000%;
	rx0push;
	proc_prints;
	rx0pop;
	la '\n'; interrupt;
	//Fork bomb. Uncomment at your own risk.
	sc %0%; 
	lb 0;la 2; farista;
	lb 1; 
	la 1; 			//if this process was created with exec.
	//la 55;        //if this process was created with fork.
	farista;
	lb 2; la 0x44; farista;
	lla %0xDE06%; 
	syscall;
	cpc;
	jmp;
	lb 0; mod;
	halt;
..(8):
	krenel_boot:
		push %10%; //make some room for that bootloader!
//		//<TODO: needs update> Overwrite the krenel. There is no way it would work if this was being done to krenel memory.
//		lrx0 %/0x20000%;
//		lrx1 %/0x20480%;
//		overwrite_krenel_looptop:
//			cbrx0;
//			la 0;
//			farista;
//			rxincr;
//			rxcmp;
//			sc %overwrite_krenel_looptop%; jmpifneq;

		//exec region syscall.
		lla %0xDE04%; lb 55; syscall;

		//Instead, fork it!
		//bootloader written here
		//This is written at 0: la 55, lfarpc
		//sc %0%; 
		//lb 0;la 2; farista;
		//lb 1; la 55; farista;
		//lb 2; la 0x44; farista;
		//lla %0xDE06%; 
		//syscall;
		nota; sc %main_program_failure%; jmpifeq;
		lrx0 0, %&STR_my_string%;
		proc_prints;
		la '\n'; syscall;
		nota; sc %main_program_failure%; jmpifeq;
		//Commit sudoku- kill ourselves.
		//lla %0xDE00%;lb 0;syscall;
		lrx0 0, %&STR_my_string%;
		proc_gets;
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		lrx0 0, %&STR_my_string%;
		proc_prints;
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		//lla %0xDE00%;lb 1;syscall; //Kill the other guy.
		//Send him a message instead.
		lla %0xDE05%; 
		lb 1;
		sc %1%; 
		lrx0 %/0xAABBCC%; 
		syscall;
		lrx0 0, %&STR_my_string%;
		proc_gets
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		lrx0 0, %&STR_my_string%;
		proc_prints;
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		lrx0 0, %&STR_yet_other_string%;
		proc_prints;
		//lla %0xDEAD%; syscall;
		
		lla %0xDE00%;lb 1;syscall; //Kill the other guy.
		lla %0xDE00%;lb 2;syscall; //Kill the other guy.
		lla %0xDE00%;lb 3;syscall; //Kill the other guy.
		lla %0xDE00%;lb 4;syscall; //Kill the other guy.
		lla %0xDE00%;lb 5;syscall; //Kill the other guy.
		lla %0xDE00%;lb 6;syscall; //Kill the other guy.
		lla %0xDE00%;lb 7;syscall; //Kill the other guy.
		lla %0xDE00%;lb 8;syscall; //Kill ourselves. 8 wraps around to 0.
		lb 0; rx1b; fltdiv
		main_program_failure:
		halt;
