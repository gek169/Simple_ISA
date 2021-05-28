#include <string.h>
#include <time.h>
typedef unsigned char u;
typedef unsigned short U;
#ifndef USE_UNSIGNED_INT
typedef unsigned long UU;
#else
typedef unsigned UU;
#endif
static u R=0, *M;
