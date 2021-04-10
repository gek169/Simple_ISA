

//assembler for the ISA.
//Supports doing any of the following:
//1) Entering opcodes with their BYTE arguments (short args unsupported)
//sa 0x53
//ldb 0xFF,69
//ldb 0xFF,0x69
//sc 0xFF,0x39
//nop
//Names must be lower case and properly spelled.
//Data must be separated by a comma.
//2) Comments
//Lines beginning with /, //, or # are comments.
//3) Multiple commands per line
//You may have multiple commands on the same line like this:
//sa 0x53;cmp;jmpifeq;
//4) Macros
//You may declare a macro with VAR like this
//VAR#my_variable_name#0x53
//You may have at most, 65,535 macros in your file.
//5) Location retrieval
//You may use the location of the current instruction you are writing as a variable.
//It is called @ as a short, or $ as a pair of bytes.
//6) Arbitrary data
//You may specify arbitrary data as either BYTES or SHORTS
//bytes 0xFF,10,34,7
//shorts 0xff10,0x3407
//You may even use macros in the expansion
//bytes myvariablename myothervariablename
//You may use bytes and shorts in-line as if they were commands.
//bytes 5; sa 7;
//7) (Mostly) Arbitrary whitespace (On a single line)
//You can omit whitespace between keywords in most cases.
//However some operations require that there be no whitespace.
//Character literals for instance must start at the beginning of a line with no preceding whitespace.
//8) Skipto
//The assembler allows you to "skip" until a desired address (which may wrap around to the beginning...)
//by specifying it with the SECTION tag.
//If the space-in-between in the ROM does not exist it will be filled in with zeroes.
//section 0x1000
//9) Fill
//The assembler allows you to write a byte value for a number of 
//fill 0xFFFF,0
//10) Strings
//The assembler allows you to directly write ascii characters into your output bin.
//Lines beginning with ! are character literal lines and they will be written to the current output location.
//11) Assembly-time directives
//%short%... converts number to a split short. It must be literal.
//Assembly-time directives must have NO leading or trailing whitespace.
//12) Builtin macros
//You can retrieve the current location in the binary as a short with @, and as a byte pair with $
//You can offset these like this: $+93+ or @+15+


#include "stringutil.h"
#include <stdio.h>
#include <stdint.h>
char* outfilename = "out.bin";
char* infilename = NULL;
char* variable_names[65535] = {0};
char* variable_expansions[65535] = {0};
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
};
unsigned char insns_numargs[128] = {
	0,//halt
	2,1,2,1, //load and load constant comboes, lda, la, ldb, lb
	2, //load constant into C
	2,2, //store A or B.
	0,0,0,0,0, //the four elementary, + mod
	0, //cmp
	0,0, //jmp insns
	0,0, //getchar,putchar,
	0,0,0,0,0, //bitwise
	0,0, //indirect load.
	0,0,0,0,0,0,0, //Register to register moves and nop.
	
	2,0,2,0, //16 bit constant loads and loads-through-c
	0,0,//16 bit self-indirect loads
	0,0,//16 bit neighbor-indirect loads
	0,0,//move to C
	0,0,//move from C
	0,0,0,0,//Indirect Stores
	0, //Direct jump.
	2,2,2, //store shorts A,B,and C
	2,2,0,0, //stack pointer ops
	0,0, //retrieve the stack pointer and put it in a or b
	0, //Compl
	//call structure.
	0,//cpc
	0,//call
	0,//ret
	0,0, //Far memory access- a
	0,0, //Far memory access- b
	0,0, //Far page load and store
	0,0,0, //lfarpc, farcall, farret,
	0,0,0,0, //Far memory access- a and b, as single bytes
	/*Fixed point multiplies.*/
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	/*More stack ops*/
	0,0,0,
	0,0, //single byte pushes
};
char* insn_repl[128] = {
	"bytes 0;", //Halt has no arguments.
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
	"bytes 60;", //call
	"bytes 61;", //ret
	/*far memory.*/
	"bytes 62;", //farillda
	"bytes 63;", //faristla
	"bytes 64;", //farilldb
	"bytes 65;", //faristlb
	"bytes 66;", //farpagel
	"bytes 67;", //farpagest
	"bytes 68;", //lfarpc
	"bytes 69;", //farcall
	"bytes 70;", //farret
	"bytes 71;", //farret
	"bytes 72;", //farret
	"bytes 73;", //farret
	"bytes 74;", //farret
	/*Fixed point ops*/
	"bytes 75;","bytes 76;","bytes 77;","bytes 78;",
	"bytes 79;","bytes 80;","bytes 81;","bytes 82;",
	"bytes 83;","bytes 84;","bytes 85;","bytes 86;",
	"bytes 87;","bytes 88;","bytes 89;","bytes 90;",
	/*Stack ops*/
	"bytes 91;",
	"bytes 92;",
	"bytes 93;",
	/*single byte stack ops*/
	"bytes 94;",
	"bytes 95;",
};
static const unsigned char n_insns = 96;
unsigned int outputcounter = 0;
unsigned int nmacros = 4; /*0,1,2,3*/
char quit_after_macros = 0;
char debugging = 0;
char printlines = 0;

void fputbyte(unsigned char b, FILE* f){
				if(debugging)
					printf("\nWriting individual byte %u\n", b);
if(ftell(f) != outputcounter){
	/*seek to the end*/
	if(debugging)
		puts("Having to move the file counter.\n");
	fseek(f,0,SEEK_END);
	if(ftell(f) > outputcounter) /*The file is already larger than needed.*/
		fseek(f,outputcounter,SEEK_SET); 
	else /*Fill with 0's until we reach the output counter.*/
		while(ftell(f)!=outputcounter)fputc(0, f);
}

fputc(b, f); outputcounter++; outputcounter&=0xffffff;
}
void fputshort(unsigned short sh, FILE* f){
	fputbyte(sh/256, f);
	fputbyte(sh, f);
}
int main(int argc, char** argv){
	variable_names[0] = "@";
	variable_names[1] = "$";
	variable_names[2] = "%";
	/*Macro to remove whitespace- this assembler works without using any whitespace at all.*/
	variable_names[3] = "\t";
	variable_expansions[3] = "";
	variable_names[4] = " ";
	variable_expansions[4] = "";
	/*Assembly-time directives.*/
	nmacros = 5;
	const unsigned long long nbuiltin_macros = 5;
	unsigned long long line_num = 0;
	for(int i = 2; i < argc; i++)
	{
		if(strprefix("-o",argv[i-1]))outfilename = argv[i];
		if(strprefix("-i",argv[i-1]))infilename = argv[i];
	}
	for(int i = 1; i < argc; i++)
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
			puts("Optional argument: -DBG: debug the assembler");
			puts("Optional argument: -E: Print macro expansion only do not write to file");
			puts("Optional argument: -pl: Print lines");
			puts("Optional argument: -v, -h, --help, --version: This printout.");
			puts("\n\nAuthored by DMHSW for the Public Domain\n\n");
			return 1;
		}
	}
	FILE* infile;
	if(debugging) infile=stdin;
	FILE* ofile = NULL;
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
	while(!feof(infile)){
		unsigned long long linesize;
		char was_macro = 0;
		if(debugging) printf("\nEnter a line...\n");
		char* line = read_until_terminator_alloced(infile, &linesize, '\n', 1);
		if(!line) return 0;
		char* line_copy = strcatalloc(line,"");line_num++;
		/*Step 0: Skip comment lines*/
		if(strprefix("#",line)) goto end;
		if(strprefix("//",line)) goto end;
		if(strprefix("!",line)) {
			/*We are writing out individual bytes for the rest of the line.*/
			if(debugging)
				printf("Detected Character Literal on Line:\n%s\n", line_copy);
			if(printlines)puts(line);
			if(!quit_after_macros)
				for(unsigned long long i = 1; i < strlen(line);i++)
					fputbyte(line[i], ofile);
			goto end;
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
		{unsigned char have_expanded = 0; unsigned short iteration = 0;
			do{
				have_expanded = 0;
				if(debugging){
					printf("\n~~Macro Expansion Stage~~, iteration %u\nLine:\n%s", iteration, line_copy);
				}
				for(unsigned int i = 0; i<(was_macro?nbuiltin_macros:nmacros); i++){ /*Only check builtin macros when writing a macro.*/
					long long linesize = strlen(line);
					long long loc = strfind(line, variable_names[i]);
					if(loc == -1) continue;
					char* line_old = line;

										/*Check to make sure that this isn't some other, longer macro.*/
					char found_longer_match = 0;
					if(!was_macro)
					for(unsigned int j = 0; j<nmacros; j++){
						if(j == i) continue;
						if(strlen(variable_names[j]) > strlen(variable_names[i])){
							long long checkme = strfind(line, variable_names[j]);
							if(checkme == -1) continue;
							/*Does this match contain us?*/
							if(
								checkme+strlen(variable_names[j]) >= loc + strlen(variable_names[i]) &&
								checkme <= loc
								){if(debugging) puts("Found longer Macro.");
									found_longer_match = 1;
									break;
								}
						}
					}
					if(found_longer_match) continue;
					/*We know the location of a macro to be expanded and it is at loc.*/
					/*This also quit conveniently defines the recursion limit for a macro.*/
					have_expanded = 1;
					long long len_to_replace = strlen(variable_names[i]);
					char* before = str_null_terminated_alloc(line_old, loc);
					if(i > 2)
						before = strcatallocf1(before, variable_expansions[i]);
					else if (i == 0){ //SYNTAX: @+7+
						char expansion[1024];
						unsigned long addval = 0;
						/*We need to check if there is a plus sign immediately after the at sign. */
						if(strprefix("+",line_old+loc+1)){
							char* add_text = line_old+loc+2;
							/*Find the next plus*/
							long long loc_eparen = strfind(line_old+loc+2,"+");
							if(loc_eparen == -1){
								printf("<ASM SYNTAX ERROR> @ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								printf("<ASM WARNING> @ with empty add section. Line:\n%s\n", line_copy);
							}
							addval = strtoull(add_text,0,0);
							if(addval == 0)
								printf("<ASM WARNING> @ with add evaluating to zero. Line:\n%s\n", line_copy);
							if(addval)
							len_to_replace += (loc_eparen-len_to_replace+3);
						}
						addval += outputcounter;
						snprintf(expansion, 1023, "%lu", addval);
						expansion[1023] = '\0'; /*Just in case...*/
						before = strcatallocf1(before, expansion);
					} else if (i==1){
						char expansion[1024];
						unsigned long addval = 0;

						if(strprefix("+",line_old+loc+1)){
							char* add_text = line_old+loc+2;
							/*Find the next plus*/
							long long loc_eparen = strfind(line_old+loc+2,"+");
							if(loc_eparen == -1){
								printf("<ASM SYNTAX ERROR> $ with no ending plus. Line:\n%s\n", line_copy);
								goto error;
							}
							if(loc_eparen == 0){
								printf("<ASM WARNING> $ with empty add section. Line:\n%s\n", line_copy);
							}
							addval = strtoull(add_text,0,0);
							if(addval == 0)
								printf("<ASM WARNING> $ with add evaluating to zero. Line:\n%s\n", line_copy);
							len_to_replace += (loc_eparen-len_to_replace+3);
						}
						addval += outputcounter;
						
						snprintf(expansion, 1023, "%lu,%lu", (unsigned long)(addval/256),(unsigned long)(addval&0xff));
						expansion[1023] = '\0'; /*Just in case...*/
						before = strcatallocf1(before, expansion);
					} else if (i==2){ /*Split directive.*/
						/*Locate the next ending one*/
						if(strlen(line_old+loc) == 0){
							printf("<ASM SYNTAX ERROR> SPLIT (%%) is at end of line. Line:\n%s\n", line_copy);
							goto error;
						}
						long long loc_eparen = strfind(line_old+loc+1,"%");
						if(loc_eparen == -1){
							printf("<ASM SYNTAX ERROR> SPLIT (%%) without ending %%. At location:\n%s\nLine:\n%s\n",line_old+loc,line_copy);
							goto error;
						}
						if(loc_eparen == 0){
							printf("<ASM WARNING> SPLIT (%%) is empty. At location:\n%s\nLine:\n%s\n",line_old+loc,line_copy);
						}
						/*the character we were going to replace anyway, plus
						the length of the stuff inbetween, plus the */
						len_to_replace+=(loc_eparen-len_to_replace+2);
						
						char expansion[1024];
						unsigned short res = strtoull(line_old+loc+1, NULL, 0);
						if(res == 0)
							printf("<ASM WARNING> Unusual SPLIT (%%) evaluates to zero. Line:\n%s\n", line_copy);
						if(debugging) printf("\nSplitting value %u\n", res);
						/*Write the upper and lower halves out, separated, to expansion.*/
						snprintf(expansion, 1023, "%u,%u", (unsigned int)(res/256),(unsigned int)(res&0xff));
						before = strcatallocf1(before, expansion);
					}
					char* after = str_null_terminated_alloc(line_old+loc+len_to_replace, 
									linesize-loc-len_to_replace);
					line = strcatallocfb(before, after);
					free(line_old);
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

		if(strprefix("VAR",line)){ /*It's show time!*/
			if(debugging){
				printf("\nThis is a macro!\n");
			}
			was_macro = 1;
			if(nmacros == 0xffff) {
				printf("<ASM ERROR>  Too many macros. Cannot define another one. Line:\n%s\n", line_copy); goto error;
			}
			long long loc_pound = strfind(line, "#");
			long long loc_pound2 = 0;
			if(loc_pound == -1) {
				printf("<ASM SYNTAX ERROR>  missing leading # in macro declaration. Line:\n%s\n", line_copy); goto error;
			}
			/*We have a macro.*/
			char* macro_name = line + loc_pound;
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
			char is_overwriting = 0;
			unsigned short index = 0;
			for(unsigned short i = 0; i < n_insns; i++){
				if( (strfind(insns[i],macro_name)>-1) ||
				streq(macro_name, insns[i])){
					printf("<ASM SYNTAX ERROR> This macro would prevent instruction %s from being used:\n%s\n", insns[i], line_copy);
					goto error;	
				}
			}
			for(unsigned short i = 0; i < nmacros; i++){
				if(i < nbuiltin_macros)
					if( (strfind(variable_names[i],macro_name)>-1) ||
						streq(macro_name, variable_names[i]))
					{
						printf("<ASM SYNTAX ERROR> This macro would prevent critical macro %s from being used.Line:\n%s\n", variable_names[i], line_copy);
						goto error;	
					}
				if(streq(macro_name, variable_names[i])){
					//printf("<ASM WARNING> redefinition of macro, line: %s\n", line_copy);
					is_overwriting = 1;
					if(i < nbuiltin_macros){
						printf("<ASM SYNTAX ERROR> attempted redefinition of critical macro, Line:\n%s\n", line_copy);
						goto error;	
					}
					index = i;
				}
			}
			if(!is_overwriting){
				variable_names[nmacros] = macro_name;
				variable_expansions[nmacros++] = 
				str_null_terminated_alloc(
						line+loc_pound+loc_pound2,
						strlen(line+loc_pound+loc_pound2)
				);
			} else {
				printf("<ASM WARNING> redefining macro, line: %s\n", line_copy);
				variable_names[index] = macro_name;
				variable_expansions[index] = 
				str_null_terminated_alloc(
						line+loc_pound+loc_pound2,
						strlen(line+loc_pound+loc_pound2)
				);
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
					printf("\n~~Ins Expansion Stage~~, iteration %u\nLine:\n%s", iteration, line);
				}
				for(unsigned int i = 0; i<n_insns; i++){
					long long linesize = strlen(line);
					long long loc = strfind(line, insns[i]);
					if(loc == -1) continue;
					char* line_old = line;
					/*Check to make sure that this isn't some other, longer insn.*/
					char found_longer_match = 0;
					for(unsigned int j = 0; j<n_insns; j++){
						if(j == i) continue;
						if(strlen(insns[j]) > strlen(insns[i])){
							long long checkme = strfind(line, insns[j]);
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
					/*We know the location of an insn to be expanded and it is at loc.*/
					/*Crawl along the string. */
					int num_commas_needed = insns_numargs[i] - 1;
					//If you have three arguments, you only need two commas.
					for(unsigned int j = loc + strlen(insns[i]); strlen(line+j)>0;j++){
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
					long long len_to_replace = strlen(insns[i]);
					char* before = str_null_terminated_alloc(line_old, loc);
					before = strcatallocf1(before, insn_repl[i]);
					char* after = str_null_terminated_alloc(line_old+loc+len_to_replace, 
									linesize-loc-len_to_replace);
					line = strcatallocfb(before, after);
					free(line_old);
				}
			}while(have_expanded && (iteration++ < 32700)); /*Probably safe right?*/
		}
		/*
			Put out bytes.
		*/
		/*Inch along*/
		if(printlines){
			printf("%s\n",line);
		}
		char* metaproc = line;
		do{
			if(strprefix("bytes", metaproc)){
				
				char* proc = metaproc + 5;
				do{
					unsigned char byteval;
					byteval = strtoull(proc,NULL,0);
					fputbyte(byteval, ofile);
					/*Find the next comma.*/
					long long incr = strfind(proc, ",");
					long long incrdont = strfind(proc, ";");
					if(incr == -1) break;
					if(incrdont != -1 &&
						incr > incrdont)break;/**/
					proc += incr; proc += 1; /*Skip the comma itself.*/
				}while(strlen(proc) > 0);
				
			} else if(strprefix("shorts", metaproc)){
				char* proc = metaproc + 6;
				do{
					unsigned short shortval;
					shortval = strtoull(proc,NULL,0);
					if(debugging)
						printf("\nWriting short %u\n", shortval);
					fputshort(shortval, ofile);
					/*Find the next comma.*/
					long long incr = strfind(proc, ",");
					long long incrdont = strfind(proc, ";");
					if(incr == -1) break;
					if(incrdont != -1 &&
						incr > incrdont) break; /**/
					proc += incr; proc += 1; /*Skip the comma itself.*/
				}while(strlen(proc) > 0);
			} else if(strprefix("section", metaproc)){
				char* proc = metaproc + 7;
				if(strlen(proc) == 0){
					puts("<ASM SYNTAX ERROR> Cannot have empty SECTION tag.");
				}
				unsigned long dest = strtoull(proc, NULL, 0);
				if(dest == 0){
				/*Explicitly check to see if they actually typed zero.*/
					if(proc[0]!='0')
					printf("<ASM WARNING> Section tag at zero. Might be a bad number. Line %s\n", line_copy);

				}
				if(debugging)
					printf("Moving the output counter to %u\n", dest);
				outputcounter = dest;
			} else if(strprefix("fill", metaproc)){
				char* proc = metaproc + 4;
				if(strlen(proc) == 0){
					puts("<ASM SYNTAX ERROR> Cannot have empty fill tag.");
					goto error;
				}
				unsigned long fillsize = strtoull(proc, NULL, 0);
				if(fillsize == 0){
					if(proc[0]!='0') /*Check if they actually typed zero.*/
					printf("<ASM WARNING> fill tag size is zero. Might be a bad number. Line %s", line_copy);
				}
				/**/
				long long next_comma = strfind(proc, ",");
				if(next_comma == -1) {
					printf("<ASM SYNTAX ERROR> Fill tag missing value. Line %s", line_copy);
					goto error;
				}
				proc += next_comma + 1;
				unsigned char fillval = strtoull(proc, NULL, 0);
				if(fillval == 0) /*potential for a mistake*/
					if(proc[0]!='0') /*Did they actually MEAN zero?*/
						printf("<ASM WARNING> fill tag value is zero. Might be a bad number. Line %s", line_copy);
				for(;fillsize>0;fillsize--)fputbyte(fillval, ofile);
			} else if(strprefix("asm_print", metaproc)){
				printf("\nRequest to print status at this insn. STATUS:\nLine:\n%s\nLine Internally:\n%s\nCounter: %04x\n", line_copy, line, outputcounter);
			} else if(strprefix("asm_quit", metaproc) || strprefix("asm_quit", metaproc)){
				printf("\nRequest to halt assembly at this insn. STATUS:\nLine:\n%s\nCounter: %04x\n", line_copy, outputcounter);
				goto error;
			}else if (strprefix("#", metaproc) || strprefix("//", metaproc) || strprefix("!", metaproc)){
				if(debugging)
					printf("<ASM WARNING> Inline Comment invalidates rest of line, Line:\n%s\nInternal:\n%s\n", line_copy, metaproc);
				break; /*Comment on line.*/
			} else {
				if(strlen(metaproc) > 0 &&
					strfind(metaproc,";") != 0)
					printf("<ASM WARNING> Ignoring invalid statement on line %s\nStatement:%s\n", line_copy, metaproc);
			}
			long long next_semicolon = strfind(metaproc, ";");
			if(next_semicolon == -1) break; /*We have handled all sublines.*/
			metaproc += next_semicolon + 1;
			if(strlen(metaproc) == 0) break; /*Convenient line break*/
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
