#include <string.h>
#include <time.h>
typedef unsigned char u;
typedef unsigned short U;
#if defined(USE_UNSIGNED_INT)
typedef unsigned int UU;
typedef signed int SUU;
#else
typedef unsigned long UU;
typedef long SUU;
#endif
static u R=0, *SEGMENT = NULL;

#define SEGMENT_PAGES 0x80000

/*This is important later!!! Only two privilege levels.
0:		Kernel mode
1:		User mode

User mode is pre-empted.
*/

#ifdef NO_EMULATE
#define NO_PREEMPT
#define NO_DEVICE_PRIVILEGE
#endif

/*
	HUGE NOTE:
	you should alter emulation.hasm if you change this constant!!!
	the libc is designed specifically to work with this!
*/
#ifndef SISA_MAX_TASKS
#define SISA_MAX_TASKS 8
#endif



typedef struct {
	UU RX0,RX1,RX2,RX3;
#ifndef NO_PREEMPT
	UU instruction_counter;
#endif
	U a,b,c,program_counter,stack_pointer;
	u program_counter_region;
}sisa_regfile;
static u M_SAVER[1+SISA_MAX_TASKS][0x1000000] = {0};
static u SEGS[1+SISA_MAX_TASKS][SEGMENT_PAGES * 256];
#define SAVE_REGISTER(XX, d) REG_SAVER[d].XX = XX;
#define LOAD_REGISTER(XX, d) XX = REG_SAVER[d].XX;

#ifdef ATTRIB_NOINLINE
#define DONT_WANT_TO_INLINE_THIS __attribute__ ((noinline))
#else
#define DONT_WANT_TO_INLINE_THIS /*A comment.*/
#endif

