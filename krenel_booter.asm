#!/usr/bin/sisa16_asm -run

..include"libc.hasm"

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


..main:
	lrx0 %/krenel_boot%;
	proc_krenel;
	lrx0 0, %&STR_my_other_string%;
	rx0push;
	proc_puts;
	halt;
..(55):
	side_process_looptop:
	la 'V';
	putchar;
	la '\n'; syscall;
	la 100; alpush; proc_wait; alpop;
	sc %side_process_looptop%; jmp;
	lb 0; div;
	halt;
..(8):
	krenel_boot:
		//Overwrite the krenel. There is no way it would work if this was being done to krenel memory.
		lrx0 %/0x20000%;
		lrx1 %/0x20480%;
		overwrite_krenel_looptop:
			cbrx0;
			la 0;
			farista;
			rxincr;
			rxcmp;
			sc %overwrite_krenel_looptop%; jmpifneq;
		lla %0xDE04%; lb 55; syscall;
		lrx0 0, %&STR_my_string%;
		rx0push;
		proc_puts;
		la '\n'; syscall;
		nota; sc %main_program_failure%; jmpifeq;
		getchar;
		getchar;
		getchar;
		getchar;
		//Commit sudoku- kill ourselves.
		//lla %0xDE00%;lb 0;syscall;
		proc_gets_user;
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		proc_puts;
		lla %0xDE00%;lb 1;syscall;
		la '\r'; putchar;
		la '\n'; putchar;
		la '\n'; syscall;
		rx0pop;
		lrx0 0, %&STR_yet_other_string%;
		rx0push;
		proc_puts;
		rx0pop;
		//lb 0; rx1b; fltdiv
		main_program_failure:
		halt;
