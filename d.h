/*Implement your own driver here. You can access memory with M[index]*/
#include <stdio.h>
#include <stdlib.h>

/*Linux only code to achieve unbuffered stdin*/
/*#if defined(__unix__) || defined(linux)*/
#if defined(USE_NCURSES)
#include <ncurses.h>
#define di(){initscr();}
#define dcl(){endwin();}
#else
#define di(){;}
#define dcl(){;}
#endif

#define gch() getchar()
#define pch(a){putchar(a);}
