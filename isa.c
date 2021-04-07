#include <stdio.h>
#include <stdint.h>
#define z return
#define k case
#define o goto
#define G r(S.p++)
#define Z (S.p+=2,(((u6)r(S.p-2))<<8)+r(S.p-1))
#define D ;switch(G&31){k 0:o h;k 1:o A;k 2:o v;k 3:o B;k 4:o V;k 5:o X;k 6: o t;k 7: o T;\
						k 8:o oa;k 9:o os;k 10:o om;k 11:o od;k 12:o oi;k 13: o C;k 14:o j;k 15:o J;\
						k 16:o Q0;k 17:o Q1;k 18:o Q2;k 19:o Q3;k 20:o Q4;k 21:o Q5;k 22:o Q6;k 23:o Q7;\
						k 24:o Q8;k 25:o Q9;k 26:o QA;k 27:o QB;k 28:o QC;k 29:o QD;k 30:o QE;k 31:o QF;}
typedef uint8_t u8;typedef uint16_t u6;struct{u8 a;u8 b;u6 c;u6 p;u8 R;u8*y;u8 M[(1<<16)];}S={0};
u8 r(u6 d){z S.M[d];}u8 w(u8 v, u6 d){S.M[d] = v;}
u6 e(){
S.R=0;D
Q2:S.a&=S.b;D Q3:S.a|=S.b;D Q4:S.a^=S.b;D
Q0:S.a=getchar()D Q1:putchar(S.a)D
Q5:S.a<<=S.b&7;D Q6:S.a>>=S.b&7;D
Q7:S.a=r(S.c)D Q8:S.b=r(S.c)D Q9:S.c=(((u6)S.a)<<8)+(u6)S.b;D
QA:S.a=S.b;D QB:S.b=S.a;D QC:S.a=S.c;D QD:S.a=S.c>>8;D
QE:QF:h:z 0;
A:S.a=r(Z)D v:S.a=G;D B: S.b=r(Z)D V:S.b=G;D X: S.c=Z;D t:w(S.a,Z)D
T:w(S.b,Z)D j:if(S.a==1) S.p=S.c;D J:if(S.a!=1) S.p=S.c;D
oa:S.a+=S.b;D os:S.a-=S.b;D om:S.a*=S.b;D
od:if(S.b!=0)S.a/=S.b;else{S.R=1;o h;}D
oi:if(S.b!=0)S.a%=S.b;else{S.R=1;o h;}D
C:if(S.a<S.b)S.a=0;else if(S.a>S.b)S.a=2;else S.a=1;D}
int main(int rc,char**rv){u6 i=0,j;
for(;!feof(stdin);)S.M[i++]=fgetc(stdin);
for(i=e();i<65504&&rc>1;i+=32)for(j=i,printf("\n%04x|",i);j<i+32;j++)printf("%02x%c",S.M[j],((j+1)%8)?' ':'|');}
