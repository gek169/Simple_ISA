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
	if(a == 0xffFE){ /*Disk Read.*/
		UU i; char buf[0x10000];
		for(i = 0; i<0x10000; i++){
			UU ind = ( (((UU)c&255)<<16)+((UU)b) + i) & 0xffFFff;
			buf[i] = M[ind];
			if(M[ind] == '\0')break;
		}
		buf[0xFFff] = '\0';
		/*
			Attempt to load it into the segment.
		*/
		{FILE* ff;size_t len;size_t n_pages;
			ff = fopen(buf, "rb");
			if(!ff) return 0;
			fseek(ff, 0, SEEK_END);
			len = ftell(ff);
			fseek(ff, 0, SEEK_SET);
			n_pages = (len + 255)/256;
			if(n_pages == 0) return 0;
			if(SEGMENT_PAGES < n_pages){
				SEGMENT = realloc(SEGMENT, 0x100 * n_pages);
				SEGMENT_PAGES = n_pages;
				if(!SEGMENT){puts("Failed Malloc");exit(1);}
			}
			(void)fread(SEGMENT, 1, len, ff);
			fclose(ff);
		}
		return 1;
	}
#if !defined(FUZZTEST)
	if(a == 0xffFD){ /*Disk Write*/
		UU i; char buf[0x10000];
		for(i = 0; i<0x10000; i++){
			UU ind = ( (((UU)c&255)<<16)+((UU)b) + i) & 0xffFFff;
			buf[i] = M[ind];
			if(M[ind] == '\0')break;
		}
		buf[0xFFff] = '\0';
		/*
			Attempt to store the segment.
		*/
		{FILE* ff;
			ff = fopen(buf, "wb");
			if(!ff) return 0;
			fwrite(SEGMENT, 1, ((size_t)SEGMENT_PAGES) * 256, ff);
			fclose(ff);
		}
		return 1;
	}
#endif
	/*TODO: add a disassembler.*/
	return a;
}
