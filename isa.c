#include <string.h>
#include <stdio.h>
typedef unsigned long UU;typedef unsigned char u;typedef unsigned short U;u R,M[(1<<24)];FILE*F;
#include "d.h"
#define z return
#define k case
#define o goto
#define PP ((UU)(pp<<16))
#define G M[PP+p++]
#define r(d)M[d]
#define Z (p+=2,((((U)M[PP+(p-2)]))<<8)+(U)M[PP+(p-1)])
#define Z2 ((((U)M[c])<<8)+(U)M[c+1])
#define Z3 ((((U)M[a])<<8)+(U)M[a+1])
#define Z4 ((((U)M[b])<<8)+(U)M[b+1])
#define ZR (P-=2,(((U)M[P])<<8)+(U)M[P+1])
#define Z2F ((((U)M[(((UU)c&255)<<16)+((UU)b)])<<8)+(U)M[(((UU)c&255)<<16)+(UU)(b+1)])
#define Z3F ((((U)M[(((UU)c&255)<<16)+((UU)a)])<<8)+(U)M[(((UU)c&255)<<16)+(UU)(a+1)])
#define w(v,d)M[d]=v;
#define W(v,d)M[d]=v>>8;M[d+1]=v&255;
#define D ;switch(G){k 0:o h;k 1:o A;k 2:o v;k 3:o B;k 4:o V;k 5:o X;k 6:o t;k 7:o T;\
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
k 128:k 129:k 130:k 131:k 132:k 133:k 134:k 135:k 136:k 137:\
k 138:k 139:k 140:k 141:k 142:k 143:k 144:k 145:k 146:k 147:\
k 148:k 149:k 150:k 151:k 152:k 153:k 154:k 155:k 156:k 157:\
k 158:k 159:k 160:k 161:k 162:k 163:k 164:k 165:k 166:k 167:\
k 168:k 169:k 170:k 171:k 172:k 173:k 174:k 175:k 176:k 177:\
k 178:k 179:k 180:k 181:k 182:k 183:k 184:k 185:k 186:k 187:\
k 188:k 189:k 190:k 191:k 192:k 193:k 194:k 195:k 196:k 197:\
k 198:k 199:k 200:k 201:k 202:k 203:k 204:k 205:k 206:k 207:\
k 208:k 209:k 210:k 211:k 212:k 213:k 214:k 215:k 216:k 217:\
k 218:k 219:k 220:k 221:k 222:k 223:k 224:k 225:k 226:k 227:\
k 228:k 229:k 230:k 231:k 232:k 233:k 234:k 235:k 236:k 237:\
k 238:k 239:k 240:k 241:k 242:k 243:k 244:k 245:k 246:k 247:\
k 248:k 249:k 250:k 251:k 252:k 253:k 254:k 255:o h;}
e(){register u pp=0;register U a=0,b=0,c=0,p=0,P=0;R=0;di();QE:D
V5:V6:V7:V8:V9:VA:VB:VC:VD:VE:VF:W0:W1:W2:W3:W4:W5:W6:W7:W8:W9:WA:WB:WC:WD:WE:WF:
h:dcl();z 0;
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
S1:W(a,Z)D
S2:W(b,Z)D
S3:W(c,Z)D
S4:P+=Z;D
S5:P-=Z;D
S6:P+=a;D
S7:P-=a;D
S8:a=P;D
S9:b=P;D
SA:a=~a;D
SB:c=p;D
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
SF:W(a,((((UU)c&255)<<16)+((UU)b)))D
T0:b=Z3F;D
T1:W(b,((((UU)c&255)<<16)+((UU)a)))D
T2:memmove(M+(((UU)a&255)<<8),M+(((UU)c)<<8),256)D
T3:memmove(M+(((UU)c)<<8),M+(((UU)a&255)<<8),256)D
T4:pp=a;D
SC:W(p,P);P+=2;p=c;D
SD:p=ZR;D
T5:W(p,P);P+=2;w(pp,P);P+=1;pp=a;p=c;D
T6:P-=1;pp=r(P);p=ZR;D
T7:a=r((((UU)c&255)<<16)+((UU)b))D
T8:w(a,((((UU)c&255)<<16)+((UU)b)))D
T9:b=r((((UU)c&255)<<16)+((UU)a))D
TA:w(b,((((UU)c&255)<<16)+((UU)a)))D
TB:a=(((UU)a)*((UU)b))>>1;D
TC:a=(((UU)a)*((UU)b))>>2;D
TD:a=(((UU)a)*((UU)b))>>3;D
TE:a=(((UU)a)*((UU)b))>>4;D
TF:a=(((UU)a)*((UU)b))>>5;D
U0:a=(((UU)a)*((UU)b))>>6;D
U1:a=(((UU)a)*((UU)b))>>7;D
U2:a=(((UU)a)*((UU)b))>>8;D
U3:a=(((UU)a)*((UU)b))>>9;D
U4:a=(((UU)a)*((UU)b))>>10;D
U5:a=(((UU)a)*((UU)b))>>11;D
U6:a=(((UU)a)*((UU)b))>>12;D
U7:a=(((UU)a)*((UU)b))>>13;D
U8:a=(((UU)a)*((UU)b))>>14;D
U9:a=(((UU)a)*((UU)b))>>15;D
UA:a=(((UU)a)*((UU)b))>>16;D
UB:W(a,P);P+=2;D
UC:W(b,P);P+=2;D
UD:W(c,P);P+=2;D
UE:w(a,P);P+=1;D
UF:w(b,P);P+=1;D
V0:a=ZR;D
V1:b=ZR;D
V2:c=ZR;D
V3:P-=1;a=r(P)D
V4:P-=1;b=r(P)D
}
main(int rc,char**rv){UU i=0,j;if(rc<2)z 1;
for(F=fopen(rv[1],"rb");!feof(F);){M[i++]=fgetc(F);i&=0xffffff;if(i==0)break;}
for(i=e();i<(1<<24)-31&&rc>2;i+=32)for(j=i,printf("%s\n%04lx|",(i&255)?"":"\n~",i);j<i+32;j++)printf("%02x%c",M[j],((j+1)%8)?' ':'|');
if(R)puts("\n<Errfl>\n");}
