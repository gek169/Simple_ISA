#include "d.h"
#define z return
#define k case
#define o goto
#define G M[p++]
#define r(d) M[d]
#define Z (p+=2,((((U)M[p-2]))<<8)+(U)M[p-1])
#define Z2 ((((U)M[c])<<8)+(U)M[c+1])
#define Z3 ((((U)M[a])<<8)+(U)M[a+1])
#define Z4 ((((U)M[b])<<8)+(U)M[b+1])
#define w(v,d)M[d]=v;
#define W(v,d)M[d]=v/255;M[d+1]=v&255;
#define D ;switch(G&63){default:o h;k 1:o A;k 2:o v;k 3:o B;k 4:o V;k 5:o X;k 6: o t;k 7: o T;\
k 8:o oa;k 9:o os;k 10:o om;k 11:o od;k 12:o oi;k 13: o C;k 14:o j;k 15:o J;\
k 16:o Q0;k 17:o Q1;k 18:o Q2;k 19:o Q3;k 20:o Q4;k 21:o Q5;k 22:o Q6;k 23:o Q7;\
k 24:o Q8;k 25:o Q9;k 26:o QA;k 27:o QB;k 28:o QC;k 29:o QD;k 30:o QE;k 31:o QF;\
k 32:o R0;k 33:o R1;k 34:o R2;k 35:o R3;k 36:o R4;k 37:o R5;k 38:o R6;k 39:o R7;\
k 40:o R8;k 41:o R9;k 42:o RA;k 43:o RB;k 44:o RC;k 45:o RD;k 46:o RE;k 47:o RF;\
k 48:o S0;k 49:o S1;k 50:o S2;k 51:o S3;k 52:o S4;k 53:o S5;k 54:o S6;k 55:o S7;\
k 56:o S8;k 57:o S9;k 58:o SA;k 59:o SB;k 60:o SC;k 61:o SD;k 62:o SE;k 63:o SF;\
}
typedef unsigned char u;typedef unsigned short U;U a,b,c,p,stp;u R,M[(1<<16)];FILE*F;
e(){R=0;p=0;a=0;b=0;stp=0;di();
QE:D
Q2:a&=b;D
Q3:a|=b;D
Q4:a^=b;D
Q0:a=gch()D
Q1:pch(a)D
Q5:a<<=b&15;D
Q6:a>>=b&15;D
Q7:a=r(c)D
Q8:b=r(c)D
Q9:c=((a&255)<<8)+(b&255)D
QA:a=b;D
QB:b=a;D
QC:a=c&0xff;D
QD:a=(c>>8)&255;D
QF:c=((b&255)<<8)+(a&255)D
R0:a=Z;D
R1:a=Z2;D
R2:b=Z;D
R3:b=Z2;D
R4:a=Z3;D
R5:b=Z4;D
R6:a=Z4;D
R7:b=Z3;D
R8:c=a;D
R9:c=b;D
RA:a=c;D
RB:b=c;D
RC:w(a,c)D
RD:w(b,c)D
RE:W(a,c)D
RF:W(b,c)D
S0:p=c;D
S1:W(a,p)p+=2;D
S2:W(b,p)p+=2;D
S3:W(c,p)p+=2;D
S4:stp+=Z;D
S5:stp-=Z;D
S6:stp+=a;D
S7:stp-=a;D
S8:a=stp;D
S9:b=stp;D
SA:a=~a;D
SB:SC:SD:SE:SF:h:dcl();z 0;
A:a=r(Z)D
v:a=G;D
B:b=r(Z)D
V:b=G;D
X:c=Z;D
t:w(a,Z)D
T:w(b,Z)D
j:if(a==1)p=c;D
J:if(a!=1)p=c;D
oa:a+=b;D
os:a-=b;D
om:a*=b;D
od:if(b!=0)a/=b;else{R=1;o h;}D
oi:if(b!=0)a%=b;else{R=1;o h;}D
C:if(a<b)a=0;else if(a>b)a=2;else a=1;D
}main(int rc,char**rv){U i=0,j;if(rc<2)z 1;
for(F=fopen(rv[1],"rb");!feof(F);){M[i++]=fgetc(F);if(i==0)break;}
for(i=e();i<65504&&rc>2;i+=32)for(j=i,printf("\n%04x|",i);j<i+32;j++)printf("%02x%c",M[j],((j+1)%8)?' ':'|');
if(R)puts("\n<Errfl>\n");}
