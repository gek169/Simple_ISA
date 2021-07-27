#include "stringutil.h"
#include <stdio.h>
#include <string.h>
#include "d.h"
#include "isa.h"

static char* outfilename = "outs16.bin";
static const char* fail_msg = "\r\n<ASM> Assembler Aborted.\r\n";
static const char* compil_fail_pref = "<ASM COMPILATION ERROR>";
static const char* syntax_fail_pref = "<ASM SYNTAX ERROR>";
static const char* general_fail_pref = "<ASM ERROR>";
static const char* internal_fail_pref = "<ASM INTERNAL ERROR>";
static const char* warn_pref = "<ASM WARNING>";
static char run_sisa16 = 0;
static char enable_dis_comments = 1;
static char clear_output = 0;
static void ASM_PUTS(const unsigned char* s){if(!clear_output)puts(s);}
static char* infilename = NULL;

static char read_until_terminator_alloced_modified_mode = 0;

void my_strcpy(unsigned char* dest, unsigned char* src){
	while(*src) *dest++ = *src++;
	*dest = 0;
}

static unsigned char line_copy[0x10000] = {0}; /*line_copy*/
static unsigned char line[0x10000] = {0}; /*line_copy*/
static unsigned char buf1[0x10000] = {0}; /*buffer for working with strings.*/
static unsigned char buf2[0x10000] = {0}; /*another buffer for working with strings.*/
static unsigned char buf_repl[0x10000] = {0}; /*another buffer for working with strings, specifically for the following function:*/

int perform_inplace_repl( /*returns whether or not it actually did a replacement.*/
	unsigned char* workbuf,
	unsigned char* replaceme,
	unsigned char* replacewith
){
	long loc = strfind((char*)workbuf, (char*)replaceme);
	if(loc == -1) return 0;
	/*printf("\r\nASKED TO REPLACE %s WITH %s IN %s", replaceme, replacewith, workbuf);*/
	memcpy(buf_repl, workbuf, loc);
	my_strcpy(buf_repl + loc, replacewith);
	my_strcpy(
		buf_repl + loc + strlen((char*)replacewith),
		workbuf + loc + strlen((char*)replaceme)
	);
	my_strcpy(workbuf, buf_repl);
	/*printf("\r\nIS NOW:%s\r\n", workbuf);*/
	return 1;
}

static unsigned char* rut_append_to_me = NULL;
static unsigned char* read_until_terminator_alloced_modified(FILE* f, unsigned long* lenout, char terminator){
	char c;
	unsigned char* const buf = line;
	unsigned long bcap = 0x10000;
	unsigned long blen = 0;
	if(rut_append_to_me){
		blen = strlen((char*)line);
		rut_append_to_me = NULL;
	}

	while(1){
		if(feof(f)){break;}
		c = fgetc(f);
		if(c == terminator) {break;}
		if(blen == (bcap-1))	/*Grow the buffer.*/
			{
				printf(general_fail_pref);
				printf("Oversized line exceeds 128k limit for a line.");
				exit(1);
			}
		buf[blen++] = c;
	}
	buf[blen] = '\0';
	*lenout = blen;
	return buf;
}

#ifndef SISA16_MAX_MACROS
#define SISA16_MAX_MACROS 0x10000
#endif
static unsigned char* variable_names[SISA16_MAX_MACROS] = {0};
static unsigned char* variable_expansions[SISA16_MAX_MACROS] = {0};
static unsigned char variable_is_redefining_flag[SISA16_MAX_MACROS] = {0};
static const unsigned long max_lines_disassembler = 0x1ffFFff;
#include "instructions.h"
static char DONT_WANT_TO_INLINE_THIS int_checker(unsigned char* proc){
	char int_mode = 0; /*starting with 0x means hexidecimal*/
	unsigned long chars_read = 0;
	if(!isdigit(proc[0])) return 1;
	if(proc[0] == '0') {
		int_mode = 1;proc++;
		chars_read++;
	} /*octal*/
	if(*proc && int_mode == 1) {
		if(proc[0] == 'x') {
			int_mode = 2;proc++;
		} else return 0;
		chars_read = 0;
	} /*hex*/
	if(int_mode == 1 && (proc[0] == ',' || proc[0] == ';' || proc[0] == '\0')) return 0; /*zero.*/
	for(;!(proc[0] == ',' || proc[0] == ';' || proc[0] ==/*(*/ ')' || proc[0] == '\0');proc++){
		/*Check hex*/
		if(int_mode==2 && 
		!( (*proc <= '9' && *proc >= '0') || (*proc <= 'F' && *proc >= 'A') || (*proc <= 'f' && *proc >= 'a'))
		){
			return 1; /*they have 0x, but no numbers!*/
		}
		if(int_mode==0 && 
		!(*proc <= '9' && *proc >= '0')){
			return 1;
		}
		if(int_mode==1 && 
		!( (*proc <= '8' && *proc >= '0') )){
			return 1;
		}
		chars_read++;
	}
	if(chars_read == 0) return 1; /*There were no characters in the number.*/
	return 0;
}


static unsigned long outputcounter = 0;
static unsigned long nmacros = 5; /*0,1,2,3,4*/
static char quit_after_macros = 0;
static char debugging = 0;
static unsigned long npasses = 0;
static char printlines = 0;
static unsigned long linesize = 0;
static unsigned long region_restriction = 0;
static char region_restriction_mode = 0; /*0 = off, 1 = block, 2 = region*/
static void DONT_WANT_TO_INLINE_THIS fputbyte(unsigned char b, FILE* f){
	if(!run_sisa16 && !quit_after_macros)
		if((unsigned long)ftell(f) != outputcounter){
			/*seek to the end*/
			fseek(f,0,SEEK_END);
			if((unsigned long)ftell(f) > outputcounter) /*The file is already larger than needed.*/
				fseek(f,outputcounter,SEEK_SET); 
			else /*Fill with 0's until we reach the output counter.*/
				while((unsigned long)ftell(f)!=outputcounter)fputc(0, f);
		}
	switch(region_restriction_mode){
		default: break;
		case 1:{
			if (((outputcounter>>8) & 0xFFFF)  != region_restriction){
				printf(compil_fail_pref);
				printf("page restriction failed. Line:\n%s", line_copy); 
				exit(1);
			}
		}
		break;
		case 2:{
			if (((outputcounter>>16) & 0xFF)  != region_restriction){
				printf(compil_fail_pref);
				printf("region restriction failed. Line:\n%s", line_copy);
				exit(1);
			}
		}
		break;
	}
	if(!quit_after_macros){
		if(!run_sisa16){
			if(npasses == 1)
				fputc(b, f);
		} else {
			if(npasses == 1)
				M_SAVER[0][outputcounter]=b;
		}
	}
	outputcounter++; outputcounter&=0xffffff;
}
static void putshort(unsigned short sh, FILE* f){fputbyte(sh/256, f);fputbyte(sh, f);}
#define ASM_MAX_INCLUDE_LEVEL 20
static FILE* fstack[ASM_MAX_INCLUDE_LEVEL];
/*#include "asm_expr_parser.h"*/
#include "disassembler.h"

int main(int argc, char** argv){
	FILE *infile,*ofile; 
	char* metaproc;
	unsigned long include_level = 0;
	const unsigned long nbuiltin_macros = 5; 
	const unsigned long nmacrodef_macros = 2;
	variable_names[0] = "@";
	variable_expansions[0] = "";
	variable_names[1] = "$";
	variable_expansions[1] = variable_expansions[0];
	variable_names[2] = "%";
	variable_expansions[2] = variable_expansions[0];
	variable_names[3] = "'";
	variable_expansions[3] = variable_expansions[0];
	/*Macros to remove whitespace- this assembler works without using any whitespace at all. There is no tokenizer.*/
	variable_names[4] = " ";
	variable_expansions[4] = variable_expansions[0];
	nmacros = 5;
	if(argc < 2) goto ASSEMBLER_SHOW_HELP;
	{int i;for(i = 2; i < argc; i++)
	{
		if(strprefix("-o",argv[i-1]))outfilename = argv[i];
		if(strprefix("-i",argv[i-1]))infilename = argv[i];
		if(strprefix("-run",argv[i-1])){
			/*FILE* f; unsigned long which = 0;*/
			infilename = argv[i];
			run_sisa16 = 1;
			clear_output = 1;
			outfilename = NULL;
		}
		if(	strprefix("-nc",argv[i-2])
			||strprefix("--no-comments",argv[i-2])) {
			enable_dis_comments = 0;
		}
		if(strprefix("--disassemble",argv[i-2]) || strprefix("-dis",argv[i-2]) || strprefix("--disassembly",argv[i-2])){
			unsigned long loc;
			puts("//Beginning Disassembly");
			loc = strtoul(argv[i],0,0) & 0xffFFff;
			disassembler(argv[i-1], loc, 3, 256 * 256 * 256 + 1, M_SAVER[0]);
			exit(0);
		}
		if(strprefix("--full-disassemble",argv[i-2]) || strprefix("-fdis",argv[i-2]) || strprefix("--full-disassembly",argv[i-2]) ){
			unsigned long loc;
			puts("//Beginning Disassembly");
			loc = strtoul(argv[i],0,0) & 0xffFFff;
			disassembler(argv[i-1], loc, 0x1000001, 256 * 256 * 256 + 1, M_SAVER[0]);
			exit(0);
		}
	}}
	{int i;for(i = 1; i < argc; i++)
	{
		if(strprefix("-E",argv[i])) {
			quit_after_macros = 1;
			ASM_PUTS("#Contents of file post macro expansion are as follows:");
		}
		if(strprefix("-C",argv[i])){
			puts("SISA-16 Assembler and Emulator written by D[MHS]W for the Public Domain");
			puts("This program is Free Software that respects your freedom, you may trade it as you wish.");
			puts("Developer's original repository: https://github.com/gek169/Simple_ISA.git");
			puts("Please submit bug reports and... leave a star if you like the project! Every issue will be read.");
			puts("Programmer Documentation for this virtual machine is provided in the provided manpage sisa16_asm.1");
			puts("~~COMPILETIME ENVIRONMENT INFORMATION~~");
#if defined(FUZZTEST)
			puts("This build of Sisa16 is safe for fuzz testing. You can run random binaries with impunity.");
#else
			puts("Fuzztesting safety is disabled. The driver may allow you to damage the host- be careful what you run!");
#endif
#if defined(NO_SEGMENT)
			puts("The segment was disabled during compilation. emulate is also disabled.");
#else
			puts("The segment is enabled, so is Emulate.");
#endif

#if defined(NO_EMULATE)
			puts("emulate and emulate_seg are disabled.");
#else

#if defined(NO_SEGMENT)
			puts("... but you CAN use emulate_seg! Just not emulate.");
#else
			puts("You may use emulate and emulate_seg");
#endif

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

			printf("Size of u is %u, it should be 1\n", (unsigned int)sizeof(u));
			printf("Size of U is %u, it should be 2\n", (unsigned int)sizeof(U));
			printf("Size of UU is %u, it should be 4\n", (unsigned int)sizeof(UU));
			printf("Size of SUU is %u, it should be 4\n", (unsigned int)sizeof(SUU));
#if !defined(NO_FP)
			printf("Size of float is %u, it should be 4\n", (unsigned int)sizeof(float));
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
#if defined(WIN32) || defined(_WIN32)
			puts("Compiled for Macrohard Doors.");
#endif
#if defined(__unix__)
			puts("Targetting *nix");
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

		if(strprefix("-DBG",argv[i])) {
			debugging = 1;
			ASM_PUTS("<ASM> Debugging.\n");
		}
		if(strprefix("-pl",argv[i])){
			printlines = 1;
			ASM_PUTS("<ASM> Printing lines.");
		}
		if(
			strprefix("-h",argv[i]) ||
			strprefix("-v",argv[i]) ||
			strprefix("--help",argv[i]) ||
			strprefix("--version",argv[i])
		){
			ASSEMBLER_SHOW_HELP:;
			if(!clear_output)printf("Usage: %s [ARGS...]\n", argv[0]);
			puts("Optional argument: -i: specify input file.");
			puts("Optional argument: -dis, --disassemble: disassemble a file, requires an input file and a location to start disassembling.");
			puts("Optional argument: -fdis, --full-disassemble: disassemble a file, without ending on halts/illegal opcodes. Same semantics as -dis");
			puts("Optional argument: -o: specify output file. If not specified it is: outsisa16.bin");
			puts("Optional argument: -DBG: debug the assembler.");
			puts("Optional argument: -E: Print macro expansion only do not write to file");
			puts("Optional argument: -pl: Print lines");
			puts("Optional argument: -C: display compiletime environment information (What C compiler you used) as well as Author.");
			puts("Optional argument: -run: Build and Execute assembly file, like -i. Compatible with shebangs on *nix machines.\nTry adding `#!/usr/bin/sisa16_asm -run` to the start of your programs!");
			puts("Optional argument: -v, -h, --help, --version: This printout.");
			puts("\n\nSISA-16 Macro Assembler, Disassembler, Debugger, and Emulator in Pure Glorious ANSI/ISO C90, Version " SISA_VERSION);
			puts("\"Let all that you do be done with love\"");
			puts("Authored by DMHSW for the Public Domain. Enjoy.\n\n");
			puts("Language information:");
			printf("There are %d instructions in the instruction set for this version.\n", n_insns);
			printf("The last instruction added to the instruction set is %s\n", insns[n_insns-1]);
			return 0;
		}
	}}
	if(debugging) infile=stdin;

	if(infilename){
		if(debugging) if(!clear_output)printf("\nReading from a file...\n");
			infile = fopen(infilename, "rb");
		if(!infile) {
			if(!clear_output)printf("\nUNABLE TO OPEN INPUT FILE %s!!!\n", infilename);
			return 1;
		}
	} else {
		printf(general_fail_pref);
		puts("No input files.");
		puts(fail_msg);
		return 1;
	}
	ofile = NULL;
	if(!quit_after_macros && !run_sisa16){
		ofile=fopen(outfilename, "wb");
	}
	if(!run_sisa16)
		if(!ofile && !quit_after_macros){
				printf(general_fail_pref);
				printf("UNABLE TO OPEN OUTPUT FILE %s!!!\n", outfilename);
			return 1;
		}
	/*Second pass to allow goto labels*/
	for(npasses = 0; npasses < 2; npasses++, fseek(infile, 0, SEEK_SET), outputcounter=0)
	while(1){
		char was_macro = 0;	
		char using_asciz = 0;
		if(feof(infile)){
			/*try popping from the fstack*/
			if(include_level > 0){
				fclose(infile); infile = NULL;
				include_level -= 1;
				infile = fstack[include_level];
				continue;
			}
			/*else, break. End of pass.*/
			break;
		}
		if(debugging) if(!clear_output)printf("\nEnter a line...\n");
		read_until_terminator_alloced_modified(infile, &linesize, '\n'); /*Always suceeds.*/
		while(
				strprefix(" ",line)
				|| strprefix("\t",line)
				|| (isspace(line[0]) && line[0] != '\0')
		){ /*Remove preceding whitespace... we do this twice, actually...*/
			my_strcpy(line, line+1);
		}

		/*if this line ends in a backslash...*/
		while(
			!feof(infile) && strlen(line) > 0 
			&& !strprefix("!",line) 
			&& !strprefix("//",line) 
			&& !strprefix("#",line) 
			&& line[strlen(line)-1] == '\\'
		)
		{
			line[strlen((char*)line)-1] = '\0';
			rut_append_to_me = line;
			read_until_terminator_alloced_modified(infile, &linesize, '\n');
		}
		/*line_copy = strcatalloc(line,"");*/
		my_strcpy(line_copy, (unsigned char*)line);
		if(strprefix("#",line)) goto end;
		if(strprefix("//",line)) goto end;

		/*
			syntactic sugars. Only one may be used on a single line!
		*/
		/*section0;la 1;lfarpc;*/
		if(strprefix("..zero:", line)){
			perform_inplace_repl(
				line,
				"..zero:",
				"section0;"
			);
		}else if(strprefix("..z:", line)){
			perform_inplace_repl(
				line,
				"..z:",
				"section0;"
			);
		}else if(strprefix("..main:", line)){
			perform_inplace_repl(
				line,
				"..main:",
				"section0;la1;lfarpc;region1;"
			);
		}else if(strprefix("..main(", line)){
			char have_expanded = 0;
			unsigned long secnum = 0;
			long loc_eparen = -1;
			const long len_command = strlen("..main(");
			/*
				attempt to find a macro to expand here.
			*/
			do{
				unsigned long i = 0;
				have_expanded = 0;
				for(i=nbuiltin_macros; i < nmacros; i++){
					if(
						strfind(line,variable_names[i]) == len_command
						&&
						line[len_command+strlen(variable_names[i])] == /*(*/')'
						)
					{
						perform_inplace_repl(line, variable_names[i], variable_expansions[i]);
						have_expanded = 1;
						break;
					}
				}
			}while(have_expanded == 1);
			if(int_checker(line+len_command)){
				printf(syntax_fail_pref);
				printf("Bad integer constant inside of main region selection syntactic sugar.\n");
				goto error;
			}
			secnum = strtoul(line+len_command, 0,0);
			loc_eparen = strfind(line, /*(*/"):");
			if(loc_eparen == -1){
				puts( /*(*/"<ASM SYNTAX ERROR> Syntax sugar for main with region selection is missing ending \"):\"");
				puts("Line:");
				puts((char*)line_copy);
				goto error;
			}
			loc_eparen += 2;
			sprintf(buf1, "%lu", secnum);
			buf2[0] = '\0';
			strcat(buf2, "section0;la");
			strcat(buf2, buf1);
			strcat(buf2, ";lfarpc;region");
			strcat(buf2, buf1);
			strcat(buf2, ";");
			strcat(buf2, line + loc_eparen);
			my_strcpy(line, buf2);
			/*
			line = strcatallocfb(
				strcatalloc(
					"section0;la",
					buf1
				),
				strcatallocf1(
					strcatallocf1(
						strcatalloc(
						";lfarpc;region", 
						buf1
						),
						";"
					),
					line + loc_eparen
				)
			);
			*/
		}else if(strprefix("..ascii:", line)){
			my_strcpy(line+1, line+strlen("..ascii:"));
			line[0] = '!';
			using_asciz = 0;
		} else if(strprefix("..asciz:", line)){
			my_strcpy(line+1, line+strlen("..asciz:"));
			line[0] = '!';
			using_asciz = 1;
		} else if(strprefix("..(", line)){
			char have_expanded = 0;
			unsigned long secnum = 0;
			long loc_eparen = -1;
			const unsigned long len_command = strlen("..(");
			/*
				attempt to find a macro to expand here.
			*/
			do{
				unsigned long i = 0;
				have_expanded = 0;
				for(i=nbuiltin_macros; i < nmacros; i++){
					if(
						strfind(line,variable_names[i]) == len_command
						&&
						line[len_command+strlen(variable_names[i])] == /*(*/')'
						)
					{
						perform_inplace_repl(line, variable_names[i], variable_expansions[i]);
						have_expanded = 1;
						break;
					}
				}
			}while(have_expanded == 1);
			if(int_checker(line+3)){
				printf(syntax_fail_pref);
				printf("Bad integer constant inside of region select syntactic sugar.\n");
				goto error;
			}
			secnum = strtoul(line + len_command, 0,0);
			loc_eparen = strfind(line, /*(*/"):");
			if(loc_eparen == -1){
				puts( /*(*/"<ASM SYNTAX ERROR> Syntax sugar for region selection is missing ending \"):\"");
				puts("Line:");
				puts((char*)line_copy);
				goto error;
			}
			loc_eparen += 2;
			sprintf((char*)buf1, "%lu", secnum * 256 * 256);

			/*
				this huge mess replaced by...
			*/
			buf2[0] = '\0';
			strcat((char*)buf2, "section ");
			strcat((char*)buf2, (char*)buf1);
			strcat((char*)buf2, ";");
			strcat((char*)buf2, (char*)line + loc_eparen);
			my_strcpy(line, buf2);
			/*
			line = strcatallocfb(
				strcatalloc(
					"section",
					buf
				),
				strcatalloc(
					";",
					line + loc_eparen
				)
			);
			*/
		} else if(strprefix("..include\"", line)){
			long loc_eparen = strfind(line + strlen("..include\""), "\"");
			if(loc_eparen == -1){
				puts("<ASM SYNTAX ERROR> Syntactic sugar for file include is missing ending \"");
				puts("Line:");
				puts((char*)line_copy);
				goto error;
			}

			buf2[0] = '\0';
			strcat(buf2, "ASM_header ");
			line[strlen("..include\"") + loc_eparen] = '\0';
			strcat(buf2, line + strlen("..include\""));
			my_strcpy(line, buf2);
			/*
				This trash replaced...
			*/
			/*
				line = strcatallocf2(
					"ASM_header ",
					str_null_terminated_alloc(line + strlen("..include\""), loc_eparen)
				);
				if(!line){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
				free(line_old);
			*/
			
		} else if(strprefix("..dinclude\"", line)){
			long loc_eparen = strfind(line + strlen("..dinclude\""), "\"");
			if(loc_eparen == -1){
				puts("<ASM SYNTAX ERROR> Syntactic sugar for data include is missing ending \"");
				puts("Line:");
				puts((char*)line_copy);
				goto error;
			}

			buf2[0] = '\0';
			strcat(buf2, "ASM_data_include ");
			line[strlen("..dinclude\"") + loc_eparen] = '\0';
			strcat(buf2, line + strlen("..dinclude\""));
			my_strcpy(line, buf2);
			
		}else if(strprefix("..export\"", line)){
			char* variable_name;
			char found = 0;
			long loc_eparen;
			unsigned long i;
			variable_name = line + strlen("..export\"");
			loc_eparen = strfind(line + strlen("..export\""), "\"");
			if(loc_eparen == -1){
				puts( /*(*/"<ASM SYNTAX ERROR> Syntactic sugar for file include is missing ending \"");
				puts("Line:");
				puts((char*)line_copy);
				goto error;
			}
			loc_eparen += strlen("..export\"");
			line[loc_eparen] = '\0';
			for(i=nbuiltin_macros; i<nmacros; i++){
				if(streq(variable_names[i], variable_name)){
					found = 1;
					variable_is_redefining_flag[i] |= 2;
					break;
				}
			}
			if(found == 0){
				printf(compil_fail_pref);
				printf("Unknown symbol %s to export!\n", variable_name);
				goto error;
			}
			goto end;
		} else if(strprefix("..decl_farproc:", (char*) line)){
			my_strcpy(buf1, line + strlen("..decl_farproc:"));
			sprintf((char*)buf2, "VAR#%s#sc%%%lu%%;la%lu;farcall;", buf1, outputcounter & 0xFFff, outputcounter >>16);
			my_strcpy(line, buf2);
			/*
			char buf[2048];
			char* line_old = line;
			char* procedure_name = strcatalloc(line + strlen("..decl_farproc:"), "");
			if(!procedure_name){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
			buf[2047] = 0;
			sprintf(buf, "VAR#%s#sc%%%lu%%;la%lu;farcall;", procedure_name, outputcounter & 0xFFff, outputcounter >>16);
			line = strcatalloc(buf,"");
			if(!line){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
			free(line_old);
			free(procedure_name);

			*/
		} else if(strprefix("..decl_farproc(" /*)*/, (char*)line)){
			long loc_colon = -1;
			unsigned long regioncode;
			char have_expanded;
			const long len_command = strlen("..decl_farproc(" /*)*/);
			regioncode = outputcounter >>16;
			loc_colon = strfind((char*)line, /*(*/"):");
			if(loc_colon == -1)
			{
				printf(syntax_fail_pref);
				printf("decl_farproc with region code is missing end parentheses and colon!");
				goto error;
			}
			loc_colon += 2;
			/*procedure_name = strcatalloc(line + loc_colon, "");*/
			my_strcpy(buf1, line + loc_colon);
			/*Construct the name.*/
						/*
				attempt to find a macro to expand here.
			*/
						/*
				attempt to find a macro to expand here.
			*/
			do{
				unsigned long i = 0;
				have_expanded = 0;
				for(i=nbuiltin_macros; i < nmacros; i++){
					if(
						strfind((char*)line,(char*)variable_names[i]) == len_command
						&&
						line[len_command+strlen((char*)(variable_names[i]))] == /*(*/')'
						)
					{
						perform_inplace_repl(line, variable_names[i], variable_expansions[i]);
						have_expanded = 1;
						break;
					}
				}
			}while(have_expanded == 1);
			regioncode = strtoul((char*)line + len_command, 0,0);
			sprintf((char*)buf2, "VAR#%s#sc%%%lu%%;la%lu;farcall;", (char*)buf1, outputcounter & 0xFFff, regioncode);

			my_strcpy(line, buf2);
		} else if(strprefix("..decl_lproc:", line)){
			my_strcpy(buf1,line + strlen("..decl_lproc:"));
			sprintf(buf2, "VAR#%s#sc%%%lu%%;call;", buf1, outputcounter & 0xFFff);
			my_strcpy(line, buf2);
		}
		/*syntactic sugar for VAR*/
		else if(line[0] == '.'){
			long loc_colon = strfind(line, ":");
			if(line[1] == '.'){
				printf(syntax_fail_pref);
				printf("Syntactic sugar has a second period but is recognized as a macro declaration. Most likely a spelling error. Line:\n%s\n", line_copy);
				goto error;
			}
			if(loc_colon != -1){
				perform_inplace_repl(line, ".", "VAR#");/*TODO*/
				perform_inplace_repl(line, ":", "#");/*TODO*/
			} else {
				printf(syntax_fail_pref);
				printf("Syntactic sugar macro declaration without colon! Line:\n%s\n", line_copy);
				goto error;
			}
		}
		/*syntactic sugar for labels*/
		else if(line[0] == ':'){
			char*  line_old = line;
			long loc_colon2 = strfind(line+1, ":");
			if(loc_colon2 == -1){
				printf(syntax_fail_pref);
				printf("Syntactic sugar label declaration without second colon! Line:\n%s\n", line_copy);
				goto error;
			}
			line[1+loc_colon2] = '\0';
			buf2[0] = 0;
			strcat(buf2, "VAR#");
			strcat(buf2, line + 1);
			strcat(buf2, "#@");
			my_strcpy(line, buf2);
		} else if(!strprefix("!",line) && !strprefix("VAR#",line) && !strprefix("ASM_",line) && !strprefix("asm_", line)){ /*Additional syntactic sugar for labels.*/
			long loc = strfind(line, "//");
			/*We must remove line comments.*/
			if(loc!= -1){
				line[loc] = '\0';
			}
			loc = strfind(line, "#");
			if(loc!= -1){
				line[loc] = '\0';
			}
			/*Now, remove succeeding whitespace.*/
			/*Alternate syntax for labels, more traditional to assemblers.*/
			loc = strfind(line, ":");
			if(loc == 0){
				printf(syntax_fail_pref);
				printf("Cannot declare empty label!!.\r\n");
				goto error;
			}
			if(
				loc > 0 &&
				!(line[loc-1] == '\\')
			){
				line[loc] = '\0';
				buf2[0] = 0;
				strcat(buf2, "VAR#");
				strcat(buf2, line);
				strcat(buf2, "#@");
				my_strcpy(line, buf2);
			}
		}
		if(strprefix("!",line)) {unsigned long i;
			/*We are writing out individual bytes for the rest of the line.*/
			if(debugging)
				if(!clear_output)printf("Detected Character Literal on Line:\n%s\n", line_copy);
			if(printlines && npasses == 1)ASM_PUTS(line);
			if(!quit_after_macros)
				for(i = 1; i < strlen(line);i++)
					fputbyte(line[i], ofile);
			if(using_asciz) fputbyte(0, ofile);
			goto end;
		}
		if(strprefix("ASM_header ", line) || strprefix("asm_header ", line)){
			FILE* tmp; char* metaproc;
			metaproc = line + strlen("ASM_header ");
			if(include_level >= ASM_MAX_INCLUDE_LEVEL){
				printf(compil_fail_pref);
				puts("Include level maximum reached.");
				goto error;
			}
			tmp = fopen(metaproc, "r");
			if(!tmp) {
				buf2[0] = '\0';
				strcat(buf2, "/usr/include/sisa16/");
				strcat(buf2, metaproc);
				tmp = fopen(buf2, "r");
			}
			if(!tmp) { 
				buf2[0] = '\0';
				strcat(buf2, "C:\\SISA16\\");
				strcat(buf2, metaproc);
				tmp = fopen(buf2, "r");
			}
			if(!tmp) {
				printf(compil_fail_pref);
				printf("Unknown/unreachable header file %s\n", metaproc); 
				goto error;
			}
			fstack[include_level] = infile;
			include_level++;
			infile = tmp;
			goto end;
		}
		if(strprefix("ASM_data_include ", line) || strprefix("asm_data_include ", line)){
			/*data include! format is
			asm_data_include filename
			*/
			FILE* tmp; char* metaproc; unsigned long len;
			metaproc = line + strlen("ASM_data_include ");
			
			tmp = fopen(metaproc, "rb");
			if(!tmp) {
				buf2[0] = '\0';
				strcat(buf2, "/usr/include/sisa16/");
				strcat(buf2, metaproc);
				tmp = fopen(buf2, "r");
			}
			if(!tmp) { 
				buf2[0] = '\0';
				strcat(buf2, "C:\\SISA16\\");
				strcat(buf2, metaproc);
				tmp = fopen(buf2, "r");
			}
			if(!tmp) {
				printf(compil_fail_pref);
				printf("unreachable data file %s\n", metaproc); 
				goto error;
			}
			fseek(tmp, 0, SEEK_END);
			len = ftell(tmp);
			if(len > 0x1000000) {
				printf(compil_fail_pref);
				printf("data file %s too big\n", metaproc); 
				goto error;
			}
			if(len == 0){
				printf(compil_fail_pref);
				printf("data file %s is empty.\n", metaproc); 
				goto error;
			}
			fseek(tmp, 0, SEEK_SET);
			for(;len>0;len--)fputbyte(fgetc(tmp), ofile);
			fclose(tmp);
			if(printlines && npasses == 1)ASM_PUTS(line);
			goto end;
		}
		/*Step 0: PRE-PRE PROCESSING. Yes, this is a thing.*/
		pre_pre_processing:
		while(
				strprefix(" ",line) 
				|| strprefix("\t",line)
				|| (isspace(line[0]) && line[0] != '\0')
		){ /*Remove preceding whitespace... we do this twice, actually...*/
			my_strcpy(line, line+1);
		}
		/*Find line comments... but only on non-VAR lines!*/
		/*Note that even though we already did this, we must do it again!*/
		if(!strprefix("VAR#",line)){
			if(strfind(line, "//") != -1){
				line[strfind(line, "//")] = 0;
			}
			if(strfind(line, "#") != -1){
				line[strfind(line, "#")] = 0;
			}
		}
		if(	
			strprefix("ASM_COPYRIGHT", line)
			|| strprefix("asm_copyright", line)
		){
			puts("SISA-16 Assembler, Disassembler, Debugger and Emulator by David M.H.S. Webster 2021 AD\navailable to you under the Creative Commons Zero license.\nLet all that you do be done with love.\n");
			goto end;
		}
		if(
			strprefix("asm_help", line) 
			|| strprefix("ASM_HELP", line)
		){
			ASM_PUTS("For help, See: man sisa16_asm");
			goto end;
		}

		if(strlen(line) < 1) goto end; /*the line is empty*/
		if(strlen(line) == 1 && !isalpha(line[0])) goto end; /*Cannot possibly be a macro, it's the end of file thing.*/
		if(!isalpha(line[0]) && 
			line[0] != ' ' && 
			line[0] != '_' && 
			line[0] != ';' && line[0] != '\t'
			&& line[0] != '\\' && line[0] != '|'){
			if(!clear_output){
				printf(warn_pref);printf("Ignoring line beginning with illegal character... Line:\n%s\n", line_copy);
			}
			goto end;
		}
		/*Step 1: Expand Macros on this line. This includes whitespace removal.*/
		{size_t i = 0;
			for(;i<strlen(line);i++)
				if(isspace(line[i]) || line[i] == '\r') line[i] = ' ';
		}
		/*MACRO_EXPANSION*/
		if(strprefix("VAR#",line))
			was_macro=1; 
		else 
			was_macro = 0;
		{unsigned char have_expanded = 0; unsigned long iteration = 0; long i; unsigned char have_reached_builtins = 0;
			do{
				have_expanded = 0;
				if(debugging){
					if(!clear_output)printf("\n~~Macro Expansion Stage~~, iteration %lu\nLine:\n%s", iteration, line);
					if(was_macro)
						ASM_PUTS("\n(This is a macro line)\n");
				}
				for(i = 
					(have_reached_builtins?(was_macro?nmacrodef_macros:nbuiltin_macros):(was_macro?nmacrodef_macros:nmacros))-1; 
					i>=0; i--)
				{ /*Only check builtin macros when writing a macro. */
					long loc; long linesize; char found_longer_match;
					long len_to_replace; 
					char* before;
					char* after;
					long j, loc_vbar = 0;;
					buf1[0] = '\0';
					buf2[0] = '\0';
					loc_vbar = strfind(line, "|");
					linesize = strlen(line);
					if((loc_vbar == -1)) loc_vbar = linesize;
					loc = strfind(line, variable_names[i]);
					if(loc == -1) continue;
					if(loc >= loc_vbar) continue; /*Respect the sequence operator.*/
					if(loc > 0 && *(line+loc-1) == '\\') continue;
					if(was_macro && (i > 1)){
						continue; /*Do not parse any other macros excepts dollar sign and at inside of a macro.*/
					}
					if(debugging) printf("\nDiscovered possible Macro \"%s\"!\n", variable_names[i]);
										/*Check to make sure that this isn't some other, longer macro.*/
					found_longer_match = 0;
					if(i < (long)nbuiltin_macros) have_reached_builtins = 1;
					if(!was_macro && (i > (long)nbuiltin_macros))
					for(j = i-1; j>=(long)nbuiltin_macros; j--){
						if(j == i) continue;
						if(strlen(variable_names[j]) > strlen(variable_names[i])){
							long checkme;
							checkme = strfind(line, variable_names[j]);
							if(checkme == -1) continue;
							/*Does this match intersect?*/
							if(
								(
									checkme+strlen(variable_names[j]) >= loc + strlen(variable_names[i]) &&
																	checkme <= loc
								) || (
									(checkme < (long)(loc + strlen(variable_names[i]))) &&
									(checkme >= loc)
								) || (
									(checkme+strlen(variable_names[j]) < loc + strlen(variable_names[i])) &&
									((long)(checkme+strlen(variable_names[j])) >= loc)
								)
							){
								found_longer_match = 1;
								break;
							}
						}
					}
					if(found_longer_match)
					{
						if(debugging){puts("Found longer Macro.");}
						continue;
					}
					/*We know the location of a macro to be expanded and it is at loc.*/
					/*This also quit conveniently defines the recursion limit for a macro.*/
					have_expanded = 1;
					
					len_to_replace = strlen(variable_names[i]);
					/*before = str_null_terminated_alloc(line_old, loc); -- old, shit*/
					before = buf1; /*DO NOT REUSE BUF1!!!!*/
					memcpy(buf1, line, loc);
					buf1[loc] = '\0';
					/*if(!before){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}*/
					if(i > 3) /*0,1,2,3 are special cases. 4,5,X are not.*/
					{
						/*before = strcatallocf1(before, variable_expansions[i]);*/
						strcat(buf1, variable_expansions[i]);
					}/*TODO*/
					else if (i == 0){ /*SYNTAX: @+7+ or @ alone*/
						char expansion[1024];
						unsigned long addval = 0;
						/*We need to check if there is a plus sign immediately after the at sign. */
						if(strprefix("+",line+loc+1)){
							char* add_text = line+loc+2;
							/*Find the next plus*/
							long loc_eparen = strfind(line+loc+2,"+");
							if(loc_eparen == -1){
								printf(syntax_fail_pref);
								printf("@ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								if(!clear_output){
									printf(warn_pref);
									printf("@ with empty add section. Line:\n%s\n", line_copy);
								}
							}
							addval = strtoul(add_text,0,0);
							if(addval == 0 && npasses == 1)
								if(!clear_output){
									printf(warn_pref);
									printf("@ with add evaluating to zero. Line:\n%s\n", line_copy);
								}
							if(addval)
							len_to_replace += (loc_eparen-len_to_replace+3);
						}
						addval += outputcounter;
						if(strprefix("&",line+loc+1)){
							addval >>= 16;
							len_to_replace++;
						}
						sprintf(expansion, "%lu", addval);
						expansion[1023] = '\0'; /*Just in case...*/
						/*before = strcatallocf1(before, expansion);*/
						strcat(buf1, expansion);
						if(!before){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
					} else if (i==1){
						char expansion[1024]; 
						unsigned long addval;
						addval = 0;
						if(strprefix("+",line+loc+1)){char* add_text; long loc_eparen;
							add_text = line+loc+2;
							/*Find the next plus*/
							loc_eparen = strfind(line+loc+2,"+");
							if(loc_eparen == -1){
								printf(syntax_fail_pref);
								printf("$ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								if(!clear_output){
									printf(warn_pref);
									printf("$ with empty add section. Line:\n%s\n", line_copy);
								}
							}
							addval = strtoul(add_text,0,0);
							if(addval == 0 && npasses == 1)
								if(!clear_output){
									printf(warn_pref);
									printf("$ with add evaluating to zero. Line:\n%s\n", line_copy);
								}
							len_to_replace += (loc_eparen-len_to_replace+3);
						}

						sprintf(expansion, "%lu,%lu", (unsigned long)(addval/256),(unsigned long)(addval&0xff));
						expansion[1023] = '\0'; /*Just in case...*/
						strcat(buf1, expansion);
					} else if (i==2){
						long loc_qmark=		-1;
						long loc_slash=		-1;
						long loc_dash_mark=		-1;
						long loc_tilde=		-1;
						long loc_ampersand=	-1;
						char do_32bit=		0;
						char do_8bit=		0;
						long loc_eparen=	-1;
						char expansion[1024];
						unsigned long res=0; /*Split directive.*/
						/*Locate the next ending one*/
						if(strlen(line+loc) == 0){
							printf(syntax_fail_pref);
							printf("SPLIT (%%) is at end of line. Line:\n%s\n", line_copy);
							goto error;
						}
						loc_eparen = strfind(line+loc+1,"%");
						loc_slash = strfind(line+loc+1,"/");
						loc_qmark = strfind(line+loc+1,"?");
						loc_dash_mark = strfind(line+loc+1,"-");
						loc_tilde = strfind(line+loc+1,"~");
						loc_ampersand = strfind(line+loc+1,"&");
						if(loc_eparen == -1){
							printf(syntax_fail_pref);
							printf("SPLIT (%%) without ending %%. At location:\n%s\nLine:\n%s\n",line+loc,line_copy);
							goto error;
						}
						if(loc_eparen == 0){
							if(!clear_output){
								printf(warn_pref);
								printf("SPLIT (%%) is empty. At location:\n%s\nLine:\n%s\n",line+loc,line_copy);
							}
						}
						if(loc_slash==0) do_32bit = 1;
						if(loc_dash_mark==0) do_32bit = 4;
						if(loc_ampersand==0) do_32bit = 3;
						if(loc_qmark==0) do_32bit = 2;
						if(loc_tilde==0) {do_8bit = 1;do_32bit = 0;}
						
						/*the character we were going to replace anyway, plus
						the length of the stuff inbetween, plus the */
						len_to_replace+=(loc_eparen-len_to_replace+2);
						if(do_8bit != 0)
							res = strtoul(line+loc+2, NULL, 0);
						else if(do_32bit == 0)
							res = strtoul(line+loc+1, NULL, 0);
						else if(do_32bit == 1 || do_32bit == 3) /*Skip the forward slash or ampersand*/
							res = strtoul(line+loc+2, NULL, 0);
						else if(do_32bit == 4){
							UU d1;
							/*Two's complement.*/
							d1 = strtoul(line+loc+2, NULL, 0); /*Skip the dash.*/
							d1 = ~d1; 	/*ones compl*/
							d1++; 		/*twos compl*/
							res = d1;
						}else if(do_32bit == 2){
#if defined(NO_FP)
							puts("<ASM ENV ERROR> Floating point unit was disabled during compilation. You may not use floating point SPLIT directives.");
							exit(1);
#else
							float a;UU d1;
							if(sizeof(a) != 4){puts("<ASM ENV ERROR> Floating point environment INCOMPATIBLE.");exit(1);}
							a = atof(line+loc+2);
							memcpy(&d1, &a, 4);
							if(sizeof(UU) == 4)
								res = d1;
							else {
								puts("<ASM ENV ERROR> UU is not 32 bit, try toggling -DUSE_UNSIGNED_INT");
								exit(1);
							}
#endif
						}
						{
							const char* error_fmt = "Unusual SPLIT (%%) evaluates to zero. Line:\n%s\nValue:\n%s\nInternal:\n%s\n";
							if(!do_32bit && !do_8bit){
								if(res == 0  && npasses == 1 && line[loc+1] != '%' && line[loc+1] != '0')
									if(!clear_output){
										printf(warn_pref);
										printf(error_fmt, line_copy, line+loc+1, line);
									}
							} else {
								
								if(res == 0  && npasses == 1 && line[loc+2] != '%' && line[loc+2] != '0')
									if(!clear_output)
									{
										printf(warn_pref);
										printf(error_fmt, line_copy, line+loc+2, line);
									}
							}
						}
						if(debugging) if(!clear_output)printf("\nSplitting value %lu\n", res);
						/*Write the upper and lower halves out, separated, to expansion.*/
						if(do_32bit == 0) {
							if(do_8bit == 0)
								sprintf(expansion, "%u,%u", ((unsigned int)(res/256)&0xff),(unsigned int)(res&0xff));
							else
								sprintf(expansion, "%u", (unsigned int)(res&0xff));
						} else if(do_32bit == 1 || do_32bit == 2 || do_32bit == 4) {
							sprintf(expansion, "%u,%u,%u,%u", (unsigned int)((res/(256*256*256))&0xff),
																(unsigned int)((res/(0x10000))&0xff),
																(unsigned int)((res/(256))&0xff),
																(unsigned int)((res)&0xff)
																);
						} else if(do_32bit == 3) {
							sprintf(expansion, "%u,%u,%u",
									(unsigned int)((res/(0x10000))&0xff),
									(unsigned int)((res/(256))&0xff),
									(unsigned int)((res)&0xff)
									);
						} else {
							printf(internal_fail_pref);puts("Invalid do_32bit mode in a split directive.");
							exit(1);
						}
						strcat(buf1, expansion);
					} else if (i==3){
						char expansion[30];
						unsigned char character_literal = ' '; /*It was probably the space character.*/
						if(strlen(line+loc) == 0){
							printf(syntax_fail_pref);
							printf("character literal is at end of line. Line:\n%s\n", line_copy);
							goto error;
						}
						expansion[29] = '\0';
						for(;line[loc+len_to_replace] && line[loc+len_to_replace] != '\'';len_to_replace++){
							if(line[loc+len_to_replace] == '\\'){
								len_to_replace++;
								if(line[loc+len_to_replace] == '\0'){
									printf(syntax_fail_pref);
									printf("character literal escapes the end of line??? Line:\n%s\n", line_copy);
								}
								character_literal = ((unsigned char*)line)[loc+len_to_replace];
								if(character_literal == 'n') character_literal = '\n';
								else if(character_literal == 'r') character_literal = '\r';
								else if(character_literal == 'a') character_literal = '\a';
								else if(character_literal == 'b') character_literal = '\b';
								else if(character_literal == 'f') character_literal = '\f';
								else if(character_literal == 'e') character_literal = '\e';
								else if(character_literal == 'v') character_literal = '\v';
								else if(character_literal == 't') character_literal = '\t';
								else if(character_literal == '0') character_literal = '\0';
								else if(character_literal == 'x') {
									printf(syntax_fail_pref);
									printf("Hex character literals not supported. Just use 0xXX for your number. Line:\n%s\n", line_copy);
									goto error;
								}
							} else {
								character_literal = ((unsigned char*)line)[loc+len_to_replace];
							}
						}
						if(line[loc+len_to_replace] != '\''){
							printf(syntax_fail_pref);
							printf("Character literal missing ending single quote! Line:\n%s\n", line_copy);
							goto error;
						}
						len_to_replace++;
						sprintf(expansion, "%lu", (unsigned long)character_literal);
						strcat(buf1, expansion);
					}
					strcat(buf1, line+loc+len_to_replace);
					my_strcpy(line, buf1);
					break;
					/*after = str_null_terminated_alloc(line+loc+len_to_replace,
									linesize-loc-len_to_replace);*/
					/*
					if(!after){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
					line = strcatallocfb(before, after);
					if(!line){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
					free(line);
					break; we have expanded something.*/
				}
			}while(have_expanded && (iteration++ < 32768));
		}
		/*Check again to see if this is a comment line.*/
		if(strprefix("#",line)) goto end;
		if(strprefix("//",line)) goto end;
		if(strprefix("!",line) || strfind(line, "!") != -1) {
			printf(syntax_fail_pref);printf("Invalid character literal.");
			printf("Character literals must be on their own line with no preceding whitespace.");
			printf("\nLine:\n%s\n", line_copy);
			goto error;
		}
		
		/*Step 2: Check to see if this is a macro*/
		if(debugging){
			if(!clear_output)printf("\n~~Is this a macro?~~\n");
		}
		/*MACRO_DEFINITION_STAGE*/
		if(strprefix("VAR",line)){ 
			long loc_pound, loc_pound2; char* macro_name;char is_overwriting; /*It's show time!*/
			unsigned long index;
			if(debugging){
				if(!clear_output)printf("\nThis is a macro!\n");
			}
			was_macro = 1;

			loc_pound = strfind(line, "#");
			loc_pound2 = 0;
			if(loc_pound == -1) {
				printf(syntax_fail_pref);printf("missing leading # in macro declaration. Line:\n%s\n", line_copy); 
				goto error;
			}
			/*We have a macro.*/
			macro_name = line + loc_pound;
			if(debugging){
				if(!clear_output)printf("\nMacro Name Pre-Allocation is identified as %s\n", macro_name);
			}
			if(strlen(macro_name) == 1){
				printf(syntax_fail_pref);printf("missing second # in macro declaration. Line:\n%s\n", line_copy); 
				goto error;
			}
			macro_name += 1; loc_pound += 1; /*We use these again.*/
			loc_pound2 = strfind(macro_name, "#");
			if(loc_pound2 == -1) {
				printf(syntax_fail_pref);printf("missing second # in macro declaration. Line:\n%s\n", line_copy); 
				goto error;
			}
			macro_name = str_null_terminated_alloc(macro_name, loc_pound2);/*TODO- use segment.*/
			if(!macro_name){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
			if(debugging){
				if(!clear_output)printf("\nMacro Name is identified as %s\n", macro_name);
			}
			if(strlen(line + loc_pound + loc_pound2) < 2){
				printf(syntax_fail_pref);printf("macro without a definition. Line:\n%s\n", line_copy);
				goto error;
			}
			loc_pound2++;
			/*Search to see if we've already defined this macro*/
			is_overwriting = 0;
			index = 0;
			{unsigned long i;for(i = 0; i < n_insns; i++){
				if( (strfind(insns[i],macro_name)>-1) ||
					(strfind(macro_name, insns[i])==0) 
				){
					printf(syntax_fail_pref);printf("This macro would prevent instruction %s from being used:\n%s\n", insns[i], line_copy);
					goto error;	
				}
			}}

			/*Conditional Declaration.*/
			if(macro_name[0] == '?'){
				unsigned long i;
				my_strcpy(macro_name, macro_name+1);
				for(i = nbuiltin_macros; i < nmacros; i++)
					if(streq(macro_name, variable_names[i])){ /*Conditional declaration no longer accepted.*/
						free(macro_name);
						goto end;
					}
			}
			/*
				Prevent macros from being defined which are illegal.
			*/
			{unsigned long q;
				if(strlen(macro_name) == 0){
					printf(syntax_fail_pref);printf("This macro has an EMPTY NAME. Invalid:\n%s\n", line_copy);
					goto error;
				}
				for(q = 0; q < strlen(macro_name); q++)
					if(!isalnum(macro_name[q]) && macro_name[q] != '_'){
						printf(syntax_fail_pref);printf("This macro contains illegal character '%c':\n%s\n",macro_name[q], line_copy);
						goto error;
					}
				if(!(isalpha(macro_name[0]) || macro_name[0] == '_')){
					printf(syntax_fail_pref);printf("This macro begins with a number! '%c':\n%s\n",macro_name[0], line_copy);
					goto error;
				}
			}
			/*
				Check and make sure this is not a reserved name
			*/
			if(
				strfind(macro_name, "!") != -1 ||
				strfind(macro_name, ".") != -1 ||
				strfind(macro_name, "?") != -1 ||
				strfind(macro_name, ".") != -1 ||
				strfind(macro_name, ":") != -1 ||
				strfind(macro_name, "~") != -1 ||
				strfind(macro_name, "-") != -1 ||
				strfind(macro_name, "[") != -1 ||
				strfind(macro_name, "(") != -1 ||
				strfind(macro_name, "]") != -1 ||
				strfind(macro_name, ")") != -1 ||
				strfind(macro_name, "\\") != -1 ||
				strfind(macro_name, "/") != -1 ||
				strfind(macro_name, " ") != -1 ||
				strfind(macro_name, "\t") != -1 ||
				strfind(macro_name, "\v") != -1 ||
				strfind(macro_name, "\f") != -1 ||
				strfind(macro_name, "\n") != -1 ||
				strfind(macro_name, "\r") != -1 ||
				strfind(macro_name, ";") != -1 ||
				strfind(macro_name, "|") != -1 ||
				strfind(macro_name, "#") != -1 ||
				strfind(macro_name, "$") != -1 ||
				strfind(macro_name, "&") != -1 ||
				strfind(macro_name, "@") != -1 ||
				strfind(macro_name, "%") != -1 ||
				strfind(macro_name, "bytes") != -1 ||
				strfind(macro_name, "shorts") != -1 ||
				strfind(macro_name, "asm_") != -1 ||
				strfind(macro_name, "ASM_") != -1 ||
				/*begins with a number*/
				(macro_name[0] <= '9' && macro_name[0] >= '0')
			)
			{
				printf(syntax_fail_pref);printf("This macro deliberately attempts to define or trample a reserved name or character. You may not use this name:\n%s\n", macro_name);
				ASM_PUTS("NOTE: You cannot use names beginning with numbers, either.");
				goto error;	
			}

			if(
				strfind("bytes",macro_name) != -1 ||
				strfind("shorts",macro_name) != -1 ||
				strfind("asm_",macro_name) != -1 ||
				strfind("ASM_", macro_name) != -1 ||
				strfind("section", macro_name) != -1 ||
				strfind("region", macro_name) != -1 ||
				strfind("fill", macro_name) != -1 ||
				strfind("..decl_farproc(", macro_name) != -1 ||
				strfind("asm_help", macro_name) != -1 ||
				strfind("asm_export_header", macro_name) != -1 ||
				strfind("asm_fix_outputcounter", macro_name) != -1 ||
				strfind("asm_print", macro_name) != -1 ||
				strfind("asm_vars", macro_name) != -1 ||
				strfind("asm_begin_region_restriction", macro_name) != -1 ||
				strfind("asm_begin_page_restriction", macro_name) != -1 ||
				strfind("asm_begin_block_restriction", macro_name) != -1 ||
				strfind("asm_end_region_restriction", macro_name) != -1 ||
				strfind("asm_end_page_restriction", macro_name) != -1 ||
				strfind("asm_end_block_restriction", macro_name) != -1 ||
				strfind("asm_end_restriction", macro_name) != -1 ||
				strfind("asm_quit", macro_name) != -1
			)
			{
				printf(syntax_fail_pref);printf("This macro would prevent language features from being used. You may not use this name:\n%s\n", macro_name);
				goto error;	
			}
			{unsigned long i;for(i = 0; i < nmacros; i++){
				if(i < nbuiltin_macros)
					if( (strfind(variable_names[i],macro_name)>-1) ||
						streq(macro_name, variable_names[i]))
					{
						printf(syntax_fail_pref);printf("This macro would prevent critical macro %s from being used.Line:\n%s\n", variable_names[i], line_copy);
						goto error;	
					}
				if(
					(!streq(macro_name, variable_names[i]))
					&& (npasses==1) && 
					(
						(strfind(variable_names[i],macro_name)>-1) ||
						(strfind(macro_name, variable_names[i])>-1)
					)
				){
					if(!clear_output){
					printf(warn_pref);
					printf("This Macro may produce a conflict with other Macro: \"%s\"Line:\n%s\n",variable_names[i], line_copy);
					}
				}
				
				if(streq(macro_name, variable_names[i])){
					if(is_overwriting){
						printf(internal_fail_pref);printf("Multiple macros exist with the same name. Line:\n%s\n", line_copy);
						goto error;
					}
					is_overwriting = 1;
					if(i < nbuiltin_macros){
						printf(syntax_fail_pref);printf("attempted redefinition of critical macro, Line:\n%s\n", line_copy);
						goto error;	
					}
					index = i;
				}
			}}
			if(!is_overwriting){
				if(nmacros >= (SISA16_MAX_MACROS-1)) {
					printf(compil_fail_pref);printf("Too many macros. Cannot define another one. Line:\n%s\n", line_copy); 
					goto error;
				}
				variable_names[nmacros] = macro_name;
				variable_expansions[nmacros++] = 
				str_null_terminated_alloc(/*TODO- use sgment.*/
						line+loc_pound+loc_pound2,
						strlen(line+loc_pound+loc_pound2)
				);
				if(!variable_expansions[nmacros-1]){printf(general_fail_pref); printf("Failed Malloc."); exit(1);}
			} else {
				if(npasses == 0)
				{
					printf(general_fail_pref);
					printf("Cannot redefine already made macro.");
				}
				free(macro_name);
				my_strcpy(buf1, line+loc_pound+loc_pound2);
				if(npasses == 1 && !(variable_is_redefining_flag[index]&1))
				{/*Ensure that the macro evaluates to the exact same piece of text as the last time.*/
					if(!streq(buf1, variable_expansions[index])){
						printf("\r\n\r\n<ASM HUGE ERROR>\r\n\r\nConfirmed!!! Macro Desynchronization between passes Line:\n%s\nInternally:\n%s\n",line_copy,line);
						goto error;
					}
				}
			}
			if(debugging){
				if(!clear_output)printf("\nMacro Contents are %s, size %u\n", variable_expansions[nmacros-1], (unsigned int)strlen(variable_expansions[nmacros-1]));
				if(!clear_output)printf("\nMacro Name is %s, size %u\n", variable_names[nmacros-1], (unsigned int)strlen(variable_names[nmacros-1]));
			}
		}
		if(was_macro) goto end;
		if(quit_after_macros || debugging) {
			if(!clear_output)printf("\n#Line post macro expansion and evaluation:\n");
			ASM_PUTS(line); 
			if(quit_after_macros) goto end;
		}

		
		/*We must first determine if this line contains a line comment. Don't search past the line comment for insns.*/
		if(strfind(line, "//") != -1){
			long loc_line_comment = strfind(line, "//");
			line[loc_line_comment] = '\0';
		}
		
		/* 
			Assembly Directives

		*/
		
		/*	most complicated step- handle insns.
			Everything breaks down to bytes underneath, so we convert to "bytes" commands.
			The difference between here and above is that we have to count
			how many commas we're supposed to encounter.
			the first comma beyond that before the next semicolon, is replaced with a semicolon.
		*/

		{unsigned long i = 0;
		for(i=0; i<n_insns; i++)
		if(
			(insn_repl[i][strlen(insn_repl[i])-1] == ',' &&
			insns_numargs[i] == 0) ||
			(insn_repl[i][strlen(insn_repl[i])-1] == ';' &&
			insns_numargs[i] > 0)
		)
		{
			printf("<ASM INTERNAL ERROR> instruction with bad args. %s \n", insns[i]);
			goto error;
		}}
		/*INSN_EXPANSION_STAGE*/
			{unsigned char have_expanded = 0; unsigned long iteration = 0;
			do{
				have_expanded = 0;
				if(debugging){
					if(!clear_output)printf("\n~~Insn Expansion Stage~~, iteration %lu\nLine:\n%s", iteration, line);
				}
				{unsigned long i;for(i = 0; i<n_insns; i++){
					long loc, linesize; unsigned long j;
					char found_longer_match; int num_commas_needed;
					linesize = strlen(line);
					loc = strfind(line, insns[i]);					
					if(loc == -1) continue;
					if(strfind(line, "|")!= -1 &&
						strfind(line, "|") < loc) continue;

					/*
						run a quick check on insns_numargs[i] versus insn_repl[i]
					*/
					
					/*Check to make sure that this isn't some other, longer insn.*/
					found_longer_match = 0;
					for(j = i+1; j<n_insns; j++){
						if(j == i) continue;
						if(strlen(insns[j]) > strlen(insns[i])){
							long checkme = strfind(line, insns[j]);
							if(checkme == -1) continue;
							/*Does this match contain us?*/
							if(
								/*
								checkme+strlen(insns[j]) >= loc + strlen(insns[i]) &&
								checkme <= loc*/

								(
									checkme+strlen(insns[j]) >= loc + strlen(insns[i]) &&
									checkme <= loc
								) ||
								(
									((checkme+(long)strlen(insns[j])) < loc + (long)strlen(insns[i])) &&
									((checkme+(long)strlen(insns[j])) >= loc)
								) || 
								(
									((checkme) < loc + (long)strlen(insns[i])) &&
									((checkme) >= loc)
								)
							)
							{
								if(debugging) ASM_PUTS("Found longer insn.");
									found_longer_match = 1;
									break;
							}
						}
					}
					if(found_longer_match) continue;
					if(loc>0){
						/*Check to make sure this isn't prefixed by something which is obviously erroneous.*/
						if(*(line+loc-1) != ';')
							if(!clear_output){
								printf(warn_pref);
								printf("Insn parsing reached with "
									"unusual prefixing character \'%c\'."
									"Instruction to parse is %s. Line:\n%s\nInternally:\n%s\n",*(line+loc-1),insns[i],line_copy, line);
							}
					}
					if(
						*(line+loc+strlen(insns[i])) != ';' &&  
						*(line+loc+strlen(insns[i])) != '\0' && 
						!(
							*(line+loc+strlen(insns[i])) >= '0' &&
							*(line+loc+strlen(insns[i])) <= '9'
						) &&
						*(line+loc+strlen(insns[i])) != ' '
					)
						if(!clear_output){
							printf(warn_pref);
							printf("Instruction parsing reached with "
								"unusual postfixing character \'%c\'. Instruction to parse is %s. Line:\n%s\nInternally:\n%s\n",*(line+loc+strlen(insns[i])),insns[i],line_copy,line);
						}
					/*We know the location of an insn to be expanded and it is at loc.*/
					/*Crawl along the string. */
					num_commas_needed = insns_numargs[i] - 1;
					/*If you have three arguments, you only need two commas.*/
					for(j = loc + strlen(insns[i]); strlen(line+j)>0;j++){
						if(line[j] == ','){
							if(num_commas_needed > 0) num_commas_needed--;
							else { /*Too many commas.*/
								printf(syntax_fail_pref);
								printf("too many arguments to insn %s, line %s", insns[i], line_copy);
								goto error;
							}
						}
						if(line[j] == ';' || line[j] == '\n' || line[j] == '\0'){
							if(num_commas_needed > 0)
							{ /*Too many commas.*/
								printf(syntax_fail_pref);printf("Insn requires more arguments. line: %s\ninsn: %s\n", line_copy, line + loc);
								goto error;
							}
							break; /*Out of this for loop.*/
						}
					}
					if(num_commas_needed > 0)
					{ /*Too many commas.*/
						printf(syntax_fail_pref);printf("Insn requires more arguments. line %s", line_copy);
						goto error;
					}
					/**/
					have_expanded = 1;
					{
						long len_to_replace;
						len_to_replace = strlen(insns[i]);
						memcpy(buf1, line, loc);
						buf1[loc] = '\0';
						strcat((char*)buf1, insn_repl[i]);
						strcat(buf1, line+loc+len_to_replace);
						my_strcpy(line, buf1);
					}
				}}
			}while(have_expanded && (iteration++ < 0x100000)); /*Probably safe right?*/
		}
		/*
			Put out bytes.
		*/
		/*Inch along*/
		if(printlines && npasses==1){
			long loc_vbar = 0;
			loc_vbar = strfind(line, "|");
			if(loc_vbar != -1) line[loc_vbar] = '\0';
			if(!clear_output)printf("%s\n",line);
			if(loc_vbar != -1) line[loc_vbar] = '|';
		}
		metaproc = line; /*declaration at beginning of main.*/
		do{ long incr, incrdont;
			if(strprefix("bytes", metaproc)){ char* proc; /*VERY_BASIC_THINGS_IT_CAN_DO*/
				proc = metaproc + 5;
				do{
					unsigned char byteval; unsigned long preval; 
					if(int_checker(proc)){
						printf(syntax_fail_pref);printf("invalid integer literal for bytes. Line:\n%s\nInternal:\n%s\n",line_copy, line);
						goto error;
					}
					
					preval = strtoul(proc,NULL,0);
					byteval = preval & 255;
					fputbyte(byteval, ofile);
					/*Find the next comma.*/
					incr = strfind(proc, ",");
					incrdont = strfind(proc, ";");
					if(incr == -1) break;
					if(incrdont != -1 &&
						incr > incrdont)break;/**/
					proc += incr; proc += 1; /*Skip the comma itself.*/
				}while(strlen(proc) > 0);
				
			} else if(strprefix("shorts", metaproc)){
				char* proc = metaproc + 6;
				do{
					unsigned short shortval;
					if(int_checker(proc)){
						printf(syntax_fail_pref);printf("invalid integer literal for shorts command. Line:\n%s\nInternal:\n%s\n",line_copy, line);
						goto error;
					}
					shortval = strtoul(proc,NULL,0);
					putshort(shortval, ofile);
					/*Find the next comma.*/
					incr = strfind(proc, ",");
					incrdont = strfind(proc, ";");
					if(incr == -1) break;
					if(incrdont != -1 &&
						incr > incrdont) break; /**/
					proc += incr; proc += 1; /*Skip the comma itself.*/
				}while(strlen(proc) > 0);
			} else if(strprefix("section", metaproc)){ 
				unsigned long dest;
				char* proc = metaproc + 7;
				if(strlen(proc) == 0){
					printf(syntax_fail_pref);puts("Cannot have empty SECTION tag.");
					goto error;
				}
				if(int_checker(proc)){
					printf(syntax_fail_pref);printf("invalid integer literal for section. Line:\n%s\nInternal:\n%s\n",line_copy, line);
					goto error;
				}
				dest = strtoul(proc, NULL, 0);
				if(dest == 0){
				/*Explicitly check to see if they actually typed zero.*/
					if(proc[0]!='0'  && npasses == 1)
					if(!clear_output){
						printf(warn_pref);
						printf("Section tag at zero. Might be a bad number. Line %s\n", line_copy);
					}
				}
				if(debugging)
					if(!clear_output)printf("Moving the output counter to %lu\n", dest);
				outputcounter = dest;
			} else if(strprefix("region", metaproc)){
				unsigned long dest;
				char* proc = metaproc + 6;
				if(strlen(proc) == 0){
					printf(syntax_fail_pref);puts("Cannot have empty region tag.");
					goto error;
				}
				if(int_checker(proc)){
					printf(syntax_fail_pref);printf("invalid integer literal for region. Line:\n%s\nInternal:\n%s\n",line_copy, line);
					goto error;
				}
				dest = strtoul(proc, NULL, 0);
				dest &= 0xff;
				dest <<= 16;
				if(dest == 0){
				/*Explicitly check to see if they actually typed zero.*/
					if(proc[0]!='0'  && npasses == 1)
					if(!clear_output){
						printf(warn_pref);
						printf("region tag at zero. Might be a bad number. Line %s\n", line_copy);
					}
				}
				if(debugging)
					if(!clear_output)printf("Moving the output counter to %lu\n", dest);
				outputcounter = dest;
			} else if(strprefix("fill", metaproc)){ unsigned long fillsize; long next_comma; unsigned char fillval;
				char* proc = metaproc + 4;
				if(strlen(proc) == 0){
					printf(syntax_fail_pref);puts("Cannot have empty fill tag.");
					goto error;
				}
				if(int_checker(proc)){
					printf(syntax_fail_pref);printf("invalid integer literal for fill command. Line:\n%s\nInternal:\n%s\n",line_copy, line);
					goto error;
				}
				fillsize = strtoul(proc, NULL, 0);
				if(fillsize == 0){
					if(proc[0]!='0') /*Check if they actually typed zero.*/
					if(!clear_output)
					{
						printf(warn_pref);printf("fill tag size is zero. Might be a bad number. Line %s", line_copy);
					}
				}
				/**/
				next_comma = strfind(proc, ",");
				if(next_comma == -1) {
					printf(syntax_fail_pref);printf("Fill tag missing value. Line %s", line_copy);
					goto error;
				}
				proc += next_comma + 1;
				if(int_checker(proc)){
					printf(syntax_fail_pref);printf("invalid integer literal for fill. Line:\n%s\nInternal:\n%s\n",line_copy, line);
					goto error;
				}
				fillval = strtoul(proc, NULL, 0);
				if(fillval == 0) /*potential for a mistake*/
					if(proc[0]!='0') /*Did they actually MEAN zero?*/
						if(!clear_output){
							printf(warn_pref);
							printf("fill tag value is zero. Might be a bad number. Line:\n%s\n", line_copy);
						}
				for(;fillsize>0;fillsize--)fputbyte(fillval, ofile);
			} else if(strprefix("asm_fix_outputcounter", metaproc)){ /*Perform a second-pass correction of the output counter.*/
				char* proc; char mode; unsigned long outputcounterold;
				if(npasses == 1){
					proc = metaproc + strlen("asm_fix_outputcounter");
					if(proc[0] == '+') mode=0; else
					if(proc[0] == '-') mode=1; else
					{
						printf(syntax_fail_pref);printf("asm_fix_outputcounter lacks a + or -.Line:\n%s\n", line_copy);
						goto error;
					}proc++;
					outputcounterold = outputcounter;
					if(int_checker(proc)){
						printf(syntax_fail_pref);printf("invalid integer literal for asm_fix_outputcounter. Line:\n%s\nInternal:\n%s\n",line_copy, line);
						goto error;
					}
					if(!mode)
						outputcounter += strtoul(proc, NULL, 0);
					else
						outputcounter -= strtoul(proc, NULL, 0);
					outputcounter &= 0xffFFff;
					if(!clear_output){
						printf(warn_pref);
						printf("output counter was:%lx, now: %lx. Line:\n%s\n", outputcounterold, outputcounter, line_copy);
					}
				}
			} else if(strprefix("asm_print", metaproc)){
				if(npasses == 1 && !printlines)
					if(!clear_output)printf("\nRequest to print status at this insn. STATUS:\nLine:\n%s\nLine Internally:\n%s\nCounter: %04lx\n", line_copy, line, outputcounter);
			} else if(strprefix("asm_begin_region_restriction", metaproc)){
				/*The assembler will warn you if the region changes during the creation of the function.*/
				region_restriction = (outputcounter>>16) & 0xFF;
				region_restriction_mode = 2; /*region*/
			} else if(strprefix("asm_end_region_restriction", metaproc)){
				region_restriction_mode = 0; /*end region*/
			} else if(strprefix("asm_begin_block_restriction", metaproc) || strprefix("asm_begin_page_restriction", metaproc)){
				/*The assembler will warn you if the block changes during the creation of the function.*/
				region_restriction = (outputcounter>>8) & 0xFFFF;
				region_restriction_mode = 1; /*block*/
			} else if(
					strprefix("asm_end_block_restriction", metaproc)|| 
					strprefix("asm_end_page_restriction", metaproc)||
					strprefix("asm_end_restriction", metaproc)
				)
			{
				region_restriction_mode = 0; /*end block*/
			} else if(strprefix("asm_vars", metaproc)){
				unsigned long i;
				if(!clear_output)printf("\nSTATUS:\nLine:\n%s\nLine Internally:\n%s\nCounter: %04lx\n", line_copy, line, outputcounter);
				if(!clear_output)printf("<DUMPING SYMBOL TABLE ON PASS %ld>\n", npasses+1);
				for(i=0;i<nmacros;i++){
					if(i==nmacros) ASM_PUTS("Showing User Macros");
					if(i==0){char expansion[1024];
						sprintf(expansion, "%lu", outputcounter);
						expansion[1023] = '\0'; /*Just in case...*/
						if(!clear_output)printf("VAR#%s#%s\n",variable_names[i],expansion);
					} else if(i==1){char expansion[1024];
						sprintf(expansion, "%lu,%lu", (unsigned long)(outputcounter/256),(unsigned long)(outputcounter&0xff));
						expansion[1023] = '\0'; /*Just in case...*/
						if(!clear_output)printf("VAR#%s#%s\n",variable_names[i],expansion);	
					}else
						if(!clear_output)printf("VAR#%s#%s\n",variable_names[i],variable_expansions[i]);
				}
			} else if (
				strprefix("ASM_EXPORT_HEADER", metaproc)
			){
				/*
					Create a header file for this compilation unit. That means exporting all macros
					that are not redefining.
				*/
				if(!run_sisa16)
				if(npasses == 1){
					char* const hfilename = buf1;
					unsigned long i;
					FILE* f;
					/*hfilename = strcatalloc(outfilename, ".hasm.tmp");TODO*/
					my_strcpy(buf1, outfilename);
					strcat(buf1, ".hasm.tmp");
					f = fopen(buf1, "w");
					if(f){
						for(i = nbuiltin_macros; i < nmacros; i++){
							if( variable_is_redefining_flag[i]&2 ){
								if(!(variable_is_redefining_flag[i]&1) ){
										fprintf(f, "VAR#%s#%s\n", variable_names[i], variable_expansions[i]);
								} else {
									printf(compil_fail_pref);
									printf("Cannot Export a redefining symbol.\n");
									goto error;
								}
							}
						}
					}else{
						printf(general_fail_pref);
						printf("Cannot open file %s", buf1);
						exit(1);
					}
				}
			} else if(strprefix("asm_quit", metaproc)){
				if(npasses == 1)
					printf("\nHalting Assembly.STATUS:\nLine:\n%s\nCounter: %04lx\n", line_copy, outputcounter);
				goto error;
			}else if(strprefix("!", metaproc)){
				printf(syntax_fail_pref);printf("Cannot put string literal here, Line:\n%s\nInternal:\n%s\n", line_copy, metaproc);
				goto error;
			}else if (strprefix("//", metaproc)){
				break; /*Comment on line.*/
			} else {
				if(strlen(metaproc) > 0 && !
					(strfind(metaproc,";") == 0 ||
					strfind(metaproc,"|") == 0)){
					printf(syntax_fail_pref);printf("invalid statement on line %s\nStatement:%s\n", line_copy, metaproc);
					goto error;
				}
			}
			{	long next_semicolon = strfind(metaproc, ";");
				long next_vbar = strfind(metaproc, "|");
				if(next_semicolon == -1) break; /*We have handled all sublines.*/
				if(next_vbar!=-1 && next_vbar < next_semicolon) break; /**/
				metaproc += next_semicolon + 1;
				if(strlen(metaproc) == 0) break; /*Convenient line break*/
			}
		} while(1);
		/*if this is a line with vertical bars, start processing the stuff after the next vertical bar. */
		if(strfind(line, "|")!=-1){
			long loc = strfind(line, "|")+1;
			my_strcpy(line_copy, (unsigned char*)line + loc);
			goto pre_pre_processing;
		}
		end:
/*		free(line);*/
/*		free(line_copy);*/
		continue;
		error:
		puts(fail_msg);
		return 1;
	}
	if(!clear_output)printf("<ASM> Successfully assembled %s\n", outfilename);
	if(ofile) 	fclose(ofile);
	if(infile)	fclose(infile);
	if(run_sisa16 && !quit_after_macros && !debugging){
		UU i=0, j=~(UU)0;
		SUU q_test=(SUU)-1;
		if(SEGMENT) free(SEGMENT);
		SEGMENT = NULL;
		SEGMENT_PAGES = 0;
		if(
			(sizeof(U) != 2) ||
			(sizeof(u) != 1) ||
			(sizeof(UU) != 4) ||
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
		R=0;e();
		if(R==1)puts("\n<Errfl, 16 bit div by 0>\n");
		if(R==2)puts("\n<Errfl, 16 bit mod by 0>\n");
		if(R==3)puts("\n<Errfl, 32 bit div by 0>\n");
		if(R==4)puts("\n<Errfl, 32 bit mod by 0>\n");
		if(R==5)puts("\n<Errfl, Bad Segment Page>\n");
		if(R==6){
			/*puts("\n<Errfl, Segment Cannot be Zero Pages>\n");*/
			puts("\r\n<Errfl, deprecated error>\r\n");
		}
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
			puts("\n<Errfl, Internal error, Reporting bad segment but not set that way at compiletime. Report this bug! https://github.com/gek169/Simple_ISA/   >");
#endif
		}
		if(R==15 || R==16 || R==17 || R==18 || R==19){
			puts("\n<Errfl, Privileged opcode executed underprivileged.>");
		}		
	}
	return 0;
}
