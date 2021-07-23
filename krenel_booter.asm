#!/usr/bin/sisa16_asm -run

..include"libc.hasm"

..(7):
:STR_my_string:
..asciz:Hello World!

..main:
	lrx0 %/krenel_boot%;
	proc_krenel;
	halt;
	krenel_boot:
		lrx0 0, %&STR_my_string%;
		rx0push;
		proc_puts;
		rx0pop;
		halt;
