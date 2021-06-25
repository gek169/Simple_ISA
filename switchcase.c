#!/usr/local/bin/tcc -run
#include <stdio.h>

/*
	equivalent of switchcase.asm
*/
int main(){
	unsigned char a = getchar();
	switch((a & 0xf0)>>4){
		case 0: putchar('0'); break;
		case 1: putchar('1'); break;
		case 2: putchar('2'); break;
		case 3: putchar('3'); break;
		case 4: putchar('4'); break;
		case 5: putchar('5'); break;
		case 6: putchar('6'); break;
		case 7: putchar('7'); break;
		case 8: putchar('8'); break;
		case 9: putchar('9'); break;
		case 0xA: putchar('A'); break;
		case 0xB: putchar('B'); break;
		case 0xC: putchar('C'); break;
		case 0xD: putchar('D'); break;
		case 0xE: putchar('E'); break;
		case 0xF: putchar('F'); break;
	}
	switch(a & 0x0f){
		case 0: putchar('0'); break;
		case 1: putchar('1'); break;
		case 2: putchar('2'); break;
		case 3: putchar('3'); break;
		case 4: putchar('4'); break;
		case 5: putchar('5'); break;
		case 6: putchar('6'); break;
		case 7: putchar('7'); break;
		case 8: putchar('8'); break;
		case 9: putchar('9'); break;
		case 0xA: putchar('A'); break;
		case 0xB: putchar('B'); break;
		case 0xC: putchar('C'); break;
		case 0xD: putchar('D'); break;
		case 0xE: putchar('E'); break;
		case 0xF: putchar('F'); break;
	}
	return 0;
}
