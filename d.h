/*Default textmode driver for SISA16.*/
#include <stdio.h>
#include <stdlib.h>
#if defined(USE_NCURSES)
#include <ncurses.h>
#include "isa_pre.h"
static void di(){initscr();}
static void dcl(){endwin();}
#else
#include "isa_pre.h"
static void di(){return;}
static void dcl(){return;}
#endif

static unsigned short gch(){
	return getchar();
}
static void pch(unsigned short a){
	putchar(a);
}
static unsigned short interrupt(unsigned short a,
									unsigned short b,
									unsigned short c,
									unsigned short stack_pointer,
									unsigned short program_counter,
									unsigned char program_counter_region,
									UU RX0,
									UU RX1,
									UU RX2,
									UU RX3
								)
{
	if(a == 0xffFF){ /*Perform a memory dump.*/
		unsigned long i,j;
		for(i=0;i<(1<<24)-31;i+=32)	
			for(j=i,printf("%s\r\n%04lx|",(i&255)?"":"\r\n~",i);j<i+32;j++)
					printf("%02x%c",M[j],((j+1)%8)?' ':'|');
	}
	return a;
}
