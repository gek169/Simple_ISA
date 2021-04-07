/*
an incredibly simple ISA

8 bit CPU with only a few basic operations.
Instructions are variable in size
See README

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
	uint8_t memory[(1<<16)-1];//Memory addressed by system
} m_regfile;


//Basic bus functions so you can actually run the ISA definition.
constexpr static inline uint8_t read(m_regfile* reg, uint16_t addr){return reg->memory[addr];}
constexpr static inline void write(m_regfile* reg, uint8_t val, uint16_t addr){reg->memory[addr] = val;}
constexpr static inline void exec(	m_regfile* reg) {
		#define GETB() read(reg, reg->pc++)
		#define GET2B() tmp = ((((uint16_t)read(reg, reg->pc))<<8) + (uint16_t)read(reg, reg->pc+1)); reg->pc += 2;
		#define DISPATCH(){\
			switch(GETB()&15){\
				case 0: isdone = true; break;\
				case 1: GET2B();reg->a = read(reg, tmp);break;\
				case 2: reg->a = GETB();break;\
				case 3: GET2B();reg->b = read(reg, tmp);break;\
				case 4: reg->b = GETB();break;\
				case 5: GET2B();reg->c = tmp;break;\
				case 6: GET2B();write(reg, reg->a, tmp);break;\
				case 7: GET2B();write(reg, reg->b, tmp);break;\
				case 8: reg->a = reg->a + reg->b;break;\
				case 9: reg->a = reg->a - reg->b;break;\
				case 10: reg->a = reg->a * reg->b;break;\
				case 11: if(reg->b!=0) reg->a = reg->a / reg->b; else {reg->error = 1; return;};break;\
				case 12: if(reg->b!=0) reg->a = reg->a % reg->b; else {reg->error = 1; return;};break;\
				case 13: if(reg->a<reg->b) reg->a = 0; else if(reg->a>reg->b) reg->a=2; else reg->a=1;break;\
				case 14: if(reg->a == 1) reg->pc = reg->c;break;\
				case 15: if(reg->a != 1) reg->pc = reg->c;break;\
			}\
		}
		reg->error = 0;
		bool isdone = false;
		while(!isdone){
			uint16_t tmp = 0;
			DISPATCH();
		}
}

constexpr m_regfile run_program(){
	size_t i = 0;
	m_regfile reggie = {0,0,0,0,0,0};
#define INS() reggie.memory[i++]
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
	exec(&reggie);
	return reggie;
}


int main(){
	run_program();
	constexpr m_regfile result = run_program();
	for(size_t i = 0; i < 65535; i++)
        if(result.memory[i] != 0)
		{printf("Memory %zu is now %u\n", i, result.memory[i]);}
}
