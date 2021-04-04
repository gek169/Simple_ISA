/*
an incredibly simple ISA

8 bit CPU with only a few basic operations.
Instructions are variable in size

halt- end execution (1 byte) (0)
lda- load byte into a (3 bytes) (1)
sa - set a to value (2 bytes) (2)
ldb - load byte into b (3 bytes) (3)
sb - set b to value (2 bytes) (4)
sc- load 2 constant bytes into c (3 bytes)
sta- store a to location (3 bytes) (6)
stb- store b to location (3 bytes) (7)
add- a = a + b (1 byte) (8)
sub- a = a - b (ditto) (9)
mul- a = a * b (10)
div- a = a / b (11)
mod- a = a % b (12)
cmp- if(a<b) a = 0; else if(a>b)a=2; else a=1; (1 byte) (13)
jmpifeq- set program counter to c if a == 1 (1 byte) (14)
jmpifneq- set program counter to c if a is not 1 (1  byte) (15)
Only the low 4 bits of the opcode are read.


no signed arithmetic is provided.

*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
typedef struct{
	uint8_t a; //accumulator
	uint8_t b; //rh op
	uint16_t c; //jmp point
	uint16_t pc; //program counter
	uint8_t error; //Error flag set if division by zero occurs.
	uint8_t *memory;//Memory addressed by system
} m_regfile;
uint8_t main_memory[(1<<16)-1];

//Basic bus functions so you can actually run the ISA definition.
inline uint8_t read(m_regfile* reg, uint16_t addr){return reg->memory[addr];}
inline void write(m_regfile* reg, uint8_t val, uint16_t addr){reg->memory[addr] = val;}

inline void exec(	m_regfile* reg) {
		#define GETB() read(reg, reg->pc++)
		#define GET2B() tmp = ((((uint16_t)read(reg, reg->pc))<<8) + read(reg, reg->pc+1)); reg->pc += 2;
		#define DISPATCH(){\
			switch(GETB()&15){\
				case 0: goto halt;\
				case 1: goto lda;\
				case 2: goto sa;\
				case 3: goto ldb;\
				case 4: goto sb;\
				case 5: goto sc;\
				case 6: goto sta;\
				case 7: goto stb;\
				case 8: goto add;\
				case 9: goto sub;\
				case 10: goto mul;\
				case 11: goto div;\
				case 12: goto mod;\
				case 13: goto cmp;\
				case 14: goto jmpifeq;\
				case 15: goto jmpifneq;\
			}\
			return;/*the compiler should be able to figure out this is unreachable, but...*/\
		}
		reg->error = 0;
		uint16_t tmp;
		DISPATCH()
		halt: return;
		lda: GET2B();reg->a = read(reg, tmp);												DISPATCH()
		sa: reg->a = GETB();																DISPATCH()
		ldb: GET2B();reg->b = read(reg, tmp);												DISPATCH()
		sb: reg->b = GETB();																DISPATCH()
		sc: GET2B();reg->c = tmp;															DISPATCH()
		sta: GET2B();write(reg, reg->a, tmp);												DISPATCH()
		stb: GET2B();write(reg, reg->b, tmp);												DISPATCH()
		add: reg->a = reg->a + reg->b;														DISPATCH()
		sub: reg->a = reg->a - reg->b;														DISPATCH()
		mul: reg->a = reg->a * reg->b;														DISPATCH()
		div: if(reg->b!=0) reg->a = reg->a / reg->b; else {reg->error = 1; goto halt;}		DISPATCH()
		mod: if(reg->b!=0) reg->a = reg->a % reg->b; else {reg->error = 1; goto halt;}		DISPATCH()
		cmp: if(reg->a<reg->b) reg->a = 0; else if(reg->a>reg->b) reg->a=2; else reg->a=1;	DISPATCH()
		jmpifeq: if(reg->a == 1) reg->pc = reg->c; 											DISPATCH()
		jmpifneq: if(reg->a != 1) reg->pc = reg->c; 										DISPATCH()
}

int main(int argc, char** argv){
	(void)argc;
	(void)argv;
	srand(time(NULL));
	for(size_t i = 0; i < 65535; i++)
		{main_memory[i] = 0;}
/*Simple program which adds 1 to location 488 until it is equal to argc.*/
	/*
		This program does this:
		do{
			[488] += 1;
		}while([488] < 99)
	*/
	size_t i = 0;
#define INS() main_memory[i++]
	//Load the value at 488 into a.
	INS() = 1;
	INS() = 488>>8;
	INS() = 488&255;
	//store 1 into b
	INS() = 4;
	INS() = 1;
	//store 0 into c, it is our jump location for the loop.
	//It'd be best practice to put this *above* the loop, but... I don't want to get this wrong?
	INS() = 5;
	INS() = 0;
	INS() = 0;
	//add 1 to a.
	INS() = 8;
	//store a back into location 488
	INS() = 6;
	INS() = 488>>8;
	INS() = 488&255;
	//store the constant 99 into B
	INS() = 4;
	INS() = 99;
	//Compare A and B.
	INS() = 13;
	//Because we are good programmers we are going to 
	//check if A is less than 70, rather than seeing if it's equal to 70
	//Store the constant zero into B, which is the value
	//from CMP that indicates that A is less than 70.
	INS() = 4;
	INS() = 0;	
	//Compare again
	INS() = 13;
	//Jump if a is 1.
	INS() = 14;
	m_regfile reggie;
	reggie.c = 0;
	reggie.pc = 0;
	reggie.memory = main_memory;
	exec(&reggie);
	for(size_t i = 0; i < 65535; i++)
        if(main_memory[i] != 0)
		{printf("Memory %zu is now %u\n", i, main_memory[i]);}
}
