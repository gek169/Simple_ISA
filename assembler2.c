#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "isa_pre.h"
unsigned char line[0x10000] = {0}; /*string buffer. Would be located in main memory.*/
/*
	standin for SISA16's SEG_REALLOC.
*/

char istext(unsigned char c){
	return isalpha(c) || (c == '_')
		;
}
char isvarvalid(unsigned char c){
	return isalpha(c) || (c == '_') || isdigit(c);
}

static unsigned char* read_until_terminator_alloced_modified(FILE* f, unsigned long* lenout){
	char c;
	unsigned char* const buf = line;
	unsigned long bcap = 0x10000;
	unsigned long blen = 0;

	local_top_1:;
	{
		
		if(feof(f)){goto local_end_1;}
		c = fgetc(f);
		if(c == '\n') {goto local_end_1;}
		if(c == '\r') {goto local_end_1;}
		if(blen == (bcap-1))	/*Grow the buffer.*/
			{
				exit(1);
			}
		buf[blen++] = c;
		goto local_top_1;

		local_end_1:;
	}
	
	buf[blen] = '\0';
	*lenout = blen;
	return buf;
}

typedef struct{
	u type; 
	/*
		0 = byte		Promotion is Downward, exclusively
		1 = u16
		2 = s16
		3 = u32
		4 = s32
		5 = f32
	*/
	u region;
	U laddr;
	U len; /*For arrays. 1 = normal variable*/
	char name[128];
}vardef;
vardef variables[1000000];
UU nvars = 0;
UU codeaddr=0; /*Where does code go?*/
UU varaddr =0x100000; /*Where do variables go?*/
vardef q;
UU i;
UU i2;
UU findvar(char* name){
	for(i2 = 0; i2 < nvars; i2++) if(strcmp(name, variables[i2].name) == 0) return i2;
	return nvars + 1;
}
UU is_keyword(char* name){
	if(strcmp(name, "setcodeaddr") == 0) return 1;
	if(strcmp(name, "setvaraddr") == 0) return 1;
	if(strcmp(name, "def") == 0) return 1;
	if(strcmp(name, "fun") == 0) return 1;
	if(strcmp(name, "if") == 0) return 1;
	if(strcmp(name, "while") == 0) return 1;
	if(strcmp(name, "goto") == 0) return 1;
	if(strcmp(name, "label") == 0) return 1;
	if(strcmp(name, "u8") == 0) return 1;
	if(strcmp(name, "i16") == 0) return 1;
	if(strcmp(name, "u16") == 0) return 1;
	if(strcmp(name, "i32") == 0) return 1;
	if(strcmp(name, "u32") == 0) return 1;
	if(strcmp(name, "f32") == 0) return 1;
	if(strcmp(name, "rega") == 0) return 1;
	if(strcmp(name, "regb") == 0) return 1;
	if(strcmp(name, "regc") == 0) return 1;
	if(strcmp(name, "reg0") == 0) return 1;
	if(strcmp(name, "reg1") == 0) return 1;
	if(strcmp(name, "reg2") == 0) return 1;
	if(strcmp(name, "reg3") == 0) return 1;
	if(strcmp(name, "load") == 0) return 1;
	if(strcmp(name, "store") == 0) return 1;
	if(strcmp(name, "push") == 0) return 1;
	if(strcmp(name, "pop") == 0) return 1;
	return 0;
}
UU pushVar(u t, UU len, char* name){
	if(len == 0) return 0;
	if(t > 5) return 0;
	if(t == 0) {if(len > 65536) return 0;}
	else if(t == 1 || t==2) {if(len >= 32768) return 0;}
	else if(t == 3 || t == 4 || t == 5) {if(len >= 16384) return 0;}
	if(strlen(name) > 127) return 0;
	if(strlen(name) < 1) return 0;
	if(!istext(*name)) return 0;
	q.type = t;
	q.region = varaddr / 0x10000;
	q.laddr = varaddr & 0xffFF;
	q.len = len;
	
	strcpy(q.name, name);
	for(i = 0; i < 128; i++) if(!isvarvalid(q.name[i])) q.name[i] = 0;
	if(strlen(q.name) == 0) return 0;
	if(findvar(q.name) != (nvars + 1)) return 0;

	/*Push variable allocation ahead*/
	if(t == 0)
		varaddr += len;
	else if(t == 1 || t == 2)
		varaddr += len * 2;
	else if(t == 3 || t == 4 || t == 5)
		varaddr += len * 4;
	variables[nvars++]=q;return 1;
}





int main(){
	unsigned char begin_char;
	unsigned long lenout;
	unsigned char* buf;
	
	/*
		The program is received via stdin and is parsed as we go along.
	*/
	begin_line:;
	read_until_terminator_alloced_modified(stdin, &lenout);
	buf = line;
	while(*buf == ' ' || *buf == '\t' || *buf == '\v') buf++; /*Skip preceding whitespace*/
	begin_char = line[0];
	if(begin_char == 0xff) return 0;
	if(begin_char == '$') goto dump_line;
	if(begin_char == 0) goto begin_line;
	/*BASIC PROCESSING!!!*/
	return 0; /*TODO*/

	goto begin_line;
	/*It was an assembly line!*/
	dump_line:puts((char*)line+1);goto begin_line;
	
	return 0;
}
