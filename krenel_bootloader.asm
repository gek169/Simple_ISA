#!/usr/bin/sisa16_asm -run
..include"libc.hasm"


..main:
	lrx0 %/krenel_boot%;
	proc_krenel;
	halt;
	krenel_boot:
	//Read 64k from disk into main memory.
	lrx2 %/0x3000%;	//our destination.
	lrx3 %/0x0000%;
		krenel_load_looptop:
			brx2;			//Where on disk?
			rx0_3;			//Where in memory?
			lla %0xDE03%
			syscall;
			lrx1 %/256%;
			rxadd;rx3_0;
			rx0_2;rxadd;rx2_0;
			lrx1 %/0x4000%
			rxcmp;
			sc %krenel_load_looptop%; jmpifneq;
	//exec on it.
	lb 3;
	lla %0xDE04%
	syscall;
	la 'd'; putchar;
	halt;
