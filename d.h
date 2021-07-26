/*Default textmode driver for SISA16.*/
#include <stdio.h>
#include <stdlib.h>
#ifdef USE_SDL2
#ifdef USE_TERMIOS
#undef USE_TERMIOS
#endif
#endif
#include "isa_pre.h"
/*
	buffers for stdout and stdin.
*/
#define SCREEN_WIDTH_CHARS 80
#define SCREEN_HEIGHT_CHARS 60
static unsigned char stdout_buf[(SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS) + SCREEN_WIDTH_CHARS] = {0};
#ifdef USE_SDL2
static const UU SCREEN_LOC = 0xB00000;
static const UU AUDIO_LOC_MEM = (0xffFF + SCREEN_LOC + (SCREEN_WIDTH_CHARS * 64 * SCREEN_HEIGHT_CHARS)) & 0xFF0000;
/*
	The SDL2 driver keeps a "standard in" buffer.
*/
static unsigned char stdin_buf[(SCREEN_WIDTH_CHARS * 64 * SCREEN_HEIGHT_CHARS)] = {0};
/*
	buffer pointer.
*/
unsigned short stdin_bufptr = 0;

/*
	Cursor position.
*/
UU  curpos = 0;
/*
	SDL2 driver, plus simple text mode.
*/

#define SDL_MAIN_HANDLED
#ifdef __TINYC__
#define SDL_DISABLE_IMMINTRIN_H 1
#endif

#include "font8x8_basic.h"
#include <SDL2/SDL.h>
/*
	TODO- refactor/rewrite av driver, integrate with getchar/putchar to make a "text mode".
*/
static SDL_Window *sdl_win = NULL;
static SDL_Renderer *sdl_rend = NULL;
static SDL_Texture *sdl_tex = NULL;
static SDL_AudioSpec sdl_spec = {0};
static const unsigned int display_scale = 2;
static UU audio_left = 0;
#ifndef SDL2_NO_EMULATE_BLOCKING_INPUT
static char blocking_input = 1;
#endif
static unsigned short shouldquit = 0;
static unsigned char active_audio_user = 0;
static unsigned char FG_color = 15;
static unsigned char BG_color = 0;
static UU SDL_targ[SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS * 64] = {0}; /*Default to all black.*/
static UU vga_palette[256] = {
#include "vga_pal.h"
};
static void DONT_WANT_TO_INLINE_THIS sdl_audio_callback(void *udata, Uint8 *stream, int len){
	SDL_memset(stream, 0, len);
	if(audio_left == 0){return;}
	len = (len < audio_left) ? len : audio_left;
	SDL_MixAudio(stream, M_SAVER[active_audio_user] + 0xB50000 + (0xB0000 - audio_left), len, SDL_MIX_MAXVOLUME);
	audio_left -= len;
}

static void DONT_WANT_TO_INLINE_THIS di(){
	    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	    {
	        printf("SDL2 could not be initialized!\n"
	               "SDL_Error: %s\n", SDL_GetError());
	        exit(1);
	    }
		sdl_spec.freq = 16000;
		sdl_spec.format = AUDIO_S16MSB; 
		sdl_spec.channels = 1;
		sdl_spec.silence = 0;
		sdl_spec.samples = 2048;
		sdl_spec.callback = sdl_audio_callback;
		sdl_spec.userdata = NULL;
		sdl_win = SDL_CreateWindow("[SISA-16]",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH_CHARS * 8 * display_scale, 
			SCREEN_HEIGHT_CHARS * 8 * display_scale,
			SDL_WINDOW_SHOWN
		);
		if(!sdl_win)
		{
			printf("SDL2 window creation failed.\n"
				"SDL_Error: %s\n", SDL_GetError());
			exit(1);
		}
		sdl_rend = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_ACCELERATED);
		if(!sdl_rend){
			printf("SDL2 renderer creation failed.\n"
				"SDL_Error: %s\n", SDL_GetError());
			exit(1);
		}
		sdl_tex = SDL_CreateTexture(sdl_rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 
			SCREEN_WIDTH_CHARS * 8,
			SCREEN_HEIGHT_CHARS * 8
		);
		if(!sdl_tex){
			printf("SDL2 texture creation failed.\n"
				"SDL_Error: %s\n", SDL_GetError());
			exit(1);
		}
		if ( SDL_OpenAudio(&sdl_spec, NULL) < 0 ){
		  printf("\r\nSDL2 audio opening failed!\r\n"
		  "SDL_Error: %s\r\n", SDL_GetError());
		  exit(-1);
		}
		SDL_PauseAudio(0);
		SDL_StartTextInput();
}
static void DONT_WANT_TO_INLINE_THIS dcl(){
		SDL_DestroyTexture(sdl_tex);
		SDL_DestroyRenderer(sdl_rend);
		SDL_CloseAudio();
    	SDL_DestroyWindow(sdl_win);
	    SDL_Quit();
}
static void pollevents(){
	SDL_Event ev;
	while(SDL_PollEvent(&ev)){
		if(ev.type == SDL_QUIT) shouldquit = 0xFFff; /*Magic value for quit.*/
		else if(ev.type == SDL_TEXTINPUT){
			char* b = ev.text.text;
			while(*b) {stdin_buf[stdin_bufptr++] = *b; b++;}
		}else if(ev.type == SDL_KEYDOWN){
			switch(ev.key.keysym.scancode){
				default: break;
				case SDL_SCANCODE_DELETE: stdin_buf[stdin_bufptr++] = 0x7F; break;
				case SDL_SCANCODE_BACKSPACE: stdin_buf[stdin_bufptr++] = 0x7F;break;
				case SDL_SCANCODE_KP_BACKSPACE: stdin_buf[stdin_bufptr++] = 0x7F;break;
				case SDL_SCANCODE_RETURN: stdin_buf[stdin_bufptr++] = 0xa;break;
				case SDL_SCANCODE_RETURN2: stdin_buf[stdin_bufptr++] = 0xa;break;
				case SDL_SCANCODE_KP_ENTER: stdin_buf[stdin_bufptr++] = 0xa;break;
				case SDL_SCANCODE_ESCAPE: stdin_buf[stdin_bufptr++] = '\e';break;
			}
		}
	}
}
static unsigned short gch(){
#ifndef SDL2_NO_EMULATE_BLOCKING_INPUT
	while(blocking_input && stdin_bufptr == 0){
		SDL_Delay(16);
		pollevents();
	}
#endif
	if(stdin_bufptr){
		stdin_bufptr--;
		return stdin_buf[stdin_bufptr];
	} else return 255;
}

static void renderchar(unsigned char* bitmap, UU p) {
	UU x, y, i, j, _x, _y;
	UU set;
	/*640/8 = 80, 480/8 = 60*/
	_x = p%SCREEN_WIDTH_CHARS;
	_y = p/SCREEN_WIDTH_CHARS;
	_x *= 8;
	_y *= 8;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			set = bitmap[x] & (1 << y);
			if (set) SDL_targ[(_x+y) + (_y+x) * (SCREEN_WIDTH_CHARS * 8)] = vga_palette[FG_color];
		}
	}
}
static void pch(unsigned short a){
	if(a == '\n'){
		curpos += SCREEN_WIDTH_CHARS;
	} else if(a == '\r'){
		while(curpos%SCREEN_WIDTH_CHARS)curpos--;
	} else if(a == 8 || a == 0x7f){
		stdout_buf[curpos-- % (SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS)] = ' ';
	} else if(a == '\t'){
		do{
			stdout_buf[curpos++ % (SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS)] = ' ';
		}while(curpos % 4);
	} else {
		stdout_buf[curpos++ % (SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS)] = a;
	}
	if(curpos>=(SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS)){
		curpos -= SCREEN_WIDTH_CHARS;
		memcpy(stdout_buf, stdout_buf + SCREEN_WIDTH_CHARS, SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS);
		memset(stdout_buf+(SCREEN_WIDTH_CHARS- 1)*SCREEN_HEIGHT_CHARS, ' ', SCREEN_WIDTH_CHARS);
	}
	/*putchar_unlocked(a);*/ /*for those poor terminal users at home.*/
}


#else

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
	setvbuf ( stdout, stdout_buf, _IOFBF, sizeof(stdout_buf));
}
static void dcl(){}
#else
static void di(){return;}
static void dcl(){return;}
#endif

#endif



/*
	non-sdl2 variants of 
*/
#ifndef USE_SDL2
static unsigned short gch(){
#if defined(USE_TERMIOS)
	return (unsigned short)getchar_unlocked();
#else
	return (unsigned short)getchar();
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
#ifdef USE_SDL2
	if(a == '\n' || a == '\r'){ /*magic values to display the screen.*/
		UU i = 0;
		SDL_Rect screenrect;
		SDL_Rect screenrect2;
		/*
			B holds the process whose memory we want to use.
		*/
		b %= (SISA_MAX_TASKS+1);
		screenrect.x = 0;
		screenrect.y = 0;
		screenrect.w = 8 * SCREEN_WIDTH_CHARS;
		screenrect.h = 8 * SCREEN_HEIGHT_CHARS;
		screenrect2 = screenrect;
		screenrect2.w *= display_scale;
		screenrect2.h *= display_scale;
		for(i=0;i<(64 * SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS);i++){
			unsigned char val = M_SAVER[active_audio_user][0xB00000 + i];
			SDL_targ[i] = vga_palette[val];
		}
		for(i=0;i<(SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS);i++){
			if(stdout_buf[i] && stdout_buf[i] != ' ' && isprint(stdout_buf[i]))
				renderchar(font8x8_basic[stdout_buf[i]], i);
		}
		/*
			TODO:
			render characters.
		*/
		SDL_UpdateTexture(
			sdl_tex,
			NULL,
			SDL_targ, 
			(SCREEN_WIDTH_CHARS*8) * 4
		);
		SDL_RenderCopy(
			sdl_rend, 
			sdl_tex,
			&screenrect,
			&screenrect2
		);
		SDL_RenderPresent(sdl_rend);
		return a;
	}
	if(a == 1){ /*Poll events.*/
		pollevents();
		return shouldquit;
	}
	if(a == 2){ /*Read gamer buttons!!!!*/
		unsigned short retval = 0;
		const unsigned char *state;
		SDL_PumpEvents();
		state = SDL_GetKeyboardState(NULL);
		retval |= 0x1 * (state[SDL_SCANCODE_UP]!=0);
		retval |= 0x2 * (state[SDL_SCANCODE_DOWN]!=0);
		retval |= 0x4 * (state[SDL_SCANCODE_LEFT]!=0);
		retval |= 0x8 * (state[SDL_SCANCODE_RIGHT]!=0);

		retval |= 0x10 * (state[SDL_SCANCODE_RETURN]!=0);
		retval |= 0x20 * (state[SDL_SCANCODE_RSHIFT]!=0);
		retval |= 0x40 * (state[SDL_SCANCODE_Z]!=0);
		retval |= 0x80 * (state[SDL_SCANCODE_X]!=0);

		retval |= 0x100 * (state[SDL_SCANCODE_C]!=0);
		retval |= 0x200 * (state[SDL_SCANCODE_A]!=0);
		retval |= 0x400 * (state[SDL_SCANCODE_S]!=0);
		retval |= 0x800 * (state[SDL_SCANCODE_D]!=0);
		return retval;
	}
	/*TODO: play samples from a buffer.*/
	if(a == 3){
		audio_left = 0xB0000;
		return 1;
	}
	/*kill the audio.*/
	if(a == 4){
		audio_left = 0;
		return 1;
	}
	if(a == 5){
		FG_color = b;
		return 1;
	}
	if(a == 6){
		
		return 1;
	}
	if(a == 7){ /*They want to set the active user for audio.*/
		active_audio_user = b % (SISA_MAX_TASKS+1);
	}
	if(a == 8){
		vga_palette[b&0xff] = RX0 & 0xFFffFF;
	}
#endif

	if(a==0xa||a == 0xd) {
#ifndef USE_SDL2
		fflush(stdout);
		return a;
#endif
	}
	if(a == 0xc){
#ifdef USE_SDL2
		memset(stdout_buf, 0, SCREEN_WIDTH_CHARS * SCREEN_HEIGHT_CHARS);
		curpos = 0;
#endif
		printf("\e[H\e[2J\e[3J");
		return a;
	}

	if(a==0xE000){
#ifdef USE_SDL2
#ifndef SDL2_NO_EMULATE_BLOCKING_INPUT
		blocking_input = 0;
#endif
		return 1;
#else

#ifdef USE_TERMIOS
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);return 1;
#else
		return 0;
#endif
#endif
	}
	if(a==0xE001){
#ifdef USE_SDL2

#ifndef SDL2_NO_EMULATE_BLOCKING_INPUT
		blocking_input = 1;
		return 1;
#else
		return 0;
#endif

#else

#ifdef USE_TERMIOS
		/*int flags = fcntl(STDIN_FILENO, F_GETFL, 0);*/
		fcntl(STDIN_FILENO, F_SETFL, 0);return 1;
#else
		return 0;
#endif

#endif

	}
	if(a == 0xffFF){ /*Perform a memory dump.*/
		unsigned long i,j;
		for(i=0;i<(1<<24)-31;i+=32)
			for(j=i,printf("%s\r\n%06lx|",(i&255)?"":"\r\n~",i);j<i+32;j++)
					printf("%02x%c",M[j],((j+1)%8)?' ':'|');
		return a;
	}
	if(a == 0xFF00){ /*Read char from saved disk.*/
		unsigned char bruh;
		RX0 &= 0x7FffFFff;
		FILE* f = fopen("sisa16.dsk", "rb+");
		if(!f){
			return 0;
		}
		if(fseek(f, RX0, SEEK_SET)){
			return 0;
		}
		bruh = (unsigned char)fgetc(f);
		fclose(f);
		return bruh;
	}
	if(a == 0xFF10){ /*Read 256 bytes from saved disk.*/
		RX0 &= 0x7FffFFff;
		FILE* f = fopen("sisa16.dsk", "rb+");
		if(!f){
			UU i = 0;
			for(i = 0; i < 256; i++){
				M[((UU)b<<8) + i] = 0;
			}
			return 0;
		}
		if(fseek(f, RX0, SEEK_SET)){
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
	if(a == 0xFF01){ /*write char 'b' to saved disk.*/
		FILE* f = fopen("sisa16.dsk", "rb+");
		RX0 &= 0x7FffFFff;
		if(!f){
			f = fopen("sisa16.dsk", "wb");
			if(!f) return 0;
			while((unsigned long)ftell(f) < (unsigned long)RX0) if(EOF == fputc(0, f)) return 0;
			fflush(f);
			fclose(f);
			f = fopen("sisa16.dsk", "rb+");
			if(!f) return 0;
		}
		if(fseek(f, RX0, SEEK_SET)){
			fseek(f, 0, SEEK_END);
			while((unsigned long)ftell(f) < (unsigned long)RX0) if(fputc(0, f) == EOF) return 0;
		}
		fputc(b&0xff, f);
		fclose(f);
		return 1;
	}

	if(a == 0xFF11){ /*write 256 bytes from 'b' to saved disk.*/
		FILE* f = fopen("sisa16.dsk", "rb+");
		RX0 &= 0x7FffFFff;
		if(!f){
			f = fopen("sisa16.dsk", "wb");
			if(!f) return 0;
			if((unsigned long)ftell(f) < (unsigned long)RX0) if(EOF == fputc(0, f)) return 0;
			fflush(f);
			fclose(f);
			f = fopen("sisa16.dsk", "rb+");
			if(!f) return 0;
		}
		if(fseek(f, RX0, SEEK_SET)){
			fseek(f, 0, SEEK_END);
			while((unsigned long)ftell(f) < (unsigned long)RX0) if(EOF == fputc(0, f)) return 0;
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
