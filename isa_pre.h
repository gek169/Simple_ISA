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
#define SISA16_MAX_RECURSION_DEPTH 16
#endif
