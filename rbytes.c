#!/usr/local/bin/tcc -run
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
	long i = 0;srand(time(NULL));for(;i<0x1000000;i++){putchar(rand()&255);}
	return 0;
}
