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
static UU SEGMENT_PAGES=0;


/*This is important later!!! Only two privilege levels.
0:		Kernel mode
1:		User mode

User mode is pre-empted.
*/

#ifdef NO_EMULATE
#define NO_PREEMPT
#define NO_DEVICE_PRIVILEGE
#endif

#ifndef SISA_MAX_TASKS
#define SISA_MAX_TASKS 4
#endif

static u M_SAVER[2][0x1000000] = {0};

typedef struct {
	UU RX0,RX1,RX2,RX3;
	U a,b,c,program_counter,stack_pointer;
	u program_counter_region;
}sisa_regfile;

static sisa_regfile REG_SAVER[2] = {0};

static u M_SAVER_TASK_BUF[SISA_MAX_TASKS][0x1000000] = {0};
static sisa_regfile REG_SAVER_TASK_BUF[SISA_MAX_TASKS] = {0};
#define SAVE_REGISTER(XX, d) REG_SAVER[d].XX = XX;
#define LOAD_REGISTER(XX, d) XX = REG_SAVER[d].XX;

#ifdef ATTRIB_NOINLINE
#define DONT_WANT_TO_INLINE_THIS __attribute__ ((noinline))
#else
#define DONT_WANT_TO_INLINE_THIS /*A comment.*/
#endif

