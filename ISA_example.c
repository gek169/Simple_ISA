#include <stdio.h>
#include <stdint.h>
#define z return
#define k case
#define o goto
#define E S.
#define G r(E p++)
#define Z (E p+=2,(((u6)r(E p-2))<<8)+r(E p-1))
#define D ;switch(G&15){k 0: o h;k 1: o A;k 2: o v;k 3: o B;k 4: o V;k 5: o X;k 6: o t;k 7: o T;k 8: o oa;k 9: o os;k 10: o om;k 11: o od;k 12: o oi;k 13: o C;k 14: o j;k 15: o J;}
typedef uint8_t u8;typedef uint16_t u6;struct{u8 a;u8 b;u6 c;u6 p;u8 R;u8*y;u8 M[(1<<16)];}S={0};
u8 r(u6 d){z E M[d];}u8 w(u8 v, u6 d){E M[d] = v;}

u6 e(){
E R=0;D
h:z 0;A:E a=r(Z)D v:E a=G;D B: E b=r(Z)D V:E b=G;D X: E c=Z;D t:w(E a, Z)D
T:w(E b, Z)D j:if(E a==1) E p = E c;D J:if(E a!=1) E p=E c;D
oa:E a += E b;D os:E a -= E b;D om:E a *= E b;D
od:if(E b!=0)E a/= E b;else{E R=1;o h;}D
oi:if(E b!=0)E a%= E b;else{E R=1;o h;}D
C: if(E a<E b)E a = 0;else if(E a>E b)E a=2; else E a=1;D
}

int main(){
	u6 i=0,j;for(;!feof(stdin);E M[i++]=fgetc(stdin));
	for(i=e();i<65520;i+=16) for(j=i,printf("\n%04x|", i);j<i+16;j++)printf("%02x%c",E M[j],((j+1)%8)?' ':'|');
}
