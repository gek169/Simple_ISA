#include <stdio.h>
#define z return
#define k case
#define o goto
#define G S.M[S.p++]
#define r(d) S.M[d]
#define Z (S.p+=2,((((U)S.M[S.p-2]))<<8)+(U)S.M[S.p-1])
#define Z2 (((((U)S.M[S.c]))<<8)+(U)S.M[S.c+1])
#define Z3 (((((U)S.M[S.a]))<<8)+(U)S.M[S.a+1])
#define Z4 (((((U)S.M[S.b]))<<8)+(U)S.M[S.b+1])
#define w(v,d)S.M[d]=v;
#define D ;switch(G&63){k 0:o h;k 1:o A;k 2:o v;k 3:o B;k 4:o V;k 5:o X;k 6: o t;k 7: o T;\
k 8:o oa;k 9:o os;k 10:o om;k 11:o od;k 12:o oi;k 13: o C;k 14:o j;k 15:o J;\
k 16:o Q0;k 17:o Q1;k 18:o Q2;k 19:o Q3;k 20:o Q4;k 21:o Q5;k 22:o Q6;k 23:o Q7;\
k 24:o Q8;k 25:o Q9;k 26:o QA;k 27:o QB;k 28:o QC;k 29:o QD;k 30:o QE;k 31:o QF;\
k 32:o R0;k 33:o R1;k 34:o R2;k 35:o R3;k 36:o R4;k 37:o R5;k 38:o R6;k 39:o R7;\
k 40:o R8;k 41:o R9;k 42:o RA;k 43:o RB;k 44:o RC;k 45:o RD;k 46:o RE;k 47:o RF;\
k 48:o S0;k 49:o S1;k 50:o S2;k 51:o S3;k 52:o S4;k 53:o S5;k 54:o S6;k 55:o S7;\
k 56:o S8;k 57:o S9;k 58:o SA;k 59:o SB;k 60:o SC;k 61:o SD;k 62:o SE;k 63:o SF;\
}
typedef unsigned char u;typedef unsigned short U;struct{U a;U b;U c;U p;u R;u M[(1<<16)];}S;FILE*F;
extern void pch(U mq);extern U gch();extern void di();extern void dcl();
U e(){S.R=0;S.p=0;S.a=0;S.b=0;di();
QE:D
Q2:S.a&=S.b;D
Q3:S.a|=S.b;D
Q4:S.a^=S.b;D
Q0:S.a=gch()D
Q1:pch(S.a)D
Q5:S.a<<=S.b&15;D
Q6:S.a>>=S.b&15;D
Q7:S.a=r(S.c)D
Q8:S.b=r(S.c)D
Q9:S.c=(((U)S.a&255)<<8)+(U)(S.b&255);D
QA:S.a=S.b;D
QB:S.b=S.a;D
QC:S.a=S.c&0xff;D
QD:S.a=(S.c>>8)&255;D
QF:S.c=(((U)S.b&255)<<8)+(U)(S.a&255);D
R0:S.a=Z;D
R1:S.a=Z2;D
R2:S.b=Z;D
R3:S.b=Z2;D
R4:S.a=Z3;D
R5:S.b=Z4;D
R6:S.a=Z4;D
R7:S.b=Z3;D
R8:S.c=S.a;D
R9:S.c=S.b;D
RA:S.a=S.c;D
RB:S.b=S.c;D
RC:w(S.a,S.c)D
RD:w(S.b,S.c)D
RE:w(S.a/256,S.c);w(S.a&255,S.c)D
RF:w(S.b/256,S.c);w(S.b&255,S.c)D
S0:S.c=S.p;D
S1:S2:S3:S4:S5:S6:S7:S8:S9:SA:SB:SC:SD:SE:SF:h:dcl();z 0;
A:S.a=r(Z)D
v:S.a=G;D
B:S.b=r(Z)D
V:S.b=G;D
X:S.c=Z;D
t:w(S.a,Z)D
T:w(S.b,Z)D
j:if(S.a==1)S.p=S.c;D
J:if(S.a!=1)S.p=S.c;D
oa:S.a+=S.b;D
os:S.a-=S.b;D
om:S.a*=S.b;D
od:if(S.b!=0)S.a/=S.b;else{S.R=1;o h;}D
oi:if(S.b!=0)S.a%=S.b;else{S.R=1;o h;}D
C:if(S.a<S.b)S.a=0;else if(S.a>S.b)S.a=2;else S.a=1;D
}int main(int rc,char**rv){U i=0,j;if(rc<2)z 1;
F=fopen(rv[1],"rb");for(;!feof(F);)S.M[i++]=fgetc(F);
for(i=e();i<65504&&rc>2;i+=32)for(j=i,printf("\n%04x|",i);j<i+32;j++)printf("%02x%c",S.M[j],((j+1)%8)?' ':'|');
if(S.R)puts("\n<Errfl>\n");}
