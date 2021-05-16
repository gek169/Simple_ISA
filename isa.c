#include <string.h>
#include <stdio.h>
typedef unsigned long UU;typedef unsigned char u;typedef unsigned short U;u R,M[(1<<24)];FILE*F;
#include "d.h"
#define k case
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
#define W(v,d)M[d]=v>>8;M[(d+1)&0xFFffFF]=v&255;
#define D ;switch(G){k 0:goto G_HALT;k 1:goto G_LDA;k 2:goto G_LA;k 3:goto G_LDB;k 4:goto G_LB;k 5:goto G_SC;k 6:goto G_STA;k 7:goto G_STB;\
k 8:goto G_ADD;k 9:goto G_SUB;k 10:goto G_MUL;k 11:goto G_DIV;k 12:goto G_MOD;k 13:goto G_CMP;k 14:goto G_JMPIFEQ;k 15:goto G_JMPIFNEQ;\
k 16:goto G_GETCHAR;k 17:goto G_PUTCHAR;k 18:goto G_AND;k 19:goto G_OR;k 20:goto G_XOR;k 21:goto G_LSHIFT;k 22:goto G_RSHIFT;k 23:goto G_ILDA;\
k 24:goto G_ILDB;k 25:goto G_CAB;k 26:goto G_AB;k 27:goto G_BA;k 28:goto G_ALC;k 29:goto G_AHC;k 30:goto G_NOP;k 31:goto G_CBA;\
k 32:goto G_LLA;k 33:goto G_ILLDA;k 34:goto G_LLB;k 35:goto G_ILLDB;k 36:goto G_ILLDAA;k 37:goto G_ILLDBB;k 38:goto G_ILLDAB;k 39:goto G_ILLDBA;\
k 40:goto G_CA;k 41:goto G_CB;k 42:goto G_AC;k 43:goto G_BC;k 44:goto G_ISTA;k 45:goto G_ISTB;k 46:goto G_ISTLA;k 47:goto G_ISTLB;\
k 48:goto G_JMP;k 49:goto G_STLA;k 50:goto G_STLB;k 51:goto G_STC;k 52:goto G_PUSH;k 53:goto G_POP;k 54:goto G_PUSHA;k 55:goto G_POPA;\
k 56:goto G_ASTP;k 57:goto G_BSTP;k 58:goto G_COMPL;k 59:goto G_CPC;k 60:goto G_CALL;k 61:goto G_RET;k 62:goto G_FARILLDA;k 63:goto G_FARISTLA;\
k 64:goto G_FARILLDB;k 65:goto G_FARISTLB;k 66:goto G_FARPAGEL;k 67:goto G_FARPAGEST;k 68:goto G_LFARPC;k 69:goto G_FARCALL;k 70:goto G_FARRET;k 71:goto G_FARILDA;\
k 72:goto G_FARISTA;k 73:goto G_FARILDB;k 74:goto G_FARISTB;k 75:goto TB;k 76:goto TC;k 77:goto TD;k 78:goto TE;k 79:goto TF;\
k 80:goto U0;k 81:goto U1;k 82:goto U2;k 83:goto U3;k 84:goto U4;k 85:goto U5;k 86:goto U6;k 87:goto U7;\
k 88:goto U8;k 89:goto U9;k 90:goto UA;k 91:goto G_ALPUSH;k 92:goto G_BLPUSH;k 93:goto G_CPUSH;k 94:goto G_APUSH;k 95:goto G_BPUSH;\
k 96:goto G_ALPOP;k 97:goto G_BLPOP;k 98:goto G_CPOP;k 99:goto G_APOP;k 100:goto G_BPOP;k 101:goto V5;k 102:goto V6;k 103:goto V7;\
k 104:goto V8;k 105:goto V9;k 106:goto VA;k 107:goto VB;k 108:goto VC;k 109:goto VD;k 110:goto VE;k 111:goto VF;\
k 112:goto W0;k 113:goto W1;k 114:goto W2;k 115:goto W3;k 116:goto W4;k 117:goto W5;k 118:goto W6;k 119:goto W7;\
k 120:goto W8;k 121:goto W9;k 122:goto WA;k 123:goto WB;k 124:goto WC;k 125:goto WD;k 126:goto WE;k 127:goto WF;\
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
k 248:k 249:k 250:k 251:k 252:k 253:k 254:k 255:goto G_HALT;}
int e(){
	register u pp=0;register U a=0,b=0,c=0,p=0,P=0;R=0;
	di();

	G_NOP:D
V5:V6:V7:V8:V9:VA:VB:VC:VD:VE:VF:W0:W1:W2:W3:W4:W5:W6:W7:W8:W9:WA:WB:WC:WD:WE:WF:
G_HALT:dcl();return 0;
G_AND:a&=b;D
G_OR:a|=b;D
G_XOR:a^=b;D
G_GETCHAR:a=gch()D
G_PUTCHAR:pch(a)D
G_LSHIFT:a<<=b&15;D
G_RSHIFT:a>>=b&15;D
G_ILDA:a=r(c)D
G_ILDB:b=r(c)D
G_CAB:c=((a&255)<<8)+(b&255)D
G_AB:a=b;D
G_BA:b=a;D
G_ALC:a=c&0xff;D
G_AHC:a=(c>>8)&255;D
G_CBA:c=((b&255)<<8)+(a&255)D
G_LLA:a=Z;D
G_ILLDA:a=Z2;D
G_LLB:b=Z;D
G_ILLDB:b=Z2;D
G_ILLDAA:a=Z3;D
G_ILLDBB:b=Z4;D
G_ILLDAB:a=Z4;D
G_ILLDBA:b=Z3;D
G_CA:c=a;D
G_CB:c=b;D
G_AC:a=c;D
G_BC:b=c;D
G_ISTA:w(a,c)D
G_ISTB:w(b,c)D
G_ISTLA:W(a,c)D
G_ISTLB:W(b,c)D
G_JMP:p=c;D
G_STLA:W(a,Z)D
G_STLB:W(b,Z)D
G_STC:W(c,Z)D
G_PUSH:P+=Z;D
G_POP:P-=Z;D
G_PUSHA:P+=a;D
G_POPA:P-=a;D
G_ASTP:a=P;D
G_BSTP:b=P;D
G_COMPL:a=~a;D
G_CPC:c=p;D
G_LDA:a=r(Z)D
G_LA:a=G;D
G_LDB:b=r(Z)D
G_LB:b=G;D
G_SC:c=Z;D
G_STA:w(a,Z)D
G_STB:w(b,Z)D
G_JMPIFEQ:if(a==1)p=c;D
G_JMPIFNEQ:if(a!=1)p=c;D
G_ADD:a+=b;D
G_SUB:a-=b;D
G_MUL:a*=b;D
G_DIV:if(b!=0)a/=b;else{R=1;goto G_HALT;}D
G_MOD:if(b!=0)a%=b;else{R=1;goto G_HALT;}D
G_CMP:if(a<b)a=0;else if(a>b)a=2;else a=1;D
G_FARILLDA:a=Z2F;D
G_FARISTLA:W(a,((((UU)c&255)<<16)+((UU)b)))D
G_FARILLDB:b=Z3F;D
G_FARISTLB:W(b,((((UU)c&255)<<16)+((UU)a)))D
G_FARPAGEL:memmove(M+(((UU)a&255)<<8),M+(((UU)c)<<8),256)D
G_FARPAGEST:memmove(M+(((UU)c)<<8),M+(((UU)a&255)<<8),256)D
G_LFARPC:pp=a;D
G_CALL:W(p,P);P+=2;p=c;D
G_RET:p=ZR;D
G_FARCALL:W(p,P);P+=2;w(pp,P);P+=1;pp=a;p=c;D
G_FARRET:P-=1;pp=r(P);p=ZR;D
G_FARILDA:a=r((((UU)c&255)<<16)+((UU)b))D
G_FARISTA:w(a,((((UU)c&255)<<16)+((UU)b)))D
G_FARILDB:b=r((((UU)c&255)<<16)+((UU)a))D
G_FARISTB:w(b,((((UU)c&255)<<16)+((UU)a)))D
/*fixed point mults*/
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

G_ALPUSH:W(a,P);P+=2;D
G_BLPUSH:W(b,P);P+=2;D
G_CPUSH:W(c,P);P+=2;D
G_APUSH:w(a,P);P+=1;D
G_BPUSH:w(b,P);P+=1;D
G_ALPOP:a=ZR;D
G_BLPOP:b=ZR;D
G_CPOP:c=ZR;D
G_APOP:P-=1;a=r(P)D
G_BPOP:P-=1;b=r(P)D
}
int main(int rc,char**rv){
	UU i=0,j;
	if(rc<2)return 1;
	for(F=fopen(rv[1],"rb");!feof(F);){
		M[i++]=fgetc(F);i&=0xffffff;if(i==0)break;
	}
	fclose(F);
	for(i=e();i<(1<<24)-31&&rc>2;i+=32)	
		for(j=i,printf("%s\n%04lx|",(i&255)?"":"\n~",i);j<i+32;j++)
			printf("%02x%c",M[j],((j+1)%8)?' ':'|');
	if(R)puts("\n<Errfl>\n");
}
