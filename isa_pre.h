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
static u R=0, M[0x1000000], PTEMP[256], *SEGMENT = NULL;
static UU SEGMENT_PAGES=0, EMULATE_DEPTH=0;

#if !defined(SISA16_MAX_RECURSION_DEPTH)
#define SISA16_MAX_RECURSION_DEPTH 4
#endif

static u M_SAVER[SISA16_MAX_RECURSION_DEPTH][0x1000000];


#ifdef ATTRIB_NOINLINE
#define DONT_WANT_TO_INLINE_THIS __attribute__ ((noinline))
#else
#define DONT_WANT_TO_INLINE_THIS /*A comment.*/
#endif

