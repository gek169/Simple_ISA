#define SISA_DEBUGGER
#include "d.h"
#include "isa.h"
/*
	Textmode emulator for SISA16.
*/
static FILE* F;
static char* filename;
static const unsigned char enable_dis_comments = 1;
#include "instructions.h"
#include "stringutil.h"
#include "disassembler.h"

static unsigned long debugger_run_insns = 0; /*if this is less than one, the debugger is not */
static char is_waiting_until_pc_is_value = 0;
static unsigned short pc_wait_value = 0;
static char is_fresh_start = 1;
static char freedom = 0;
UU sisa_breakpoints[0x10000];
UU n_breakpoints = 0;
UU debugger_setting_displaylines = 30;
char debugger_setting_do_dis = 1;
static u M2[(((UU)1)<<24)];

#define N "\r\n"

static char* read_until_terminator_alloced_modified(FILE* f){
	unsigned char c;
	char* buf;
	unsigned long bcap = 40;
	char* bufold;
	unsigned long blen = 0;
	buf = STRUTIL_ALLOC(40);
	if(!buf) return NULL;
	while(1){
		if(feof(f)){break;}
		c = fgetc(f);
		if(c == '\n' || c=='\r') {break;}
		if(c > 127) continue; /*Pretend it did not happen.*/
		if(c < 8) continue; /*Also didn't happen*/
		putchar(c);
		if(
			c == 127
			|| c==8
		)
		{
			if(blen)blen--;
			continue;
		}
		if(blen == (bcap-1))	/*Grow the buffer.*/
			{
				bcap<<=1;
				bufold = buf;
				buf = STRUTIL_REALLOC(buf, bcap);
				if(!buf){free(bufold); return NULL;}
			}
		buf[blen++] = c;
	}
	buf[blen] = '\0';
	return buf;
}
static char make_breakpoint(UU new_breakpoint){
	unsigned long i = 0;
	new_breakpoint &= 0xffFFff;
	for(; i < n_breakpoints; i++){
		if(sisa_breakpoints[i] == new_breakpoint)
			return 0;
		else if(sisa_breakpoints[i] == (UU)0x1FFffFF){
			sisa_breakpoints[i] = new_breakpoint;
			return 1;
		}
	}
	if(n_breakpoints >= 0xffFF){
		printf("\r\n<Cannot make a breakpoint, there are too many already.\r\n");
	}
	sisa_breakpoints[n_breakpoints++] = new_breakpoint;
	return 1;
}

static char delete_breakpoint(UU breakpoint){
	unsigned long i = 0;
	breakpoint &= 0xffFFff;
	for(; i < n_breakpoints; i++){
		if(sisa_breakpoints[i] == breakpoint){
			sisa_breakpoints[i] = (UU)0x1FFffFF;
			if(i == n_breakpoints-1) n_breakpoints--;
			return 1;
		}
	}
	return 0;
}
static void help(){
		puts(
			N "~~SISA16 Debugger~~"
			N "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
			N "~~Let all that you do be done with love~~"
			N "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
			N "COMMANDS:"
			N "h for [h]elp            | display help."
			N "t for s[t]atus          | display registers and EMULATE_DEPTH from emulate and emulate_seg insns."
			N "m to [m]odify           | modify a register. Syntax: m a 3 will set register a to the value 3. Every editable register has a single character representing it, viewable in the status."
			N "s to [s]tep             | step a single instruction. May provide a number of steps: s 10 will step for 10 insns."
			N "b to set [b]reakpoint   | Set a breakpoint. With no arguments, sets breakpoint at current location. b 0x100ff sets a breakpoint at that insn."
			N "e to d[e]l breakpoint   | Delete a breakpoint. With no arguments, delete breakpoint at current location."
			N "l to [l]ist breakpoint  | Print all breakpoints."
			N "    e 0x100ff deletes a breakpoint at that insn."
			N "u to r[u]n              | Run until breakpoint."
			N "d to [d]isassemble      | disassemble. If no argument, disassembles the next 30 bytes."
			N "    SYNTAX: d 50 will disassemble 50 bytes from the program counter."
			N "    You may also include a location, d 50 0x100 will disassemble 50 bytes from 0x100."
			N "q to [q]uit             | quit debugging."
			N "r to [r]eload           | reload at the current emulation depth. "
			N "g for settin[g]         | change settings. Available:"
			N "    g d 30              | change the number of lines displayed by default when disassembling."
			N "    g i 1               | set whether or not a disassembly should be shown every time the REPL is activated."
		N);
}

void debugger_hook(unsigned short *a,
									unsigned short *b,
									unsigned short *c,
									unsigned short *stack_pointer,
									unsigned short *program_counter,
									unsigned char *program_counter_region,
									UU *RX0,
									UU *RX1,
									UU *RX2,
									UU *RX3
){
	char* line = NULL;
	if(freedom) 
	{	unsigned long i=0;
		if(n_breakpoints == 0) return;
		for(; i < n_breakpoints; i++){
			if(((unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16)) == sisa_breakpoints[i])
				{freedom = 0;
				debugger_run_insns = 0;}
		}
	}
	
	if(is_fresh_start){
		help();
		is_fresh_start = 0;
	}
	
	if(debugger_run_insns)
	{unsigned long i = 0;
		debugger_run_insns--;
		if(n_breakpoints)
			for(; i < n_breakpoints; i++){
				if(((unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16)) == sisa_breakpoints[i])
					{freedom = 0;
					debugger_run_insns = 0;}
			}
		return;
	}
	if(debugger_setting_do_dis){
		disassembler(
			filename, 
			(unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16), 
			3,
			((unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16)) + debugger_setting_displaylines
		);
		printf("\r\n");
	}
	repl_start:

		printf("<region: %lu, pc: 0x%06lx >\n\r", (unsigned long)*program_counter_region, 
													  (unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16)
		);
		if(line)free(line);
		line = NULL;
			line = read_until_terminator_alloced_modified(stdin);
		if(!line){
			puts("\r\n Failed Malloc.");
			for(;EMULATE_DEPTH >0;){EMULATE_DEPTH--;dcl();}
			dcl();
			exit(1);
		}
		if(line[0] > 126 || line[0] < 0) goto repl_start;
		switch(line[0]){
			default: 
			puts("\r\n<unrecognized command>\r\n");
			case 'h':help();goto repl_start;
			case 'g':{
				unsigned long stepper = 1;
				char setting;
				unsigned long mode;
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == 0){
					printf("~~Settings~~\r\n");
					printf("d: 0x%08lx  | The default number of lines to bytes to diassemble ahead.\r\n", (unsigned long)debugger_setting_displaylines);
					printf("i: 0x%08lx  | Should we disassemble at every step?\r\n", (unsigned long)debugger_setting_do_dis);
					goto repl_start;
				}
				setting = line[stepper++];
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == 0){
					printf("Missing Mode.\r\n"); goto repl_start;
				}
				mode = strtoul(line + stepper, 0,0);
				switch(setting){
					default:
						printf("Unknown setting.\r\n");
					goto repl_start;
					case 'd': debugger_setting_displaylines = mode;
					goto repl_start;
					case 'i': debugger_setting_do_dis = mode;
					goto repl_start;
				}
			}
			case 'q':
			for(;EMULATE_DEPTH >0;){EMULATE_DEPTH--;dcl();}
			dcl();
			exit(1);
			case '\0':
			case '\r':
			case '\n':
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			goto repl_start;
			case 'u': freedom = 1; free(line); return;
			case 'd':{
				unsigned long stepper = 1;
				unsigned long insns = debugger_setting_displaylines;
				unsigned long location = (unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16);
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == '\0') goto disassemble_end;
				insns = strtoul(line + stepper, 0,0); /*grab number of insns*/
				for(;!isspace(line[stepper]) && line[stepper];stepper++); /*skip the number*/
				for(;isspace(line[stepper]);stepper++); /*skip spaces.*/
				if(line[stepper] == '\0') goto disassemble_end;
				location = strtoul(line + stepper, 0,0);
				disassemble_end:
				disassembler(
						filename, 
						location, 
						3,
						location + insns
				);
				printf("\r\n");
				goto repl_start;
			}
			case 's':
			{unsigned long stepper = 1;
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == '\0') {
					free(line);
					debugger_run_insns = 1;
					goto repl_end;
				}
				debugger_run_insns = strtoul(line+stepper, 0,0);
				free(line);
				goto repl_end;
			}

			case 'b':
			{
				unsigned long stepper = 1;
				unsigned long location = (unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16);
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == '\0') {
					if(!make_breakpoint(location)){
						printf("\r\nBreakpoint already exists.");
					}
					goto repl_start;
				}
				location = strtoul(line+stepper, 0,0);
				if(!make_breakpoint(location)){
					printf("\r\nBreakpoint already exists.");
				}
				goto repl_start;
			}
			case 'l':{
				unsigned long i = 0;
				for(i = 0; i < n_breakpoints; i++){
					printf("\r\n @: 0x%06lx",sisa_breakpoints[i]);
				}
				goto repl_start;
			}
			case 'e':
			{
				unsigned long stepper = 1;
				unsigned long location = (unsigned long)*program_counter + (((unsigned long)*program_counter_region)<<16);
				for(;isspace(line[stepper]);stepper++);
				if(line[stepper] == '\0') {
					if(!delete_breakpoint(location)){
						printf("\r\nBreakpoint already exists.");
					}
					goto repl_start;
				}
				location = strtoul(line+stepper, 0,0);
				if(!delete_breakpoint(location)){
					printf("\r\nBreakpoint already exists.");
				}
				goto repl_start;
			}
			case 'm':
			{
				unsigned long stepper = 1;
				char to_edit = 'A';
				UU value = 0;
			 	for(;isspace(line[stepper]);stepper++);
			 	if(line[stepper] == '\0') 
			 	{
			 		puts("\r\n<Syntax Error, no register to modify.\r\n");
			 		goto repl_start;
			 	}
			 	to_edit = line[stepper++];
			 	for(;isspace(line[stepper]);stepper++);
			 	if(line[stepper] == '\0') 
			 	{
			 		puts("\r\n<Clearing register to zero>\r\n");
			 		value = 0;
			 		goto repl_start;
			 	} else
			 		value = strtoul(line + stepper, 0,0);
			 	switch(to_edit){
			 		case 'a':
			 		case 'A':
			 			*a = value; 
			 		break;
			 		case 'b':
			 		case 'B':
			 			*b = value; 
			 		break;
			 		case 'c':
			 		case 'C':
			 			*c = value; 
			 		break;
			 		case 's':
			 		case 'S':
			 			*stack_pointer = value; 
			 		break;
			 		case 'p':
			 		case 'P':
			 			*program_counter = value; 
			 		break;
			 		case 'r':
			 		case 'R':
			 			*program_counter_region = value; 
			 		break;
			 		case '0':
			 			*RX0 = value; 
			 		break;
			 		case '1':
			 			*RX1 = value; 
			 		break;
					case '2':
			 			*RX2 = value; 
			 		break;
					case '3':
			 			*RX3 = value; 
			 		break;
			 		default:
			 			puts("\r\n<Error, no such register conforming to this name. the names are A,B,C,S,P,R,0,1,2,3");
			 		
			 	}
			 	goto repl_start;
			}
			case 't':
				printf("\r\nFile: %s", filename);
				printf("\r\n~~Registers~~ (Highlighted characters)");
				printf("\r\n[A]         = 0x%04lx", (unsigned long)*a);
				printf("\r\n[B]         = 0x%04lx", (unsigned long)*b);
				printf("\r\n[C]         = 0x%04lx", (unsigned long)*c);
				printf("\r\n[S]TP       = 0x%04lx", (unsigned long)*stack_pointer);
				printf("\r\n[P]C        = 0x%04lx", (unsigned long)*program_counter);
				printf("\r\nPC [R]EGION = 0x%02lx", (unsigned long)*program_counter_region);
				printf("\r\nRX[0]       = 0x%08lx", (unsigned long)*RX0);
				printf("\r\nRX[1]       = 0x%08lx", (unsigned long)*RX1);
				printf("\r\nRX[2]       = 0x%08lx", (unsigned long)*RX2);
				printf("\r\nRX[3]       = 0x%08lx", (unsigned long)*RX3);
				printf("\r\nEMU DEPTH   = 0x%02lx", (unsigned long)EMULATE_DEPTH);
				printf("\r\nSEG PAGES   = 0x%08lx", (unsigned long)SEGMENT_PAGES);
			goto repl_start;
			case 'r':
				memcpy(M, M2, sizeof(M));
				*a = 0;
				*b = 0;
				*c = 0;
				R=0;
				*program_counter_region = 0;
				*program_counter = 0;
				*RX0 = 0;
				*RX1 = 0;
				*RX2 = 0;
				*RX3 = 0;
				if(SEGMENT)free(SEGMENT);
				SEGMENT = calloc(1, 256);
				if(!SEGMENT) {puts("\r\n Failed Malloc.");exit(1);}
				SEGMENT_PAGES = 1;
				is_fresh_start = 1;
			goto repl_start;
		}
		repl_end: return;
}
int main(int rc,char**rv){
	UU i , j=~(UU)0;
	SUU q_test = (SUU)-1;
	/*M = malloc((((UU)1)<<24));*/
	
	if(
		(sizeof(U) != 2) ||
		(sizeof(u) != 1) ||
		(sizeof(UU) != 4)||
		(sizeof(SUU) != 4)
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
		if(sizeof(SUU) != 4)
			puts("SUU is not 4 bytes. Try toggling -DUSE_UNSIGNED_INT. the default is to use int as SUU.");
#ifndef NO_FP
		if(sizeof(float) != 4){
			puts("float is not 4 bytes. Disable the floating point unit during compilation, -DNO_FP");
		}
#endif
		return 1;
	}
#if !defined(NO_SIGNED_DIV)
	memcpy(&i,&q_test, sizeof(UU));
	if(i != j){
		puts("<COMPILETIME ENVIRONMENT ERROR> This is not a two's complement architecture. You must define NO_SIGNED_DIV");
		exit(1);
	}
	j = (UU)0x80000000;
	q_test = -2147483648;
	memcpy(&i,&q_test, sizeof(UU));
	if(i != j){
		puts("<COMPILETIME ENVIRONMENT ERROR> This is not a -conformant- two's complement architecture. It appears the sign bit is not the highest bit.\nYou must define NO_SIGNED_DIV");
		exit(1);
	}
#endif
	if(rc<2){
			puts("SISA-16 Standalone Debugger written by D[MHS]W for the Public Domain");
			puts("This program is Free Software that respects your freedom, you may trade it as you wish.");
			puts("\n\nSISA-16 Macro Assembler, Disassembler, Debugger, and Emulator in Pure Glorious ANSI/ISO C90, Version " SISA_VERSION);
			puts("\"Let all that you do be done with love\"");
			puts("Authored by DMHSW for the Public Domain. Enjoy.\n\n");
			puts("Developer's original repository: https://github.com/gek169/Simple_ISA.git");
			puts("Please submit bug reports and... leave a star if you like the project! Every issue will be read.");
			puts("Programmer Documentation for this virtual machine is provided in the provided manpage sisa16_asm.1");
			puts("~~COMPILETIME ENVIRONMENT INFORMATION~~");
#if defined(NO_SEGMENT)
			puts("The segment was disabled during compilation. Emulate is also disabled.");
#else
			puts("The segment is enabled, so is Emulate.");
#endif

#if defined(NO_FP)
			puts("Floating point unit was disabled during compilation. Float ops generate error code 8.");
#else
			puts("Floating point unit was enabled during compilation. You may use fltadd, fltsub, fltmul, fltdiv, and fltcmp");
			if(sizeof(float) != 4) puts("Floats are an incorrect size. You may not use the floating point unit. Disable the floating point unit.");
#endif
#if defined(NO_SIGNED_DIV)
			puts("32 bit signed integer division instructions were disabled during compilation, they generate error code 10.");
			puts("You can manually implement signed twos-complement integer division.");
#else
			puts("32 bit signed integer division instructions were enabled during compilation. Don't divide by zero!");
#endif
			printf("Size of u is %u, it should be 1, any other result is UB.\n", (unsigned int)sizeof(u));
			printf("Size of U is %u, it should be 2, any other result is UB.\n", (unsigned int)sizeof(U));
			printf("Size of UU is %u, it should be 4, any other result is UB.\n", (unsigned int)sizeof(UU));
			printf("Size of SUU is %u, it should be 4, any other result is UB.\n", (unsigned int)sizeof(SUU));
#if !defined(NO_FP)
			printf("Size of float is %u, it should be 4, any other result is UB.\n", (unsigned int)sizeof(float));
#endif

#ifdef __STDC_IEC_559__
#if __STDC_IEC_559__ == 0
		puts("Floating point compatibility of the environment is specifically not guaranteed.");
#else
		puts("Floating point compatibility of the environment is specifically guaranteed.");
#endif

#else
		puts("the STDC_IEC_559 define is not defined. Standard behavior is not guaranteed.");
#endif
		if(sizeof(void*) == 2) puts("Compiled for a... 16 bit architecture? This is going to be interesting...");
		if(sizeof(void*) == 4) puts("Compiled for a 32 bit architecture.");
		if(sizeof(void*) == 8) puts("Compiled for a 64 bit architecture.");
#if defined(__i386__) || defined(_M_X86) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(__INTEL__) || defined(__386)
		puts("Compiled for x86");
#endif
#if defined(__ia64__) || defined(_IA64) || defined(__ia64) || defined(_M_IA64) || defined(__itanium__)
		puts("Compiled for Itanium.")
#endif
#if defined(__m68k__) || defined(M68000) || defined(__MC68K__)
		puts("Compiled for the M68k");
#endif
#if defined(__mips__) || defined(__mips) || defined(_MIPS_ISA) || defined(__MIPS__)
		puts("Compiled for MIPS.");
#endif
#if defined(__powerpc__) || defined(__powerpc) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__ppc64__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || defined(__ppc)
		puts("Compiled for PowerPC");
#endif
#if defined(__THW_RS6000)
		puts("Compiled for RS/6000");
#endif
#if defined(__sparc__) || defined(__sparc) || defined(__sparc_v8__) || defined(__sparc_v9__) || defined(__sparcv8) || defined(__sparcv9)
		puts("Compiled for sparc.");
#endif
#if defined(__sh__)
		puts("Compiled for SuperH.");
#endif
#if defined(__370__) || defined(__s390__) || defined(__s390x__) || defined(__zarch__) || defined(__SYSC_ZARCH__)
		puts("Compiled for SystemZ");
#endif
#if defined(_TMS320C2XX) || defined(_TMS320C5X) || defined(_TMS320C6X)
		puts("Compiled for TMS320");
#endif
#if defined(__TMS470__)
		puts("Compiled for TMS470");
#endif
#if defined(__SISA16__)
		puts("Compiled for... This architecture!\nIf you're reading this from the source code, let this be a challenge to you:"
		"\nWrite a C compiler for SISA16 and tell me about it on Github. Then, show me some screenshots.\nI'll give you a special prize.");
		puts("The first person to get sisa16_asm to be self-hosting will win something. If that's you, Submit an issue! https://github.com/gek169/Simple_ISA");
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
		puts("Compiled for x86_64");
#endif
#if defined(__alpha__) || defined(_M_ALPHA)
		puts("Compiled for Alpha.");
#endif
#if defined(__arm__) || defined(_ARM) || defined(_M_ARM)|| defined(_M_ARMT) || defined(__thumb__) || defined(__TARGET_ARCH_ARM)|| defined(__TARGET_ARCH_THUMB)
		puts("Compiled for arm.");
#endif
#if defined(__aarch64__)
		puts("Compiled for aarch64");
#endif
#if defined(__bfin)
		puts("Compiled for blackfin.");
#endif
#if defined(__convex__)
		puts("Compiled for convex.");
#endif
#if defined(__epiphany__)
		puts("Compiled for epiphany.");
#endif
#if defined(__hppa__) || defined(__HPPA__)
		puts("Compiled for hppa.");
#endif

		{
			volatile UU a = 0;
			a = 1;
			if(*((unsigned char*) &a)){
				puts("Executing on a Little-Endian Architecture.");
			} else {
				if(*(((unsigned char*)(&a))+2))
					puts("Executing on a PDP-Endian Architecture... That's pretty strange.");
				else if(*(((unsigned char*)(&a))+1))
					puts("Executing on an unknown Middle-Endian Architecture... How Unusual.");
				else
					puts("Executing on a Big-Endian Architecture.");
			}
		}
#if defined(WIN32) || defined(_WIN32)
			puts("Compiled for Macrohard Doors.");
#endif
#if defined(__unix__)
			puts("Targetting some Nix-like OS.");	
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined (_linux) || defined(_LINUX) || defined(__LINUX__)
			puts("Targetting Linux. Free Software Is Freedom.");
#endif
#if defined(__MINGW32__)
			puts("Compiler self-identifying as MINGW.");
#endif
#if defined(__TenDRA__)
			puts("Compiled with TenDRA. Gotta say, that's a cool name for a compiler.");
			return 0;
#endif

#ifdef __INTEL_LLVM_COMPILER
			puts("Compiled with Intel LLVM. Duopoly Inside.");
			return 0;
#endif

#ifdef __cilk
			puts("Compiled with Cilk. Duopoly inside.");
			return 0;
#endif


#ifdef __TINYC__
			puts("Compiled with TinyCC. All Respects to F. Bellard and Crew~~ Try TinyGL! https://github.com/C-Chads/tinygl/");
			return 0;
#endif

#ifdef _MSVC_VER
			puts("Gah! You didn't really compile my beautiful software with that disgusting compiler?");
			puts("MSVC is the worst C compiler on earth. No, not just because Microsoft wrote it. They make *some* good products.");
			puts("The bytecode is horrible and the compiler is uncooperative and buggy.");
			puts("Herb Sutter can go suck an egg! He's trying to kill C++.");
			return 0;
#endif

#ifdef __SDCC
			puts("Compiled with SDCC. Please leave feedback on github about your experiences compiling this with SDCC, I'd like to know.");
			return 0;
#endif

#ifdef __clang__
			puts("Compiled with Clang. In my testing, GCC compiles this project faster *and* produces faster x86_64 code.");
			return 0;
#endif

#ifdef __GNUC__
			puts("Compiled with GCC. Free Software Is Freedom.");
			return 0;
#endif
			puts("The C compiler does not expose itself to be one of the ones recognized by this program. Please tell me on Github what you used.");
			return 0;
	}
	F=fopen(rv[1],"rb");
	if(!F){
		puts("SISA16 debugger cannot open this file.");
		exit(1);
	}
	filename = rv[1];
		for(i=0;i<0x1000000 && !feof(F);){M[i++]=fgetc(F);}
		memcpy(M2, M, sizeof(M));
	fclose(F);
#if !defined(NO_SEGMENT)
	{
		SEGMENT = calloc(1,256);
		SEGMENT_PAGES = 1;
	}
	if(!SEGMENT){puts("Segment Allocation Failed.");return 1;}
#endif
	R=0;

	/*
	*/
	e();
	puts("\r\nExecution Finished normally.\r\n");
	if(R==0)puts("\r\nNo Errors Encountered.\r\n");
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
#if defined(NO_FP)
	if(R==13)
		{puts("\n<Errfl, Either signed division or the FPU were disabled during compilation.>\n");
		R=0;}
	if(R==8)puts("\n<Errfl, Floating point unit disabled by compiletime options>\n");
#else
	if(R==8)puts("\n<Errfl, Internal error, reporting broken SISA16 FPU. Report this bug! https://github.com/gek169/Simple_ISA/  >\n");
#endif

	if(R==9)puts("\n<Errfl, Floating point divide by zero>\n");
#if defined(NO_SIGNED_DIV)
	if(R==13)
		{puts("\n<Errfl, Either signed division or the FPU were disabled during compilation.>\n");
		R=0;}
	if(R==10)puts("\n<Errfl, Signed 32 bit division disabled by compiletime options>\n");
#else
	if(R==10)puts("\n<Errfl, Internal error, reporting broken SISA16 signed integer division module. Report this bug! https://github.com/gek169/Simple_ISA/  >\n");
#endif
	if(R==11)puts("\n<Errfl, Sandboxing limit reached >\n");
	if(R==12)puts("\n<Errfl, Sandboxing could not allocate needed memory.>\n");
	if(R==13)
		{
			puts("\n<Errfl, Internal error, Broken Float-Int Interop. Report this bug! https://github.com/gek169/Simple_ISA/  >\n");
			R=0;
		}
	if(R==14){
#if defined(NO_SEGMENT)
		puts("\n<Errfl, Segment Disabled>");
#else
		puts("\n<Errfl, Internal error, Reporting segment disabled but not set that way at compiletime. Report this bug! https://github.com/gek169/Simple_ISA/   >");
#endif
	}
}
