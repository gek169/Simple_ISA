/*Implement your own driver here. You can access memory with M[index]*/
#include <stdio.h>
#include <stdlib.h>

/*Linux only code to achieve unbuffered stdin*/
#include <ncurses.h>
void di(){initscr();}
void dcl(){endwin();}

unsigned short gch(){return getchar();}
void pch(unsigned short a){putchar(a);}
