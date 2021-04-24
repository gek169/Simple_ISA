/*Implement your own driver here. You can access memory with M[index]*/
#include <stdio.h>
#include <stdlib.h>

/*Linux only code to achieve unbuffered stdin*/
#include <ncurses.h>
#define di(){initscr();}
#define dcl(){endwin();}

#define gch() getchar()
#define pch(a){putchar(a);}
