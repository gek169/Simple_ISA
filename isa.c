#include "d.h"
#include "isa.h"
/*
	Textmode emulator for SISA16.
*/
static FILE* F;

int main(int rc,char**rv){
	UU i , j=0;
	/*M = malloc((((UU)1)<<24));*/
	if(
		(sizeof(U) != 2) ||
		(sizeof(u) != 1) ||
		(sizeof(UU) != 4)
#ifndef NO_FP
		|| (sizeof(float) != 4)
#endif
	){
		puts("SISA16 ERROR!!! Compilation Environment conditions unmet.");
		if(sizeof(U) != 2)
			puts("U is not 2 bytes. Try using something other than unsigned short (default).");
		if(sizeof(u) != 1)
			puts("u is not 2 bytes. Try using something other than unsigned char (default).");
		if(sizeof(UU) != 4)
			puts("UU is not 4 bytes. Try toggling -DUSE_UNSIGNED_INT. the default is to use unsigned int as UU.");
#ifndef NO_FP
		if(sizeof(float) != 4){
			puts("float is not 4 bytes. Disable the floating point unit during compilation, -DNO_FP");
		}
#endif
		return 1;
	}
	if(rc<2){
		printf("\nUsage: %s myprogram.bin [additional args if you want memory dump]\n", rv[0]);
		puts("SISA-16 Emulator written by D[MHS]W for the Public Domain");
		puts("This program is Free Software that respects your freedom, you may trade it as you wish.");
		puts("Developer's original repository: https://github.com/gek169/Simple_ISA.git");
		puts("Please submit bug reports and... leave a star if you like the project! Every issue will be read.");
		puts("Programmer Documentation for this virtual machine is provided in the provided manpage sisa16_asm.1");
		puts("This program should have come with a compatible assembler so you can write programs.");
		puts("~~COMPILETIME ENVIRONMENT INFORMATION~~");
#ifdef NO_FP
		puts("Floating point unit was disabled during compilation. Float ops generate error code 8.");
#else
		puts("Floating point unit was enabled during compilation. You may use fltadd, fltsub, fltmul, fltdiv, and fltcmp");
#endif

#ifdef __cilk
		puts("Compiled with ICC. Duopoly inside.");
		return 1;
#endif

#ifdef __clang__
		puts("Compiled with Clang. In my testing, GCC compiles this project faster *and* produces faster x86_64 code.");
		return 1;
#else
#ifdef __GNUC__
		puts("Compiled with GCC. Free Software Is Freedom.");
		return 1;
#endif
#endif
#ifdef __TINYC__
		puts("Compiled with TinyCC. All Respects to F. Bellard and Crew~~ Try TinyGL! https://github.com/C-Chads/tinygl/");
		return 1;
#endif
#ifdef _MSVC_VER
		puts("Gah! You didn't really compile my beautiful software with that disgusting compiler?");
		puts("MSVC is the worst C compiler on earth. No, not just because Microsoft wrote it. They make *some* good products.");
		puts("The bytecode is horrible and the compiler is uncooperative and buggy.");
		puts("Herb Sutter can go suck an egg! He's trying to kill C++.");
		return 1;
#endif
#ifdef __SDCC
		puts("Compiled with SDCC. Please leave feedback on github about your experiences compiling this with SDCC, I'd like to know.");
		return 1;
#endif
		puts("The C compiler does not expose itself to be one of the ones recognized by this program. Please tell me on Github what you used.");
		return 1;
	}
	F=fopen(rv[1],"rb");
	if(!F){
		puts("SISA16 emulator cannot open this file.");
		exit(1);
	}
		for(i=0;i<0x1000000 && !feof(F);){M[i++]=fgetc(F);}
	fclose(F);
	{
		SEGMENT = malloc(0x100);
		SEGMENT_PAGES = 1;
	}
	if(!SEGMENT){puts("Segment Allocation Failed.");return 1;}
	e();
	for(i=0;i<(1<<24)-31&&rc>2;i+=32)	
		for(j=i,printf("%s\n%04lx|",(i&255)?"":"\n~",(unsigned long)i);j<i+32;j++)
			printf("%02x%c",M[j],((j+1)%8)?' ':'|');
	if(R==1)puts("\n<Errfl, 16 bit div by 0>\n");
	if(R==2)puts("\n<Errfl, 16 bit mod by 0>\n");
	if(R==3)puts("\n<Errfl, 32 bit div by 0>\n");
	if(R==4)puts("\n<Errfl, 32 bit mod by 0>\n");
	if(R==5)puts("\n<Errfl, Bad Segment Page>\n");
	if(R==6)puts("\n<Errfl, Segment Cannot be Zero Pages>\n");
	if(R==7)puts("\n<Errfl, Segment Failed Allocation>\n");
#ifdef NO_FP
	if(R==8)puts("\n<Errfl, Floating point unit disabled by compiletime options>\n");
#else
	if(R==8)puts("\n<Errfl, Internal error, reporting broken SISA16 FPU. Report this bug! https://github.com/gek169/Simple_ISA/>\n");
#endif
	if(R==9)puts("\n<Errfl, Floating point divide by zero>\n");
}
