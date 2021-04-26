
/*
assembler for the ISA.
Supports doing any of the following:
1) Entering opcodes with their BYTE arguments (short args unsupported)
sa 0x53
ldb 0xFF,69
ldb 0xFF,0x69
sc 0xFF,0x39
nop
Names must be lower case and properly spelled.
Data must be separated by a comma.
2) Comments
Lines beginning with /, , or # are comments.
3) Multiple commands per line
You may have multiple commands on the same line like this:
sa 0x53;cmp;jmpifeq;
4) Macros
You may declare a macro with VAR like this
VAR#my_variable_name#0x53
You may have at most, 65,535 macros in your file.
5) Location retrieval
You may use the location of the current instruction you are writing as a variable.
It is called @ as a short, or $ as a pair of bytes.
6) Arbitrary data
You may specify arbitrary data as either BYTES or SHORTS
bytes 0xFF,10,34,7
shorts 0xff10,0x3407
You may even use macros in the expansion
bytes myvariablename myothervariablename
You may use bytes and shorts in-line as if they were commands.
bytes 5; sa 7;
7) (Mostly) Arbitrary whitespace (On a single line)
You can omit whitespace between keywords in most cases.
However some operations require that there be no whitespace.
Character literals for instance must start at the beginning of a line with no preceding whitespace.
8) Skipto
The assembler allows you to "skip" until a desired address (which may wrap around to the beginning...)
by specifying it with the SECTION tag.
If the space-in-between in the ROM does not exist it will be filled in with zeroes.
section 0x1000
9) Fill
The assembler allows you to write a byte value for a number of 
fill 0xFFFF,0
10) Strings
The assembler allows you to directly write ascii characters into your output bin.
Lines beginning with ! are character literal lines and they will be written to the current output location.
11) Assembly-time directives
%short%... converts number to a split short. It must be literal.
Assembly-time directives must have NO leading or trailing whitespace.
12) Builtin macros
You can retrieve the current location in the binary as a short with @, and as a byte pair with $
You can offset these like this: $+93+ or @+15+
13) asm_macro_call#macroname#arg1#arg2#
you can call a macro of name macroname with _arg1, _arg2, _arg3... being automatically defined for you.
14) 

*/

#include "stringutil.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
char* outfilename = "out.bin";
char* infilename = NULL;
char* variable_names[65535] = {0};
char* variable_expansions[65535] = {0};
char variable_is_redefining_flag[65535] = {0};
char* insns[128] = {
	"halt",
	"lda",
	"la",
	"ldb",
	"lb",
	"sc",
	"sta",
	"stb",
	"add",
	"sub",
	"mul",
	"div",
	"mod",
	"cmp",
	"jmpifeq",
	"jmpifneq",
	"getchar",
	"putchar",
	"and",
	"or",
	"xor",
	"lsh",
	"rsh",
	"ilda",
	"ildb",
	"cab",
	"ab",
	"ba",
	"alc",
	"ahc",
	"nop",
	"cba",
	/*Special insns for 16 bit mode*/
	"lla",
	"illda",
	"llb",
	"illdb",
	/*Self-Indirecting access mode.*/
	"illdaa",
	"illdbb",
	"illdab",
	"illdba",
	"ca",
	"cb",
	"ac",
	"bc",
	"ista",
	"istb",
	"istla",
	"istlb",
	"jmp",
	"stla",
	"stlb",
	"stc",
	"push",
	"pop",
	"pusha",
	"popa",
	"astp",
	"bstp",
	"compl",
	/*Call structure*/
	"cpc",
	"call",
	"ret",
	/*Far Memory Access*/
	"farillda",
	"faristla",	
	"farilldb",
	"faristlb",
	"farpagel",
	"farpagest",
	"lfarpc",
	"farcall",
	"farret",
	"farilda",
	"farista",
	"farildb",
	"faristb",
	"imul15.1",
	"imul14.2",
	"imul13.3",
	"imul12.4",
	"imul11.5",
	"imul10.6",
	"imul9.7",
	"imul8.8",
	"imul7.9",
	"imul6.10",
	"imul5.11",
	"imul4.12",
	"imul3.13",
	"imul2.14",
	"imul1.15",
	"imul.16",
	/*quickly push registers onto the stack*/
	"alpush",
	"blpush",
	"cpush",
	"apush",
	"bpush",
	/*quickly pop registers from the stack*/
	"alpop",
	"blpop",
	"cpop",
	"apop",
	"bpop",
};
unsigned char insns_numargs[128] = {
	0,/*halt*/
	2,1,2,1, /*load and load constant comboes, lda, la, ldb, lb*/
	2, /*load constant into C*/
	2,2, /*store A or B.*/
	0,0,0,0,0, /*the four elementary, + mod*/
	0, /*cmp*/
	0,0, /*jmp insns*/
	0,0, /*getchar,putchar,*/
	0,0,0,0,0, /*bitwise*/
	0,0, /*indirect load.*/
	0,0,0,0,0,0,0, /*Register to register moves and nop.*/
	
	2,0,2,0, /*16 bit constant loads and loads-through-c*/
	0,0,/*16 bit self-indirect loads*/
	0,0,/*16 bit neighbor-indirect loads*/
	0,0,/*move to C*/
	0,0,/*move from C*/
	0,0,0,0,/*Indirect Stores*/
	0, /*Direct jump.*/
	2,2,2, /*store shorts A,B,and C*/
	2,2,0,0, /*stack pointer ops*/
	0,0, /*retrieve the stack pointer and put it in a or b*/
	0, /*Compl*/
	/*call structure.*/
	0,/*cpc*/
	0,/*call*/
	0,/*ret*/
	0,0, /*Far memory access- a*/
	0,0, /*Far memory access- b*/
	0,0, /*Far page load and store*/
	0,0,0, /*lfarpc, farcall, farret,*/
	0,0,0,0, /*Far memory access- a and b, as single bytes*/
	/*Fixed point multiplies.*/
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	/*More stack ops*/
	0,0,0,
	0,0, 
	/*register pops*/
	0,0,0,
	0,0, 
};
char* insn_repl[128] = {
	"bytes 0;", 
	/*The direct load-and-store operations have args.*/
	"bytes 1,",
	"bytes 2,",
	"bytes 3,",
	"bytes 4,",
	"bytes 5,",
	/*stores have arguments*/
	"bytes 6,",
	"bytes 7,",
	/*have no arguments.*/
	"bytes 8;",
	"bytes 9;",
	"bytes 10;",
	"bytes 11;",
	"bytes 12;",
	"bytes 13;",
	"bytes 14;",
	"bytes 15;",
	"bytes 16;",
	"bytes 17;",
	"bytes 18;",
	"bytes 19;",
	"bytes 20;",
	"bytes 21;",
	"bytes 22;",
	"bytes 23;",
	"bytes 24;",
	"bytes 25;",
	"bytes 26;",
	"bytes 27;",
	"bytes 28;",
	"bytes 29;",
	"bytes 30;",
	"bytes 31;",
	/*16 bit mode insns*/
	"bytes 32,",
	"bytes 33;",
	"bytes 34,",
	"bytes 35;",
	/*self, neighbors indirection*/
	"bytes 36;",
	"bytes 37;",
	"bytes 38;",
	"bytes 39;",
	/*direct move to c*/
	"bytes 40;",
	"bytes 41;",
	/*direct move from c*/
	"bytes 42;",
	"bytes 43;",
	/*indirect stores*/
	"bytes 44;",
	"bytes 45;",
	"bytes 46;",
	"bytes 47;",
	"bytes 48;",
	/*Store shorts to constant address*/
	"bytes 49,",
	"bytes 50,",
	"bytes 51,",
	/*Stack pointer operations.*/
	/*Push and pop by constant amounts*/
	"bytes 52,",
	"bytes 53,",
	/*using a*/
	"bytes 54;",
	"bytes 55;",
	/*stack pointer retrieval into a or b*/
	"bytes 56;",
	"bytes 57;",
	/*compl of a*/
	"bytes 58;",
	/*cpc*/
	"bytes 59;",
	"bytes 60;",  /*call*/
	"bytes 61;",  /*ret*/
	/*far memory.*/
	"bytes 62;", /*farillda*/
	"bytes 63;", /*faristla*/
	"bytes 64;", /*farilldb*/
	"bytes 65;", 
	"bytes 66;", 
	"bytes 67;", 
	"bytes 68;", 
	"bytes 69;", 
	"bytes 70;", 
	"bytes 71;", 
	"bytes 72;", 
	"bytes 73;", 
	"bytes 74;", 
	/*Fixed point ops*/
	"bytes 75;","bytes 76;","bytes 77;","bytes 78;",
	"bytes 79;","bytes 80;","bytes 81;","bytes 82;",
	"bytes 83;","bytes 84;","bytes 85;","bytes 86;",
	"bytes 87;","bytes 88;","bytes 89;","bytes 90;",
	/*Stack ops- pushes*/
	"bytes 91;",
	"bytes 92;",
	"bytes 93;",
	/*single byte stack ops*/
	"bytes 94;",
	"bytes 95;",
	/*Stack ops- pops*/
	"bytes 96;",
	"bytes 97;",
	"bytes 98;",

	"bytes 99;",
	"bytes 100;",
};
static const unsigned char n_insns = 101;
unsigned long outputcounter = 0;
unsigned long nmacros = 5; /*0,1,2,3,4*/
char quit_after_macros = 0;
char debugging = 0;
unsigned long npasses = 0;
char printlines = 0;
unsigned long linesize = 0;char* line = NULL, *line_copy = NULL;
unsigned long region_restriction = 0;
char region_restriction_mode = 0; /*0 = off, 1 = block, 2 = region*/
void fputbyte(unsigned char b, FILE* f){
				/*if(debugging)
					printf("\nWriting individual byte %u\n", b);*/
	if((unsigned long)ftell(f) != outputcounter){
		/*seek to the end*/
		if(debugging)
			puts("Having to move the file counter.\n");
		fseek(f,0,SEEK_END);
		if((unsigned long)ftell(f) > outputcounter) /*The file is already larger than needed.*/
			fseek(f,outputcounter,SEEK_SET); 
		else /*Fill with 0's until we reach the output counter.*/
			while((unsigned long)ftell(f)!=outputcounter)fputc(0, f);
	}
	switch(region_restriction_mode){
		case 0: break;
		case 1:{
			if (((outputcounter>>8) & 0xFFFF)  != region_restriction){
				printf("<ASM COMPILATION ERROR> block restriction failed. Outputcounter exited 256 byte bounds. Line:\n%s", line_copy); 
				exit(1);
			}
		}
		break;
		case 2:{
			if (((outputcounter>>16) & 0xFF)  != region_restriction){
				printf("<ASM COMPILATION ERROR> region restriction failed. Outputcounter exited 64k bounds. Line:\n%s", line_copy);
				exit(1);
			}
		}
		break;
		default: puts("<ASM INTERNAL ERROR> invalid region_restriction_mode set somehow."); exit(1);
	}
	if(npasses == 1)
		fputc(b, f);
	outputcounter++; outputcounter&=0xffffff;
}
void fputshort(unsigned short sh, FILE* f){
	fputbyte(sh/256, f);
	fputbyte(sh, f);
}
int main(int argc, char** argv){FILE* infile,* ofile; char* metaproc;
	const unsigned long nbuiltin_macros = 6; unsigned long nmacrocalls = 0; const unsigned long maxmacrocalls = 0x10000;
	unsigned long line_num = 0;
	variable_names[0] = "@";
	variable_expansions[0] = "";
	variable_names[1] = "$";
	variable_expansions[1] = variable_expansions[0];
	variable_names[2] = "%";
	variable_expansions[2] = variable_expansions[0];
	/*Macros to remove whitespace- this assembler works without using any whitespace at all.*/
	variable_names[3] = "\t";
	variable_expansions[3] = variable_expansions[0];
	variable_names[4] = " ";
	variable_expansions[4] = variable_expansions[0];
	variable_names[5] = "  ";
	variable_expansions[5] = variable_expansions[0];
	/*Assembly-time directives.*/
	nmacros = 6;

	{int i;for(i = 2; i < argc; i++)
	{
		if(strprefix("-o",argv[i-1]))outfilename = argv[i];
		if(strprefix("-i",argv[i-1]))infilename = argv[i];
	}}
	{int i;for(i = 1; i < argc; i++)
	{
		if(strprefix("-E",argv[i])) {
			quit_after_macros = 1;
			puts("#Contents of file post macro expansion are as follows:");
		}
		if(strprefix("-DBG",argv[i])) {
			debugging = 1;
			puts("<ASM> Debugging.\n");
		}
		if(strprefix("-pl",argv[i])){
			printlines = 1;
			puts("<ASM> Printing lines.");
		}
		if(
			strprefix("-h",argv[i]) ||
			strprefix("-v",argv[i]) ||
			strprefix("--help",argv[i]) ||
			strprefix("--version",argv[i])
		){
			puts("Usage: ./asm -i infile -o outfile\n");
			puts("Optional argument: -DBG: debug the assembler. do not specify an infile if you want to use stdin.");
			puts("Optional argument: -E: Print macro expansion only do not write to file");
			puts("Optional argument: -pl: Print lines");
			puts("Optional argument: -v, -h, --help, --version: This printout.");
			puts("\n\nSISA-16 Assembler in Pure Glorious C89, Version 1.0, ");
			puts("Authored by DMHSW for the Public Domain\n\n");
			
			return 1;
		}
	}}
	if(debugging) infile=stdin;
	ofile = NULL;
	if(!quit_after_macros)
		ofile=fopen(outfilename, "w");
	if(!ofile){
		printf("\nUNABLE TO OPEN OUTPUT FILE %s!!!\n", outfilename);return 1;
	}
	if(infilename){
		if(debugging) printf("\nReading from a file...\n");
			infile = fopen(infilename, "r");
		if(!infile) {
			printf("\nUNABLE TO OPEN INPUT FILE %s!!!\n", infilename);return 1;
		}
	} else {
		if(debugging) printf("\nReading from stdin...");
		if(!debugging) {
			puts("\n<ASM ERROR> May not use reading from stdin when not in debugging mode.\n");
			puts("\n<ASM> Assembler Aborted.");
			return 1;
		}
	}
	/*Second pass to allow forward declarations.*/
	for(npasses = 0; npasses < 2; npasses++, fseek(infile, 0, SEEK_SET), outputcounter=0)
	while(!feof(infile)){
		char was_macro = 0;
		if(debugging) printf("\nEnter a line...\n");
		line = read_until_terminator_alloced(infile, &linesize, '\n', 1);
		if(!line) return 0;
		line_copy = strcatalloc(line,"");line_num++;
		/*Step 0: PRE-PRE PROCESSING. Yes, this is a thing.*/
		while(strprefix(" ",line) || strprefix("\t",line)){ /*Remove preceding whitespace... we do this twice, actually...*/
			char* line_old = line;
			line = strcatalloc(line+1,"");
			free(line_old);
		}
		if(strprefix("#",line)) goto end;
		if(strprefix("//",line)) goto end;
		if(strprefix("!",line)) {unsigned long i;
			/*We are writing out individual bytes for the rest of the line.*/
			if(debugging)
				printf("Detected Character Literal on Line:\n%s\n", line_copy);
			if(printlines)puts(line);
			if(!quit_after_macros)
				for(i = 1; i < strlen(line);i++)
					fputbyte(line[i], ofile);
			goto end;
		}
		if(strprefix("ASM_data_include ", line)){
			/*data include! format is
			[newline]asm_data_include filename
			*/
			FILE* tmp; char* metaproc; unsigned long len;
			metaproc = line + strlen("ASM_data_include ");
			tmp = fopen(metaproc, "rb");
			if(!tmp) {printf("<ASM COMPILATION ERROR> Unknown/unreachable data file %s\n", metaproc); goto error;}
			fseek(tmp, 0, SEEK_END);
			len = ftell(tmp);
			if(len > 0x1000000) {
				printf("<ASM COMPILATION ERROR> data file %s larger than entire address space.\n", metaproc); 
				goto error;
			}
			if(len == 0){
				printf("<ASM COMPILATION ERROR> data file %s is empty.\n", metaproc); 
				goto error;
			}
			fseek(tmp, 0, SEEK_SET);
			for(;len>0;len--)fputbyte(fgetc(tmp), ofile);
			fclose(tmp);
			goto end;
		}
		pre_pre_processing:
		if(nmacrocalls > maxmacrocalls){
			printf("<ASM COMPILATION ERROR> the recursion limit for macro calls has been reached.Line:\n%s\n", line_copy);
			goto error;
		}
		nmacrocalls++;
		while(strprefix(" ",line) || strprefix("\t",line)){ /*Remove preceding whitespace.*/
			char* line_old = line;
			line = strcatalloc(line+1,"");
			free(line_old);
		}

		if(strprefix("ASM_COMPILE", line)){
			puts("<ASM SYNTAX ERROR> Unimplemented feature ASM_COMPILE was used.");
			goto error;
		}
		if(strprefix("ASM_COPYRIGHT", line)){
			puts("SISA-16 Assembler,(C)David M.H.S. Webster 2021\navailable to you under the Creative Commons Zero license.");
			goto end;
		}
		if(strprefix("asm_help", line)){
			puts("This assembler should have come with a README.MD and an emulator \"isa.c\". If it did not, you got scammed! ");
			puts("There are 100 instructions in the original version of the emulator which you can invoke by name.");
			puts("This is no ordinary assembler, it has a very complicated macro syntax, arbitrary data includes, and string literals.");
			puts("There are, however, no structured programming control flow statements. you've only got the jmp, jmpifeq, jmpifneq, call, farcall, ret, and farret insns.");
			puts("The assembler is a two-pass compiler. Both passes work roughly the same. The first pass won't actually write the file, though.");
			puts("However, on the second pass, you can use variable defined from the first pass.");
			puts("This allows you to do things like define goto labels. On the first pass, the jump target will evaluate as zero, but on the second it will be defined.");
			puts("Note that it is possible to (purposefully or by mistake...) deysynchronize the two passes. That is, cause the number of bytes written to the output file to be different on different passes.");
			puts("(You can fix this on the second pass with asm_correct_outp+amount; or asm_correct_outp-amount;)");
			puts("You can try to figure out how it works from source but you really need the readme.");
		}

		
		if(
			(!strprefix("VAR#", line)) /*Do not parse asm_call's inside of a VAR# line.*/
			&& (strfind(line, "asm_call#") != -1) /*We have found one!*/
			){
			char* f, *before, *after, *content; long next_pound = -1; long found_macro_id=-1;
			unsigned long i = 0;char* macro_name=0;
			f = line + strfind(line, "asm_call#") + strlen("asm_call#");
			before = str_null_terminated_alloc(line, strfind(line, "asm_call#"));
			/*The string immediately after the first # is the macro to call.
			the arguments thereafter are arg1, arg2, arg3... */
			next_pound = strfind(f, "#");
			if(next_pound == -1) {
				printf("<ASM SYNTAX ERROR> macro call lacking end pound.Line:\n%s\n", line_copy);
				goto error;
			}
			macro_name = str_null_terminated_alloc(f, next_pound);
			/*printf("\nMacro call attempt detected on macro \'%s\'", macro_name);*/
			{
				unsigned long macro_i;
				for(macro_i = 0; macro_i < nmacros; macro_i++)
					if(streq(macro_name, variable_names[macro_i])){found_macro_id=macro_i;break;}
				if(found_macro_id == -1){
					printf("<ASM SYNTAX ERROR> macro call on non-existent macro.Line:\n%s\n", line_copy);
					goto error;
				}
			}
			content = strcatalloc(variable_expansions[found_macro_id],"");
			if(debugging) printf("\nMacro call attempt on macro \'%s\'", macro_name);
			f+=next_pound+1;
			for(i=1;*f!='\0' && *f!='#';i++){
				unsigned long macro_i;
				char namebuf[64]; /*Holds name of argXXX*/
				/*Used for defining the macro.*/
				char* varname = NULL;
				unsigned long replacements = 0;
				const unsigned long max_repl = 0x10000;
				char* vardef = NULL;
				long overwriting = -1;/*if not negative, this means it's replacing something.*/
				/*Check to see if we even can define a macro.*/
				if(nmacros == 0xffff) {
					printf("<ASM ERROR>  Too many macros. Cannot define another one. Line:\n%s\n", line_copy); goto error;
				}
				/*There is enough room for a macro.*/
				sprintf(namebuf, "_arg%lu", i);
				varname = strcatalloc(namebuf, "");
				if(debugging)
					printf("\nReplacing %s...", varname);
				/*We need to see if this macro already exists.*/
				for(macro_i = 0; macro_i < nmacros; macro_i++)
					if(streq(varname, variable_names[macro_i]))
						overwriting = macro_i; /*It does!*/
				if(overwriting == -1) {
					overwriting = nmacros++;
				}

				/*get the definition*/
				next_pound = strfind(f, "#");
				if(next_pound == -1) {
					printf("<ASM SYNTAX ERROR> macro call lacking end pound.Line:\n%s\n", line_copy);
					goto error;
				}
				vardef = str_null_terminated_alloc(f, next_pound);
				f+=next_pound+1;
				if(variable_names[overwriting]){free(variable_names[overwriting]);variable_names[overwriting] = NULL;}
				if(variable_expansions[overwriting]){free(variable_expansions[overwriting]);variable_expansions[overwriting] = NULL;}
				variable_names[overwriting] = varname;
				variable_expansions[overwriting] = vardef;
				if(debugging)
					printf("\nReplacing %s with %s", varname, vardef);
				/*replace _argXXX in content.*/
				while(strfind(content, varname)!= -1 && replacements < max_repl){
					char* content_before, *content_after;replacements++;
					if(debugging) puts("Found an instance of a required replacement!");
					content_before = str_null_terminated_alloc(content,strfind(content, varname));
					content_after = strcatalloc(content+strfind(content, varname)+strlen(varname),"");
					free(content); /**/
					content = strcatallocfb(
						content_before,
						strcatallocf2(vardef, content_after)
					);
				}
			}
			if(*f != '#'){
				printf("<ASM SYNTAX ERROR> macro call lacking end second pound.Line:\n%s\n", line_copy);
				goto error;
			}f++;
			/*Old code:
			free(line);
			line = strcatallocf2("",macro_name);
			*/
			/*Get the after.*/
			after = strcatalloc(f,"");
			line = strcatallocf1(
				before,
				strcatallocfb(content, after)
			);
			if(debugging) printf("\nThe line is now \n%s\nas a result of a macro call!\n", line);
			goto pre_pre_processing; /*Allow recursive calling.*/
		}

		if(strlen(line) < 1) goto end; /*Allow single character macros.*/
		if(strlen(line) == 1 && !isalpha(line[0])) goto end; /*Cannot possibly be a macro, it's the end of file thing.*/
		if(!isalpha(line[0]) && line[0] != ' ' && line[0] != ';' && line[0] != '\t'){
			printf("<ASM WARNING> Ignoring line beginning with illegal character... Line:\n%s\n", line_copy);
			goto end;
		}
		/*Step 1: Expand Macros on this line. This includes whitespace removal.*/
		/*Not performed on MACRO Lines.*/
		if(strfind(line,"VAR#")!= -1) was_macro=1;
		{unsigned char have_expanded = 0; unsigned short iteration = 0; long i;
			do{
				have_expanded = 0;
				if(debugging){
					printf("\n~~Macro Expansion Stage~~, iteration %u\nLine:\n%s", iteration, line);
					if(was_macro)
						puts("\n~~~~~~~~~~~~~~~This is a macro line~~~~~~~~~~~~~~~\n");
				}
				if(!was_macro) 
					if(strfind(line, "asm_call") != -1) 
						goto pre_pre_processing; /*A macro has been expanded, resulting in an asm_call forming.*/
				for(i = (was_macro?nbuiltin_macros:nmacros)-1; i>=0; i--){ /*Only check builtin macros when writing a macro.*/
					char* line_old; long loc; long linesize; char found_longer_match;
					long len_to_replace; char* before;char* after;
					long j;
					linesize = strlen(line);
					loc = strfind(line, variable_names[i]);
					if(loc == -1) continue;
					if(was_macro && i==2){
						continue; /*Do not parse the split directive inside of a macro.*/
					}
					if(was_macro && (i==3 || i==4)){
						continue; /*Do not parse the space or tab inside of a macro.*/
					}
					line_old = line;
					if(debugging)printf("\nDiscovered possible Macro \"%s\"!\n", variable_names[i]);
										/*Check to make sure that this isn't some other, longer macro.*/
					found_longer_match = 0;
					if(!was_macro)
					for(j = 0; j<(long)nmacros; j++){
						if(j == i) continue;
						if(strlen(variable_names[j]) > strlen(variable_names[i])){
							long checkme;
							checkme = strfind(line, variable_names[j]);
							if(checkme == -1) continue;
							/*Does this match contain us?*/
							if(
								checkme+strlen(variable_names[j]) >= loc + strlen(variable_names[i]) &&
								checkme <= loc
								){
									found_longer_match = 1;
									break;
								}
						}
					}
					if(debugging)
						if(found_longer_match) 
							{puts("Found longer Macro.");continue;}
					/*We know the location of a macro to be expanded and it is at loc.*/
					/*This also quit conveniently defines the recursion limit for a macro.*/
					have_expanded = 1;
					
					len_to_replace = strlen(variable_names[i]);
					before = str_null_terminated_alloc(line_old, loc);
					if(i > 2) /*0,1,2 are special cases. 3,4,X are not.*/
						before = strcatallocf1(before, variable_expansions[i]);
					else if (i == 0){ /*SYNTAX: @+7+*/
						char expansion[1024];
						unsigned long addval = 0;
						/*We need to check if there is a plus sign immediately after the at sign. */
						if(strprefix("+",line_old+loc+1)){
							char* add_text = line_old+loc+2;
							/*Find the next plus*/
							long loc_eparen = strfind(line_old+loc+2,"+");
							if(loc_eparen == -1){
								printf("<ASM SYNTAX ERROR> @ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								printf("<ASM WARNING> @ with empty add section. Line:\n%s\n", line_copy);
							}
							addval = strtoul(add_text,0,0);
							if(addval == 0 && npasses == 1)
								printf("<ASM WARNING> @ with add evaluating to zero. Line:\n%s\n", line_copy);
							if(addval)
							len_to_replace += (loc_eparen-len_to_replace+3);
						}
						addval += outputcounter;
						/*snprintf(expansion, 1023, "%lu", addval);*/
						sprintf(expansion, "%lu", addval);
						expansion[1023] = '\0'; /*Just in case...*/
						before = strcatallocf1(before, expansion);
					} else if (i==1){
						char expansion[1024]; unsigned long addval;
						
						addval = 0;
						if(strprefix("+",line_old+loc+1)){char* add_text; long loc_eparen;
							add_text = line_old+loc+2;
							/*Find the next plus*/
							loc_eparen = strfind(line_old+loc+2,"+");
							if(loc_eparen == -1){
								printf("<ASM SYNTAX ERROR> $ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								printf("<ASM WARNING> $ with empty add section. Line:\n%s\n", line_copy);
							}
							addval = strtoul(add_text,0,0);
							if(addval == 0 && npasses == 1)
								printf("<ASM WARNING> $ with add evaluating to zero. Line:\n%s\n", line_copy);
							len_to_replace += (loc_eparen-len_to_replace+3);
						}
						addval += outputcounter;
						
						/*snprintf(expansion, 1023, "%lu,%lu", (unsigned long)(addval/256),(unsigned long)(addval&0xff));*/
						sprintf(expansion, "%lu,%lu", (unsigned long)(addval/256),(unsigned long)(addval&0xff));
						expansion[1023] = '\0'; /*Just in case...*/
						before = strcatallocf1(before, expansion);
					} else if (i==2){long loc_eparen;char expansion[1024]; unsigned short res; /*Split directive.*/
						/*Locate the next ending one*/
						if(strlen(line_old+loc) == 0){
							printf("<ASM SYNTAX ERROR> SPLIT (%%) is at end of line. Line:\n%s\n", line_copy);
							goto error;
						}
						loc_eparen = strfind(line_old+loc+1,"%");
						if(loc_eparen == -1){
							printf("<ASM SYNTAX ERROR> SPLIT (%%) without ending %%. At location:\n%s\nLine:\n%s\n",line_old+loc,line_copy);
							goto error;
						}
						if(loc_eparen == 0){
							/*printf("<ASM WARNING> SPLIT (%%) is empty. At location:\n%s\nLine:\n%s\n",line_old+loc,line_copy);*/
						}
						/*the character we were going to replace anyway, plus
						the length of the stuff inbetween, plus the */
						len_to_replace+=(loc_eparen-len_to_replace+2);
						
						
						res = strtoul(line_old+loc+1, NULL, 0);
						if(res == 0  && npasses == 1 && line_old[loc+1] != '%' && line_old[loc+1] != '0')
							printf("<ASM WARNING> Unusual SPLIT (%%) evaluates to zero. Line:\n%s\nValue:\n%s\nInternal:\n%s\n", line_copy, line_old+loc+1, line_old);
						if(debugging) printf("\nSplitting value %u\n", res);
						/*Write the upper and lower halves out, separated, to expansion.*/
						/*snprintf(expansion, 1023, "%u,%u", (unsigned int)(res/256),(unsigned int)(res&0xff));*/
						sprintf(expansion, "%u,%u", (unsigned int)(res/256),(unsigned int)(res&0xff));
						before = strcatallocf1(before, expansion);
					}
					after = str_null_terminated_alloc(line_old+loc+len_to_replace, 
									linesize-loc-len_to_replace);
					line = strcatallocfb(before, after);
					free(line_old);
					break; /*we have expanded something.*/
				}
			}while(have_expanded && (iteration++ < 32768));
		}
		/*Check again to see if this is a comment line.*/
		if(strprefix("#",line)) goto end;
		if(strprefix("//",line)) goto end;
		if(strprefix("!",line) || strfind(line, "!") != -1) {
			printf("<ASM SYNTAX ERROR> Invalid character literal.");
			printf("Character literals must be on their own line with no preceding whitespace.");
			printf("\nLine:\n%s\n", line_copy);
			goto error;
		}
		
		/*Step 2: Check to see if this is a macro*/
		if(debugging){
			printf("\n~~Is this a macro?~~\n");
		}

		if(strprefix("VAR",line)){ long loc_pound, loc_pound2; char* macro_name;char is_overwriting; /*It's show time!*/
			unsigned short index;
			if(debugging){
				printf("\nThis is a macro!\n");
			}
			was_macro = 1;
			if(nmacros == 0xffff) {
				printf("<ASM ERROR>  Too many macros. Cannot define another one. Line:\n%s\n", line_copy); goto error;
			}
			loc_pound = strfind(line, "#");
			loc_pound2 = 0;
			if(loc_pound == -1) {
				printf("<ASM SYNTAX ERROR>  missing leading # in macro declaration. Line:\n%s\n", line_copy); goto error;
			}
			/*We have a macro.*/
			macro_name = line + loc_pound;
			if(debugging){
				printf("\nMacro Name Pre-Allocation is identified as %s\n", macro_name);
			}
			if(strlen(macro_name) == 1){
				printf("\n<ASM SYNTAX ERROR> missing second # in macro declaration. Line:\n%s\n", line_copy); goto error;
			}
			macro_name += 1; loc_pound += 1; /*We use these again.*/
			loc_pound2 = strfind(macro_name, "#");
			if(loc_pound2 == -1) {
				printf("\n<ASM SYNTAX ERROR> missing second # in macro declaration. Line:\n%s\n", line_copy); goto error;
			}
			macro_name = str_null_terminated_alloc(macro_name, loc_pound2);
			if(debugging){
				printf("\nMacro Name is identified as %s\n", macro_name);
			}
			if(strlen(line + loc_pound + loc_pound2) < 2){
				printf("<ASM SYNTAX ERROR> macro without a definition. Line:\n%s\n", line_copy);
				goto error;
			}
			loc_pound2++;
			/*Search to see if we've already defined this macro*/
			is_overwriting = 0;
			index = 0;
			{unsigned short i;for(i = 0; i < n_insns; i++){
				if( (strfind(insns[i],macro_name)>-1) ||
					(strfind(macro_name, insns[i])==0) 
				){
					printf("<ASM SYNTAX ERROR> This macro would prevent instruction %s from being used:\n%s\n", insns[i], line_copy);
					goto error;	
				}
			}}
			/*
				Prevent macros from being defined which contain other 
			*/
			/*
				Check and make sure this is not a reserved name
			*/
			if(
				strfind(macro_name, "!") != -1 ||
				strfind(macro_name, "//") != -1 ||
				strfind(macro_name, " ") != -1 ||
				strfind(macro_name, "\t") != -1 ||
				strfind(macro_name, ";") != -1 ||
				strfind(macro_name, "#") != -1 ||
				strfind(macro_name, "$") != -1 ||
				strfind(macro_name, "@") != -1 ||
				strfind(macro_name, "%") != -1 ||
				strfind(macro_name, "bytes") != -1 ||
				strfind(macro_name, "shorts") != -1 ||
				strfind(macro_name, "asm_") != -1 ||
				strfind(macro_name, "ASM_") != -1 ||
				strfind(macro_name, "_arg") != -1 ||
				/*begins with a number*/
				(macro_name[0] <= '9' && macro_name[0] >= '0')
				
			)
			{
				printf("<ASM SYNTAX ERROR> This macro deliberately attempts to define or trample a reserved name or character. You may not use this name:\n%s\n", macro_name);
				puts("NOTE: You cannot use names beginning with numbers, either.");
				goto error;	
			}

			if(
				strfind("!", macro_name) != -1 ||
				strfind("//", macro_name) != -1 ||
				strfind(" ", macro_name) != -1 ||
				strfind("\t", macro_name) != -1 ||
				strfind(";", macro_name) != -1 ||
				strfind("#", macro_name) != -1 ||
				strfind("$", macro_name) != -1 ||
				strfind("@", macro_name) != -1 ||
				strfind("%",macro_name) != -1 ||
				strfind("bytes",macro_name) != -1 ||
				strfind("shorts",macro_name) != -1 ||
				strfind("asm_",macro_name) != -1 ||
				strfind("ASM_", macro_name) != -1 ||
				strfind("_arg", macro_name) != -1 ||
				strfind("section", macro_name) != -1 ||
				strfind("fill", macro_name) != -1 ||
				strfind("asm_help", macro_name) != -1 ||
				strfind("asm_call", macro_name) != -1 ||
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
				printf("<ASM SYNTAX ERROR> This macro would prevent language features from being used. You may not use this name:\n%s\n", macro_name);
				goto error;	
			}
			{unsigned short i;for(i = 0; i < nmacros; i++){
				if(i < nbuiltin_macros)
					if( (strfind(variable_names[i],macro_name)>-1) ||
						streq(macro_name, variable_names[i]))
					{
						printf("<ASM SYNTAX ERROR> This macro would prevent critical macro %s from being used.Line:\n%s\n", variable_names[i], line_copy);
						goto error;	
					}
				if( (
					(strfind(variable_names[i],macro_name)>-1) ||
					(strfind(macro_name, variable_names[i])>-1)
					)
					&& !streq(macro_name, variable_names[i]))
				{
					printf("<ASM WARNING> This macro may produce a conflict with other Macro: \"%s\"Line:\n%s\n",variable_names[i], line_copy);
				}
				
				if(streq(macro_name, variable_names[i])){
					is_overwriting = 1;
					if(i < nbuiltin_macros){
						printf("<ASM SYNTAX ERROR> attempted redefinition of critical macro, Line:\n%s\n", line_copy);
						goto error;	
					}
					index = i;
				}
			}}
			if(!is_overwriting){
				variable_names[nmacros] = macro_name;
				variable_expansions[nmacros++] = 
				str_null_terminated_alloc(
						line+loc_pound+loc_pound2,
						strlen(line+loc_pound+loc_pound2)
				);
			} else {char* temp;
				if(npasses == 0)
					{
						printf("<ASM WARNING> redefining macro, potentially desyncing compilation. Check that. line: %s\n", line_copy);
						variable_is_redefining_flag[index] = 1;
					}
				
				variable_names[index] = macro_name;
				temp = 
				str_null_terminated_alloc(
						line+loc_pound+loc_pound2,
						strlen(line+loc_pound+loc_pound2)
				);
				if(npasses == 1 && 
					!variable_is_redefining_flag[index] && 
					!strprefix("_arg", variable_names[index])
					)
				{/*Ensure that the macro evaluates to the exact same piece of text as the last time.*/

					if(!streq(temp, variable_expansions[index])){
						printf("<ASM HUGE WARNING>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nConfirmed!!! Macro Desynchronization between passes Line:\n%s\nInternally:\n%s\n",line_copy,line);
						free(variable_expansions[index]);
						variable_expansions[index] = temp;
					}
					free(variable_expansions[index]);
					variable_expansions[index] = temp;
				} else {
					free(variable_expansions[index]);
					variable_expansions[index] = temp;
				}
			}
			if(debugging){
				printf("\nMacro Contents are %s, size %u\n", variable_expansions[nmacros-1], (unsigned int)strlen(variable_expansions[nmacros-1]));
				printf("\nMacro Name is %s, size %u\n", variable_names[nmacros-1], (unsigned int)strlen(variable_names[nmacros-1]));
			}
		}
		if(quit_after_macros || debugging) {
			printf("\n#Line post macro expansion and evaluation:\n");
			puts(line); 
			if(quit_after_macros) goto end;
		}

		if(was_macro) goto end;
		/*We must first determine if this line contains a line comment. Don't search past the line comment for insns.*/
		if(strfind(line, "#") != -1){
			long loc_line_comment = strfind(line, "#");
			line[loc_line_comment] = '\0';
		} else if(strfind(line, "//") != -1){
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
		
			{unsigned char have_expanded = 0; unsigned short iteration = 0;
			do{
				have_expanded = 0;
				if(debugging){
					printf("\n~~Insn Expansion Stage~~, iteration %u\nLine:\n%s", iteration, line);
				}
				{unsigned long i;for(i = 0; i<n_insns; i++){char* line_old; long loc, linesize; unsigned long j;
					char found_longer_match; int num_commas_needed;
					linesize = strlen(line);
					loc = strfind(line, insns[i]);					
					line_old = line;
					if(loc == -1) continue;

					/*Check to make sure that this isn't some other, longer insn.*/
					found_longer_match = 0;
					for(j = 0; j<n_insns; j++){
						if(j == i) continue;
						if(strlen(insns[j]) > strlen(insns[i])){
							long checkme = strfind(line, insns[j]);
							if(checkme == -1) continue;
							/*Does this match contain us?*/
							if(
								checkme+strlen(insns[j]) >= loc + strlen(insns[i]) &&
								checkme <= loc
								){if(debugging) puts("Found longer insn.");
									found_longer_match = 1;
									break;
								}
						}
					}
					if(found_longer_match) continue;
					if(loc>0){
						/*Check to make sure this isn't prefixed by something which is obviously erroneous.*/
						if(*(line+loc-1) != ';')
							printf("<ASM WARNING> Instruction parsing reached with "
									"unusual prefixing character \'%c\'. May be syntax error. Instruction to parse is %s. Line:\n%s\nInternally:\n%s\n",*(line+loc-1),insns[i],line_copy, line);
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
						printf("<ASM WARNING> Instruction parsing reached with "
								"unusual postfixing character \'%c\'. May be syntax error. Instruction to parse is %s. Line:\n%s\nInternally:\n%s\n",*(line+loc+strlen(insns[i])),insns[i],line_copy,line);
					/*We know the location of an insn to be expanded and it is at loc.*/
					/*Crawl along the string. */
					num_commas_needed = insns_numargs[i] - 1;
					/*If you have three arguments, you only need two commas.*/
					for(j = loc + strlen(insns[i]); strlen(line+j)>0;j++){
						if(line[j] == ','){
							if(num_commas_needed > 0) num_commas_needed--;
							else { /*Too many commas.*/
								printf("\n<ASM SYNTAX ERROR> too many arguments to insn, line %s", line_copy);
								goto error;
							}
						}
						if(line[j] == ';' || line[j] == '\n' || line[j] == '\0'){
							if(num_commas_needed > 0)
							{ /*Too many commas.*/
								printf("\n<ASM SYNTAX ERROR> Insn requires more arguments. line %s", line_copy);
								goto error;
							}
							break; /*Out of this for loop.*/
						}
					}
					if(num_commas_needed > 0)
					{ /*Too many commas.*/
						printf("\n<ASM SYNTAX ERROR> Insn requires more arguments. line %s", line_copy);
						goto error;
					}
					/**/
					have_expanded = 1;
					{char* before; char* after; long len_to_replace;
					len_to_replace = strlen(insns[i]);
					before = str_null_terminated_alloc(line_old, loc);
					before = strcatallocf1(before, insn_repl[i]);
					after = str_null_terminated_alloc(line_old+loc+len_to_replace, 
									linesize-loc-len_to_replace);
					line = strcatallocfb(before, after);
					free(line_old);
					}
				}}
			}while(have_expanded && (iteration++ < 32700)); /*Probably safe right?*/
		}
		/*
			Put out bytes.
		*/
		/*Inch along*/
		if(printlines){
			printf("%s\n",line);
		}
		metaproc = line; /*declaration at beginning of main.*/
		do{ long incr, incrdont;
			if(strprefix("bytes", metaproc)){ char* proc;
				proc = metaproc + 5;
				do{ 
					unsigned char byteval; unsigned long preval;
					preval = strtoul(proc,NULL,0);byteval = preval & 255;
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
					shortval = strtoul(proc,NULL,0);
					if(debugging)
						printf("\nWriting short %u\n", shortval);
					fputshort(shortval, ofile);
					/*Find the next comma.*/
					incr = strfind(proc, ",");
					incrdont = strfind(proc, ";");
					if(incr == -1) break;
					if(incrdont != -1 &&
						incr > incrdont) break; /**/
					proc += incr; proc += 1; /*Skip the comma itself.*/
				}while(strlen(proc) > 0);
			} else if(strprefix("section", metaproc)){ unsigned long dest;
				char* proc = metaproc + 7;
				if(strlen(proc) == 0){
					puts("<ASM SYNTAX ERROR> Cannot have empty SECTION tag.");
				}
				dest = strtoul(proc, NULL, 0);
				if(dest == 0){
				/*Explicitly check to see if they actually typed zero.*/
					if(proc[0]!='0'  && npasses == 1)
					printf("<ASM WARNING> Section tag at zero. Might be a bad number. Line %s\n", line_copy);

				}
				if(debugging)
					printf("Moving the output counter to %lu\n", dest);
				outputcounter = dest;
			} else if(strprefix("fill", metaproc)){ unsigned long fillsize; long next_comma; unsigned char fillval;
				char* proc = metaproc + 4;
				if(strlen(proc) == 0){
					puts("<ASM SYNTAX ERROR> Cannot have empty fill tag.");
					goto error;
				}
				fillsize = strtoul(proc, NULL, 0);
				if(fillsize == 0){
					if(proc[0]!='0') /*Check if they actually typed zero.*/
					printf("<ASM WARNING> fill tag size is zero. Might be a bad number. Line %s", line_copy);
				}
				/**/
				next_comma = strfind(proc, ",");
				if(next_comma == -1) {
					printf("<ASM SYNTAX ERROR> Fill tag missing value. Line %s", line_copy);
					goto error;
				}
				proc += next_comma + 1;
				fillval = strtoul(proc, NULL, 0);
				if(fillval == 0) /*potential for a mistake*/
					if(proc[0]!='0') /*Did they actually MEAN zero?*/
						printf("<ASM WARNING> fill tag value is zero. Might be a bad number. Line:\n%s\n", line_copy);
				for(;fillsize>0;fillsize--)fputbyte(fillval, ofile);
			} else if(strprefix("asm_fix_outputcounter", metaproc)){ /*Perform a second-pass correction of the output counter.*/
				char* proc; char mode; unsigned long outputcounterold;
				if(npasses == 1){
					proc = metaproc + strlen("asm_fix_outputcounter");
					if(proc[0] == '+') mode=0; else
					if(proc[0] == '-') mode=1; else
					{
						printf("\n<ASM SYNTAX ERROR> asm_correct_outp lacks a + or -.Line:\n%s\n", line_copy);
						goto error;
					}proc++;
					outputcounterold = outputcounter;
					if(!mode)
						outputcounter += strtoul(proc, NULL, 0);
					else
						outputcounter -= strtoul(proc, NULL, 0);
					outputcounter &= 0xffFFff;
					printf("<ASM WARNING> output counter was:%lx, now: %lx. Line:\n%s\n", outputcounterold, outputcounter, line_copy);
				}
			} else if(strprefix("asm_print", metaproc)){
				if(npasses == 1)
					printf("\nRequest to print status at this insn. STATUS:\nLine:\n%s\nLine Internally:\n%s\nCounter: %04lx\n", line_copy, line, outputcounter);
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
			} else if(strprefix("asm_end_block_restriction", metaproc)|| 
					strprefix("asm_end_page_restriction", metaproc)||
					strprefix("asm_end_restriction", metaproc)
				)
			{
				region_restriction_mode = 0; /*end block*/
			} else if(strprefix("asm_vars", metaproc)){
				unsigned long i;
				printf("\nSTATUS:\nLine:\n%s\nLine Internally:\n%s\nCounter: %04lx\n", line_copy, line, outputcounter);
				printf("<DUMPING SYMBOL TABLE ON PASS %ld>\n", npasses+1);
				for(i=0;i<nmacros;i++){
					if(i==nmacros) puts("Showing User Macros");
					if(strprefix("_arg", variable_names[i])) printf("\n<auto>:");
					if(i==0){char expansion[1024];
						sprintf(expansion, "%lu", outputcounter);
						expansion[1023] = '\0'; /*Just in case...*/
						printf("VAR#%s#%s\n",variable_names[i],expansion);
					} else if(i==1){char expansion[1024];
						sprintf(expansion, "%lu,%lu", (unsigned long)(outputcounter/256),(unsigned long)(outputcounter&0xff));
						expansion[1023] = '\0'; /*Just in case...*/
						printf("VAR#%s#%s\n",variable_names[i],expansion);	
					}else
						printf("VAR#%s#%s\n",variable_names[i],variable_expansions[i]);
				}
			} else if(strprefix("asm_quit", metaproc)){
				if(npasses == 1)
					printf("\nRequest to halt assembly at this insn. STATUS:\nLine:\n%s\nCounter: %04lx\n", line_copy, outputcounter);
				goto error;
			}else if(strprefix("!", metaproc)){
				printf("<ASM SYNTAX ERROR> Cannot put string literal here, Line:\n%s\nInternal:\n%s\n", line_copy, metaproc);
			}else if (strprefix("#", metaproc) || strprefix("//", metaproc)){
				if(debugging)
					printf("<ASM WARNING> Inline Comment invalidates rest of line, Line:\n%s\nInternal:\n%s\n", line_copy, metaproc);
				break; /*Comment on line.*/
			} else {
				if(strlen(metaproc) > 0 &&
					strfind(metaproc,";") != 0){
					printf("<ASM SYNTAX ERROR> invalid statement on line %s\nStatement:%s\n", line_copy, metaproc);
					goto error;
				}
			}
			{long next_semicolon = strfind(metaproc, ";");
			if(next_semicolon == -1) break; /*We have handled all sublines.*/
			metaproc += next_semicolon + 1;
			if(strlen(metaproc) == 0) break; /*Convenient line break*/
			}
		} while(1);
		end:
		free(line);
		free(line_copy);
		continue;
		/*Yikes!*/
		error:
		puts("<ASM> Assembler Aborted.\n");
		return 1;
	}
	printf("<ASM> Successfully assembled %s\n", outfilename);
	fclose(ofile);
	fclose(infile);
	return 0;
}
