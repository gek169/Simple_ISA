/*Default textmode driver for SISA16.*/
#include <stdio.h>
#include <stdlib.h>


#ifdef USE_SDL2
/*
	SDL2 driver, plus simple text mode.
*/
#include "isa_pre.h"
#define SDL_MAIN_HANDLED
#ifdef __TINYC__
#define SDL_DISABLE_IMMINTRIN_H 1
#endif
#include <SDL2/SDL.h>
static SDL_Window *sdl_win = NULL;
static SDL_Renderer *sdl_rend = NULL;
static SDL_Texture *sdl_tex = NULL;
static SDL_AudioSpec sdl_spec;
static unsigned short audio_pos = 0;
static unsigned short audio_left = 0;
static UU SDL_targ[640*480];
static const UU arne_palette[16] = {
		0x000000,
		0x493c2b,
		0xbe2633,
		0xe06f8b,
		0x9d9d9d,
		0xa46422,
		0xeb8931,
		0xf7e26b,
		0xffffff,
		0x1b2632,
		0x2f484e,
		0x44891a,
		0xa3ce27,
		0x005784,
		0x31a2f2,
		0xb2dcef
};
void sdl_audio_callback(void *udata, Uint8 *stream, int len){
	SDL_memset(stream, 0, len);
	if(audio_left == 0){return;}
	len = (len < audio_left) ? len : audio_left;
	SDL_MixAudio(stream, M + 0xB50000 + audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_left -= len;
}

static void di(){
	if(EMULATE_DEPTH==0){
	    // Initialize SDL
	    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	    {
	        printf("SDL2 could not be initialized!\n"
	               "SDL_Error: %s\n", SDL_GetError());
	        exit(1);
	    }
		sdl_spec.freq = 16000;//Sampling rate
		sdl_spec.format = AUDIO_U8; //Number of data bits
		sdl_spec.channels = 1;//Number of channels
		sdl_spec.silence = 0;
		sdl_spec.samples = 2048;
		sdl_spec.callback = sdl_audio_callback;
		sdl_spec.userdata = NULL;
		sdl_win = SDL_CreateWindow("[Sisa16 Emulation]",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640, 480,
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
		sdl_tex = SDL_CreateTexture(sdl_rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
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
	}
}
static void dcl(){
	if(EMULATE_DEPTH==0){
		SDL_DestroyTexture(sdl_tex);
		SDL_DestroyRenderer(sdl_rend);
		SDL_CloseAudio();
    	SDL_DestroyWindow(sdl_win);
	    SDL_Quit();
	}
}
#else

/*
	textmode driver.
*/
#include "isa_pre.h"

#ifdef USE_NCURSES
#include <ncurses.h>
static void di(){if(EMULATE_DEPTH==0)initscr(); return;}
static void dcl(){if(EMULATE_DEPTH==0)endwin();return;}
#else
#ifdef USE_TERMIOS
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
static struct termios oldChars;
static struct termios newChars;
void initTermios(int echo) //struct termios &oldChars, struct termios &newChars)
{
  /*fcntl(0, F_SETFL, O_NONBLOCK);*/ /*Do you want non-blocking IO? This is how you get non-blocking IO.*/
  tcgetattr(STDIN_FILENO, &oldChars); /* grab old terminal i/o settings */
  newChars = oldChars; /* make new settings same as old settings */
  newChars.c_lflag &= ~ICANON; /* disable buffered i/o */
  newChars.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &newChars); /* use these new terminal i/o settings now */
}
void dieTermios(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldChars); /* use these new terminal i/o settings now */	
}
static void di(){
if(EMULATE_DEPTH==0){
	initTermios(0);
	atexit(dieTermios);
}
}
static void dcl(){
if(EMULATE_DEPTH==0){
	/*We actually just do nothing!*/
}
}
#else
static void di(){return;}
static void dcl(){return;}
#endif
#endif

#endif

static unsigned short gch(){
#if defined(linux) || defined(__linux__) || defined(__linux) || defined (_linux) || defined(_LINUX) || defined(__LINUX__) || defined(__unix__)
return (unsigned short)getchar_unlocked();
#else
return (unsigned short)getchar();
#endif
}
static void pch(unsigned short a){
#if defined(linux) || defined(__linux__) || defined(__linux) || defined (_linux) || defined(_LINUX) || defined(__LINUX__) || defined(__unix__)
putchar_unlocked(a);
#else
putchar(a);
#endif
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

#ifdef USE_SDL2
	if(a == 0){ /*magic value to display the screen.*/
		UU i = 0;
		static SDL_Rect screenrect;
		screenrect.x = 0;
		screenrect.y = 0;
		screenrect.w = 640;
		screenrect.h = 640;
		for(;i<640*480;i++){
			unsigned char val = M[0xB00000 + (i/2)];
			if(i%2){
				val &= 0xf0;
				val >>= 4;
			} else {
				val &= 0xf;
			}
			SDL_targ[i] = arne_palette[val];
		}
		SDL_UpdateTexture(
			sdl_tex,
			NULL,
			SDL_targ, 
			640 * 4
		);
		SDL_RenderCopy(
			sdl_rend, 
			sdl_tex,
			NULL,
			NULL
		);
		SDL_RenderPresent(sdl_rend);
		return 1;
	}
	if(a == 1){ /*check for quit event.*/
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT) return 0xFFff; /*Magic value for quit.*/
		}
		return 0;
	}
	if(a == 2){ /*Read the buttons*/
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
		audio_left = 0xffFF;
		audio_pos = 0;
		return 0;
	}
#endif

	if(a==0xa||a == 0xd) {fflush(stdout);return a;}

	if(a==0xE000){
#ifdef USE_TERMIOS
		/* set O_NONBLOCK on fd */
		int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
		return 1;
#else
		return 0;
#endif
	}
	if(a==0xE001){
#ifdef USE_TERMIOS
		/* set O_NONBLOCK on fd */
		int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
		return 1;
#else
		return 0;
#endif
	}
	if(a == 0xffFF){ /*Perform a memory dump.*/
		unsigned long i,j;
		for(i=0;i<(1<<24)-31;i+=32)
			for(j=i,printf("%s\r\n%04lx|",(i&255)?"":"\r\n~",i);j<i+32;j++)
					printf("%02x%c",M[j],((j+1)%8)?' ':'|');
		return a;
	}
	if(a == 0xFF00){ /*Read char from saved disk.*/
		unsigned short bruh;
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
	if(a == 0xFF01){ /*write char 'b' to saved disk.*/
		FILE* f = fopen("sisa16.dsk", "rb+");
		RX0 &= 0x7FffFFff;
		if(!f){
			f = fopen("sisa16.dsk", "wb");
			if(!f) return 0;
			while((unsigned long)ftell(f) < (unsigned long)RX0) fputc(0, f);	
			fflush(f);
			fclose(f);
			f = fopen("sisa16.dsk", "rb+");
			if(!f) return 0;
		}
		/*
		fseek(f, 0, SEEK_END);
		if((unsigned long)ftell(f) < (unsigned long)RX0){
			while((unsigned long)ftell(f) < (unsigned long)RX0) fputc(0, f);	
			fflush(f);
		}
		*/
		if(fseek(f, RX0, SEEK_SET)){
			fseek(f, 0, SEEK_END);
			while((unsigned long)ftell(f) < (unsigned long)RX0) fputc(0, f);
		}
		fputc(b&0xff, f);
		fclose(f);
		return 1;
	}
	return a;
}
