#!/usr/bin/sisa16_asm -run

..include"libc_pre.hasm"
..zero:
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

:STR_my_other_string:
..ascii:Krenel shut down...
bytes '\r' ,'\n', 0;
:STR_yet_other_string:
..ascii:End of Program.
bytes '\r' ,'\n', 0;


//ENTER: exec 2048 on the command line!!!
..(55):

	
	// Overwrite the krenel. There is no way it would work if this was being done to krenel memory!
			lrx0 %LIBC_REGION%, %LIBC_KRENEL_BEGIN%;
			lrx1 %LIBC_REGION%, %LIBC_KRENEL_END%;
			overwrite_krenel_looptop:
				cbrx0;
				la 0;
				farista;
				rxincr;
				rxcmp;
				sc %overwrite_krenel_looptop%; jmpifneq;

	side_process_looptop:
	la 'V';
	putchar;
	la '\n'; syscall;
	lla %0xDE02%; syscall; //sleep!
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
	la 'h'; putchar;
	la 'm'; putchar;
	la 'm'; putchar;
	la '?'; putchar;
	la '\r'; putchar;
	la '\n'; putchar; interrupt;
	lrx0 %/0x260000%;
	proc_prints;
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
	lrx2 %/0x50000%;
	lrx1 %/0x90000%;
	bruhtop:
		rx0_2;
//		seg_ld;
		rx0_1; rxincr; rx1_0;
		sc %bruhtop%; jmp;
	
	lb 0; mod;
	halt;
..(8):
	krenel_boot:
		lla %0xDE02%; syscall; //sleep!
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; interrupt;
		push %10%; //make some room for that bootloader!
		//Attempt to get a lock on the segment.
		cpc;
		la 'q'; putchar;
		la '\n'; putchar; interrupt;
		lla %0xDE08%; syscall;
		jmpifneq;



		//exec region syscall.
		lla %0xDE04%; lb 55; syscall;

		//Instead, fork it!
		//bootloader written here
		//This is written at 0: la 55, lfarpc
//		sc %0%; 
//		lb 0; la 2; farista;
//		lb 1; la 55; farista;
//		lb 2; la 0x44; farista;
//		lla %0xDE06%; 
//		syscall;
		nota; sc %main_program_failure%; jmpifeq;
		lrx0 0, %&STR_my_string%;
		proc_prints;
		la '\n'; syscall;
		nota; sc %main_program_failure%; jmpifeq;
		//Send a KILL to pid0
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
		//Send a message to all processes!
		lla %0xDE05%; 
		lb 1;
		sc %0%; 
		lrx0 %/0xAABBCC%; 
		syscall;
		lla %0xDE05%; 
		lb 1;
		sc %1%; 
		lrx0 %/0xAABBCC%; 
		syscall;		

		lla %0xDE05%; 
		lb 1;
		sc %2%; 
		lrx0 %/0xAABBCC%; 
		syscall;

		lla %0xDE05%; 
		lb 1;
		sc %2%; 
		lrx0 %/0xAABBCC%; 
		syscall;

		lla %0xDE05%; 
		lb 1;
		sc %3%; 
		lrx0 %/0xAABBCC%; 
		syscall;
		lla %0xDE05%; 
		lb 1;
		sc %4%; 
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
		lla %0xDEAD%; syscall;
		
		lla %0xDE00%;lb 0;syscall; //Kill the other guy.
		lla %0xDE00%;lb 2;syscall; //Kill the other guy.
		lla %0xDE00%;lb 3;syscall; //Kill the other guy.
		lla %0xDE00%;lb 4;syscall; //Kill the other guy.
		lla %0xDE00%;lb 5;syscall; //Kill the other guy.
		lla %0xDE00%;lb 6;syscall; //Kill the other guy.
		lla %0xDE00%;lb 7;syscall; //Kill the other guy.
		lla %0xDE00%;lb 1;syscall; //Kill ourselves. We are actually 1.
		lb 0; rx1b; fltdiv
		main_program_failure:
		la 'f'; putchar;
		halt;
