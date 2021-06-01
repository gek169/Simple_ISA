#include <string.h>
#include <time.h>
typedef unsigned char u;
typedef unsigned short U;
#if defined(USE_UNSIGNED_INT)
typedef unsigned UU;
typedef signed SUU;
#else
typedef unsigned long UU;
typedef long SUU;
#endif
static u R=0, M[(((UU)1)<<24)], *SEGMENT;
static UU SEGMENT_PAGES=0;
