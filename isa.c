#include <string.h>
#include <stdio.h>
#include "d.h"
#define z return
#define k case
#define o goto
#define G M[p++]
#define r(d) M[d]
#define Z (p+=2,((((U)M[p-2]))<<8)+(U)M[p-1])
#define Z2 ((((U)M[c])<<8)+(U)M[c+1])
#define Z2F ((((U)M[(((UU)c)<<8)+((UU)b&255)])<<8)+(U)M[(((UU)c)<<8)+((UU)b&255)+1])
#define Z3F ((((U)M[(((UU)c)<<8)+((UU)a&255)])<<8)+(U)M[(((UU)c)<<8)+((UU)a&255)+1])
#define Z3 ((((U)M[a])<<8)+(U)M[a+1])
#define Z4 ((((U)M[b])<<8)+(U)M[b+1])
#define ZR ((((U)M[P])<<8)+(U)M[P+1])
#define w(v,d)M[d]=v;
#define W(v,d)M[d]=v/255;M[d+1]=v&255;
#define D ;switch(G&127){k 0:o h;k 1:o A;k 2:o v;k 3:o B;k 4:o V;k 5:o X;k 6:o t;k 7:o T;\
k 8:o oa;k 9:o os;k 10:o om;k 11:o od;k 12:o oi;k 13: o C;k 14:o j;k 15:o J;\
k 16:o Q0;k 17:o Q1;k 18:o Q2;k 19:o Q3;k 20:o Q4;k 21:o Q5;k 22:o Q6;k 23:o Q7;\
k 24:o Q8;k 25:o Q9;k 26:o QA;k 27:o QB;k 28:o QC;k 29:o QD;k 30:o QE;k 31:o QF;\
k 32:o R0;k 33:o R1;k 34:o R2;k 35:o R3;k 36:o R4;k 37:o R5;k 38:o R6;k 39:o R7;\
k 40:o R8;k 41:o R9;k 42:o RA;k 43:o RB;k 44:o RC;k 45:o RD;k 46:o RE;k 47:o RF;\
k 48:o S0;k 49:o S1;k 50:o S2;k 51:o S3;k 52:o S4;k 53:o S5;k 54:o S6;k 55:o S7;\
k 56:o S8;k 57:o S9;k 58:o SA;k 59:o SB;k 60:o SC;k 61:o SD;k 62:o SE;k 63:o SF;\
k 64:o T0;k 65:o T1;k 66:o T2;k 67:o T3;k 68:o T4;k 69:o T5;k 70:o T6;k 71:o T7;\
k 72:o T8;k 73:o T9;k 74:o TA;k 75:o TB;k 76:o TC;k 77:o TD;k 78:o TE;k 79:o TF;\
k 80:o U0;k 81:o U1;k 82:o U2;k 83:o U3;k 84:o U4;k 85:o U5;k 86:o U6;k 87:o U7;\
k 88:o U8;k 89:o U9;k 90:o UA;k 91:o UB;k 92:o UC;k 93:o UD;k 94:o UE;k 95:o UF;\
k 96:o V0;k 97:o V1;k 98:o V2;k 99:o V3;k 100:o V4;k 101:o V5;k 102:o V6;k 103:o V7;\
k 104:o V8;k 105:o V9;k 106:o VA;k 107:o VB;k 108:o VC;k 109:o VD;k 110:o VE;k 111:o VF;\
k 112:o W0;k 113:o W1;k 114:o W2;k 115:o W3;k 116:o W4;k 117:o W5;k 118:o W6;k 119:o W7;\
k 120:o W8;k 121:o W9;k 122:o WA;k 123:o WB;k 124:o WC;k 125:o WD;k 126:o WE;k 127:o WF;\
}
typedef unsigned long UU;typedef unsigned char u;typedef unsigned short U;U a,b,c,p,P;u R,M[(1<<24)];FILE*F;
e(){R=0;p=0;a=0;b=0;P=0;di();QE:D
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
S4:P+=Z;D
S5:P-=Z;D
S6:P+=a;D
S7:P-=a;D
S8:a=P;D
S9:b=P;D
SA:a=~a;D
SB:c=p;D
SC:W((p),P);P+=2;p=c;D
SD:P-=2;p=ZR;D
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
SE:a=Z2F;D
SF:W(a,((((UU)c)<<8)+((UU)b&255)))D
T0:b=Z3F;D
T1:W(b,((((UU)c)<<8)+((UU)a&255)))D
T2:memcpy(M+(((UU)a&255)<<8),M+(((UU)c)<<8),256)D
T3:memcpy(M+(((UU)c)<<8),M+(((UU)a&255)<<8),256)D
T4:T5:T6:T7:T8:T9:TA:TB:TC:TD:TE:TF:U0:U1:U2:U3:U4:U5:U6:U7:U8:U9:UA:UB:UC:UD:UE:UF:V0:V1:V2:V3:V4:V5:V6:V7:V8:V9:VA:VB:VC:VD:VE:VF:W0:W1:W2:W3:W4:W5:W6:W7:W8:W9:WA:WB:WC:WD:WE:WF:
h:dcl();z 0;}
main(int rc,char**rv){UU i=0,j;if(rc<2)z 1;
for(F=fopen(rv[1],"rb");!feof(F);){M[i++]=fgetc(F);i&=0xffffff;if(i==0)break;}
for(i=e();i<(1<<24)-31&&rc>2;i+=32)for(j=i,printf("%s\n%04lx|",(i&255)?"":"\n~",i);j<i+32;j++)printf("%02x%c",M[j],((j+1)%8)?' ':'|');
if(R)puts("\n<Errfl>\n");}
