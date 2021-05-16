/*Implement your own driver here. You can access memory with M[index]*/
#include <stdio.h>
#include <stdlib.h>

/*Linux only code to achieve unbuffered stdin*/
/*#if defined(__unix__) || defined(linux)*/
#if defined(USE_NCURSES)
#include <ncurses.h>
static void di(){initscr();}
static void dcl(){endwin();}
#else
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
									unsigned char program_counter_region
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
