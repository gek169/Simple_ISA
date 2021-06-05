/*Default textmode driver for SISA16.*/
#include <stdio.h>
#include <stdlib.h>
#if defined(USE_NCURSES)
/*
	the Ncurses binding cannot be put in the public domain,
	but the di/dcl interface is.
	The following four lines are under this license:
	Copyright 2018-2019,2020 Thomas E. Dickey
	Copyright 1998-2017,2018 Free Software Foundation, Inc.
	Copyright 2021-2021 David M. Webster
	
	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, distribute with modifications, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
	DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
	THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Except as contained in this notice, the name(s) of the above copyright
	holders shall not be used in advertising or otherwise to promote the
	sale, use or other dealings in this Software without prior written
	authorization.
*/
#include <ncurses.h>
#include "isa_pre.h"
static void di(){if(EMULATE_DEPTH==0)initscr();}
static void dcl(){if(EMULATE_DEPTH==0)endwin();}
#else
/*
	Fully Public domain textmode driver.
*/
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
		{FILE* ff;size_t len;UU n_pages;
			ff = fopen(buf, "rb");
			if(!ff) return 0;
			fseek(ff, 0, SEEK_END);
			len = ftell(ff);
			fseek(ff, 0, SEEK_SET);
			n_pages = (len + 255)/256;
			if(n_pages == 0) return 0;
			if(SEGMENT_PAGES < n_pages){
				u* SEGMENT_OLD = SEGMENT;
				UU SEGMENT_PAGES_OLD = SEGMENT_PAGES;
				SEGMENT = realloc(SEGMENT, 0x100 * n_pages);
				SEGMENT_PAGES = n_pages;
				if(!SEGMENT){
					SEGMENT = SEGMENT_OLD;
					SEGMENT_PAGES = SEGMENT_PAGES_OLD;
					return 0;
				}
			}
			fread(SEGMENT, 1, len, ff);
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
