/*Default textmode driver for SISA16.*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static const size_t DISK_ACCESS_MASK = 0x3FffFFff;

static unsigned short shouldquit = 0;

/*
__#if defined(__arm__) && defined(USE_TERMIOS)
__#undef USE_TERMIOS
__#endif
*/
#ifndef NO_SIGNAL
#include <signal.h>
#define TRAP_CTRLC signal(SIGINT, emu_respond);
void emu_respond(int bruh){
	(void)bruh;
	shouldquit = 0xffFF;
	return;
}
#else
#define TRAP_CTRLC /*a comment.*/
#endif


#include "isa_pre.h"
/*
	buffers for stdout and stdin.
*/
#define SCREEN_WIDTH_CHARS 80
#define SCREEN_HEIGHT_CHARS 60
static unsigned char stdout_buf[(SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS) + SCREEN_WIDTH_CHARS] = {0};




/*
	textmode driver.
*/
#ifdef USE_TERMIOS
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
static struct termios oldChars;
static struct termios newChars;
static void initTermios(int echo) 
{
  tcgetattr(STDIN_FILENO, &oldChars); /* grab old terminal i/o settings */
  newChars = oldChars; /* make new settings same as old settings */
  newChars.c_lflag &= ~ICANON; /* disable buffered i/o */
  newChars.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &newChars); /* use these new terminal i/o settings now */
}
static void dieTermios(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldChars); /* use these new terminal i/o settings now */	
}
static void DONT_WANT_TO_INLINE_THIS di(){
	initTermios(0);
	atexit(dieTermios);
#ifndef SISA_DEBUGGER
		TRAP_CTRLC
#endif
	setvbuf ( stdout, stdout_buf, _IOFBF, sizeof(stdout_buf));
}
static void dcl(){}
#else
static void di(){
#ifndef SISA_DEBUGGER
		TRAP_CTRLC
#endif
	return;
}
static void dcl(){return;}

#endif



/*
	non-sdl2 variants of 
*/
#ifndef USE_SDL2
static unsigned short gch(){
#if defined(USE_TERMIOS)
	return 0xff & getchar_unlocked();
#else
	return 0xff & getchar();
#endif
}
static void pch(unsigned short a){
#if defined(USE_TERMIOS)
	putchar_unlocked(a);
#else
	putchar(a);
#endif
}
#endif





static unsigned short DONT_WANT_TO_INLINE_THIS interrupt(unsigned short a,
									unsigned short b,
									unsigned short c,
									unsigned short stack_pointer,
									unsigned short program_counter,
									unsigned char program_counter_region,
									UU RX0,
									UU RX1,
									UU RX2,
									UU RX3,
									u* M
								)
{
	if(a == 0x80) return 0x80; /*Ignore 80- it is reserved for system calls!*/
	if(a == 0) return 0;


	if(a == 1){
		return shouldquit;
	}
	if(a==0xa||a == 0xd) {
		fflush(stdout);
		return a;
	}
	if(a == 0xc){
		printf("\e[H\e[2J\e[3J");
		return a;
	}
	if(a==0xE000){
#ifdef USE_TERMIOS
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);return 1;
#else
		return 0;
#endif
	}
	if(a==0xE001){
#ifdef USE_TERMIOS
		/*int flags = fcntl(STDIN_FILENO, F_GETFL, 0);*/
		fcntl(STDIN_FILENO, F_SETFL, 0);return 1;
#else
		return 0;
#endif
	}
	if(a == 0xffFF){ /*Perform a memory dump.*/
		unsigned long i,j;
		for(i=0;i<(1<<24)-31;i+=32)
			for(j=i,printf("%s\r\n%06lx|",(i&255)?"":"\r\n~",i);j<i+32;j++)
					printf("%02x%c",M[j],((j+1)%8)?' ':'|');
		return a;
	}
	if(a == 0xFF10){ /*Read 256 bytes from saved disk to page b*/
		size_t location_on_disk = ((size_t)RX0) << 8;
		FILE* f = fopen("sisa16.dsk", "rb+");
		location_on_disk &= DISK_ACCESS_MASK;
		if(!f){
			UU i = 0;
			for(i = 0; i < 256; i++){
				M[((UU)b<<8) + i] = 0;
			}
			return 0;
		}
		if(fseek(f, location_on_disk, SEEK_SET)){
			UU i = 0;
			for(i = 0; i < 256; i++){
				M[((UU)b<<8) + i] = 0;
			}
			return 0;
		}
		{
			UU i = 0;
			for(i = 0; i < 256; i++){
				M[((UU)b<<8) + i] = fgetc(f);
			}
		}
		fclose(f);
		return 1;
	}

	if(a == 0xFF11){ /*write 256 bytes from page 'b' to saved disk.*/
		size_t location_on_disk = ((size_t)RX0) << 8;
		FILE* f = fopen("sisa16.dsk", "rb+");
		location_on_disk &= DISK_ACCESS_MASK;
		if(!f){
			f = fopen("sisa16.dsk", "wb");
			if(!f) return 0;
			if((unsigned long)ftell(f) < (unsigned long)location_on_disk) if(EOF == fputc(0, f)) return 0;
			fflush(f);
			fclose(f);
			f = fopen("sisa16.dsk", "rb+");
			if(!f) return 0;
		}
		if(fseek(f, location_on_disk, SEEK_SET)){
			fseek(f, 0, SEEK_END);
			while((unsigned long)ftell(f) < (unsigned long)location_on_disk) if(EOF == fputc(0, f)) return 0;
		}
		{
			UU i = 0;
			for(i = 0; i < 256; i++){
				fputc(M[((UU)b<<8) + i], f);
			}
		}
		fclose(f);
		return 1;
	}
	return a;
}
