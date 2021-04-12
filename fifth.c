#include <stdio.h>
#include "stringutil.h"

/*
Compiler for the "fifth" programming language.

Compiles into compliant loads and stores.

1) VARIABLE DECLARATIONS
	if global:
		short[memory location] myvariable
		byte[memory location] myvariable
		short[memory location] myvariable[arraysize]
		byte[memory location] myvariable[arraysize]
		short*[memory location] myvariable;
		byte*[memory location] myvariable;
	if on the stack
		short myvariable
		byte myvariable
		short* myvariable;
		byte* myvariable;
	You may not declare arrays on the stack.
	the name of the array is henceforth evaluated as a compiletime-constant pointer to its first element.
	you may declare compiletime constants with:
	const short myvariable;
2) PROCEDURE DECLARATIONS
returntype myfunction(argument description)
	<STATEMENT>
By example:
short add3 (short a, byte b, short c){
#a, b, and c are pointers to their respective elements.
#if loads are performed on compiletime-constant indexes,
#the compiler will optimize them out.
#
	@a @b + @c + return;
}
#Offset store example.
#writes 5 into a[3]
short writeshort(short* a){
	5 3 @a !_ 0 return;
}
void offsetwriteshort(short* a, short offset){
#!_ requires pointer, short(index), value.
#the typeid of value must be a.typeid.pointer_level-1
#(the first argument is first tested to see if its pointer_level is zero)
#the 
	@a @offset 3 !_ return;
}
short not(short a){
	@a ~ return;
}
short* to_pointer(byte high, short low){
#demonstrating the to-pointer syntax.
#a pointer is just two shorts with the uppermost byte being zero.
	@high @low join(short*) return;
}
short pointer_to_short(short* a){
	@a split return;
}
byte pointer_high_byte_retrieval(short* a){
	@a split void return;
}

void pointer_write(short* a){
#write 3 into the location pointed to by a.
	3 0 @a !_;
}
void variable_write(short a){
	short q;
#the topmost short on the stack is written to q,
#which is the result of adding a and 3.
	@a 3 + !q;
#after every statement, any excess stack from the previous statement is removed.
#the one short on the stack is thrown away.
#this is a void function so return can be omitted.
}

3) STATEMENT
	Form 1: compound statement
	{
		<STATEMENT>
		<STATEMENT>
		<...>
	}
	Form 2: Expression on its own.
		<EXPRESSION>
	Form 3: Label
		label <LABELNAME>:
	Form 4: Goto
		goto <LABEL>;
		You may only GOTO a label in the current stack frame.
		if you wish to have an if statement which gotos, you must *not* begin a stack frame.
	Form 5: Gotoif
		<expression> gotoif <LABEL>;
		The same restriction applies as above.
		
	
4) EXPRESSION
	Postfix notation is used, like a calculator.
	Forth-like load/store semantics are used.
	to call a function with two short arguments a and b...
	@a @b myfunction;

	Operators' types are compiletime-determined.

	Note the placement of @ and !. @ and ! must prepend a variable name.
	
5) EMBEDDED DATA

You may specify a raw data file to embed into your binary
DATA(filename)[memory location where it should be written]

You may also specify character literals like this:
[memorylocation]!Hello world! this is a character literal.\n\\< this is a backslash on a new line.

The compiler warns you if the data or character string occupies more than a single region of memory.

7) TYPE CONVERSION
	All byte types are promoted to shorts automatically.
	You can split a pointer into two shorts with split
	you can join two shorts into a pointer with join(type)
8) CONSTANT DECLARATION
	you can declare a constant like this
	const type = value
	it effectively equates to a macro, and it must be an integer literal.
9) HOW EXPRESSIONS ARE PARSED

	short Operators are brain-dead simple.
	operator+ for instance:
	alpop;blpop;add;alpush;
	
	@<variablename> loads and !<variablename> stores for byte and short types are handled like this
	~~LOADS~~
	.a global short:
	sc %<upper byte>%; llb %<lower short>%; farillda; alpush;
	.a global byte:
	sc %<upper byte>%; llb %<lower short>%; farilda; alpush;
	.a stack short:
	astp; llb %stack placement depth%; sub; illdaa; alpush;
	.a stack byte:
	astp; llb %stack placement depth%; sub; ca; ilda; alpush;
	~
	.a global pointer:
	sc %<upper byte>%; llb %<lower short>%; farilda; alpush;
	llb %<lower short+1>%; farillda; alpush;
	.a stack pointer:
	astp; llb %stack placement depth%; sub; ca; ilda; alpush;
	astp; llb %stack placement depth+1%; sub; illdaa; alpush;
	~~STORES~~
	.a global short:
	sc %<upper byte>%; llb %<lower short>%; alpop; faristla;
	.a global byte:
	sc %<upper byte>%; llb %<lower short>%; alpop; farista;
	.a stack short:
	astp; llb %stack placement depth%; sub; ca; alpop; istla;
	.a stack byte:
	astp; llb %stack placement depth%; sub; ca; alpop; ista;
	~
	.a global pointer:
	sc %<upper byte>%; llb %<lower short>%; alpop; farista;
	llb %<lower short+1>%; alpop; faristla;
	.a stack pointer:
	astp; llb %stack placement depth%; sub; ca; alpop; ista;
	astp; llb %stack placement depth+1%; sub; ca; alpop; istla;
#########################
	The more complex @_ which takes <short index> <pointer> and !_ which takes <value> <short index> <pointer>
	are handled like this:
#########################
	~~LOADS~~
	.Pointer to global bytes:
	alpop;	bpop;cb;blpop; add; farildb; blpush;

	.Pointer to global shorts:
	blpop;	apop;ca;alpop;
	blpush; 
		lb2; mul; 
	blpop;
	add; farilldb; blpush;

	.Pointer to global pointers:
	blpop;	apop;ca;alpop;
		blpush; 
			lb3; mul; 
		blpop;
	add; farildb; bpush;
	lb1; add; farilldb; blpush;
	
	~~STORES~~
	.Pointer to global bytes:
	alpop;	bpop;cb; blpop; add; 			blpop;faristb;
	.Pointer to global shorts:
	blpop;	apop;ca; alpop;	
		blpush;
			lb2; mul;
		blpop;
	add; blpop; faristlb;
	.Pointer to global pointers:
	blpop;	apop;ca; alpop;	
		blpush;
			lb3; mul;
		blpop;
	add; 
#we have now set up the pointer to where we want to write our stuff set up in c,a
#
	lb1;add;blpop;faristlb;
	lb1;sub;bpop;faristb;
*/


typedef struct{
	unsigned int pointer_level; /*0 = not a pointer*/
	unsigned int value; 
	/*
	All variables not on the stack are "far-indexed"
	this includes goto labels.
	if this is a constant, this holds its value.
	if this is a global variable, this holes its address.
	if this is a stack variable, this holds its placement
	in the stack frame.
	*/
	char is_stack; /*does this variable exist on the stack?*/
	char is_constant; /*is this a compiletime constant? if 1, ignore is_stack.*/
	char basic_type; /*0 for byte, 1 for short.*/
}typeid;

#define TOKEN_NAME_MAX 63

typedef struct {
	char is_operator; /*Is this an operator to execute on things on the stack, or is this a
					value/variable?*/
	typeid type;
	char is_array; /*Invalid for an expression, but valid for a global variable declaration.*/
	char is_label; 
	char name[TOKEN_NAME_MAX + 1];
} token;



typedef struct {
	token* tokens; unsigned int ntokens;
	char can_return; /*can this expression return?*/
}expression;

typedef enum {
	NORMAL_STATEMENT=0,
	CONTROL_FLOW_STATEMENT=1, /*Check for the presence of "goto" or "gotoif" inside non-compound statement.*/
	LABEL_STATEMENT=2, /*label <name>:*/
	COMPOUND_STATEMENT=3
} statement_type;



typedef struct stackframe{
	token* variables; 
	unsigned int nvariables;
	char can_return; 
} stackframe;

typedef struct statement{
	statement_type type; /*0 = default. 1 = compound. 2 = goto. 3 = gotoif.*/
	char label_name[TOKEN_NAME_MAX+1]; /*labels may have names that are, at most, 32 characters long.*/
	struct statement* child_statements; /*used in compound statement.*/
	expression expr; /*This *only* goes unused in the compound statement.*/
	stackframe frame;
}statement;

typedef struct{
	typeid return_type;
	typeid arguments[10];
	char narguments;
	statement s;
	char* function_body_text;
	char name[TOKEN_NAME_MAX+1];
	unsigned int function_body_start;
	unsigned int function_body_end;
} functiondecl;

typedef struct{
	char* data;
	unsigned int datasize;
} dataincl;

#define NUM_RESERVED_KEYWORDS 44
const char* reserved_keywords[NUM_RESERVED_KEYWORDS] = {
	"gotoif","goto",
	"{","}",
	"short","byte",
	"!_","@_",
	"!", "@",
	"void",
	"split","join",
	";",",",
	/*Math.*/
	"*","/",
	"+","-",
	"%","&",
	"|","^",
	"~","<",
	">","==",
		"!=",
	"*.1.15","*.2.14",
	"*.3.13","*.4.12",
	"*.5.11","*.6.10",
	"*.7.9","*.8.8",
	"*.9.7","*.10.6",
	"*.11.5","*.12.4",
	"*.13.3","*.14.2",
	"*.15.1","*.16",
};

#define MAX_GLOBAL_VARS 5000
#define MAX_FUNCTIONS 5000
#define MAX_SEGS 100
token global_vars[MAX_GLOBAL_VARS];
functiondecl functions[MAX_FUNCTIONS];
dataincl datasegs[MAX_SEGS]; /*included data segments.*/
unsigned int nglobals = 0;
unsigned int nfunctions = 0;
unsigned int is_debugging = 1;

int check_is_reserved(char* name){
	unsigned i;
	for(i = 0; i < NUM_RESERVED_KEYWORDS; i++)
		if(streq(name, reserved_keywords[i]))
			return 1;
	return 0;
}

int check_is_function(char* name){
	unsigned i;
	for(i = 0; i < nfunctions; i++)
		if(streq(name, functions[i].name))
			return 1;
	return 0;
}

int check_is_global(char* name){
	unsigned i;
	for(i = 0; i < nglobals; i++)
		if(streq(name, global_vars[i].name))
			return 1;
	return 0;
}

int main(int argc, char** argv){
	unsigned long filelen; unsigned int i; FILE* filein,* fileout; char* filetext;
	for(i = 1; i<(unsigned)argc; i++){
		if(streq("-i", argv[i-1])){
			if(!filein)
				filein = fopen(argv[i], "r");
			else
			{
				puts("<FIFTH ERROR> Too many input files");
				return 1;
			}
		}
		if(streq("-o", argv[i-1])){
			if(!fileout)
				fileout = fopen(argv[i], "wb");
			else
			{
				puts("<FIFTH ERROR> Too many output files");
				return 1;
			}
		}
	}
	if(!filein){
		puts("<FIFTH ERROR> No input file");
		return 1;
	}
	if(!fileout){
		puts("<FIFTH ERROR> No output file");
		return 1;
	}
	filetext = read_file_into_alloced_buffer(filein, &filelen);
	if(filelen == 1){
		puts("<FIFTH ERROR> empty input. Cannot compile.");
		return 1;
	}filelen--;
	/*
		The source code has been received.
		Pass 1: Retrieve the list of all functions.
	*/
	for(i = 0; i < filelen;){
		unsigned int namestart, nameend;
		functiondecl current = {0};
		/*
			a function declaration looks like this
			<short|byte><some number of stars><space> <name><space> <body>
		*/
		if(strprefix("#", filetext+i)){/*Skip over line comments.*/
			while(i<filelen && !strprefix("\n", filetext+i))i++;
			if(i >= filelen) continue; /*End of file reached.*/
		}
		if(strprefix("/#", filetext+i)){/*Skip over multi-line comments.*/
			while(i<filelen && !strprefix("#/", filetext+i))i++;
			if(i >= filelen) continue; /*End of file reached.*/
		}
		if(strprefix("{" /*}*/, filetext+i)){ /*Skip over scopes.*/
			/*{*/while(i<filelen && !strprefix("}", filetext+i))i++;
			if(i >= filelen){
				printf("\n<FIFTH ERROR> You have declared a scope which does not end.\n");
				return 1;
			}
		}
		if(i < filelen && !strprefix("short",filetext+i) && !strprefix("byte", filetext+i)){i++; continue;}
		/*We have now reached something which either starts with short or byte.*/
		if(strprefix("short",filetext+i)) {current.return_type.basic_type = 1;i+=5;}
		else if(strprefix("byte",filetext+i)) {current.return_type.basic_type = 0;i+=4;}
		else {
			printf("\n<FIFTH INTERNAL ERROR> Bad function recognition <1>");
			return 1;
		}
		/*Count stars*/
		current.return_type.pointer_level = 0;
		while(strprefix("*",filetext+i)) current.return_type.pointer_level++;
		if(i>=filelen){		
			printf("\n<FIFTH ERROR> You began declaring something at the end of the file...\n");
			return 1;
		}
		/*Check if this is a global variable declaration.*/
		if(filetext[i] == '[')continue;
		/*Skip whitespace*/
		while(strprefix(" ",filetext+i) || strprefix("\n",filetext+i) || strprefix("\t",filetext+i) ) i++;
		/*The next part will be its name! but first, make sure that this thing doesn't end here.*/
		if(i>=filelen){
			printf("\n<FIFTH ERROR> You began declaring a function at the end of the file...\n");
			return 1;
		}

		/*Ensure the first character is valid.*/
		if(!isalpha(filetext[i])){
			printf("\n<FIFTH ERROR> You began declaring a function with a non-alpha character. Here:\n%s\n", filetext + i);
			return 1;
		}
		namestart = i;
		while(i < filelen && isalnum(filetext[i]))i++;
		nameend = i;
		if(nameend == namestart) {
			printf("\n<FIFTH ERROR> Function declaration of zero length at:\n%s\n", filetext + i);
			return 1;
		}
		if(nameend -namestart > 63) {
			printf("\n<FIFTH ERROR> Function name too long! at:\n%s\n", filetext + i);
			return 1;
		}
		if(i>=filelen){
			printf("\n<FIFTH ERROR> You began declaring a function at the end of the file...\n");
			return 1;
		}
		if(
			filetext[i] != '(' /*)*/
		){
			printf("\n<FIFTH ERROR> Function declaration without immediately following open parentheses. Here:\n%s\n", filetext+i);
			return 1;
		} /*This is most likely a variable declaration.*/
		memcpy(current.name, filetext+i, nameend-namestart);
		/*This is a name so we have to check and make sure the user didn't try to use a pre-existing global symbol.*/
		if(check_is_global(current.name) ||
			check_is_function(current.name) ||
			check_is_reserved(current.name)
		){
			printf("\n<FIFTH ERROR> Function declaration has name which is used. Here:\n%s\n", filetext+i);
			return 1;
		}
	}
	/*
		TODO:
		1) Parse global variables
		2) Parse Function Bodies into variable declarations, labels, and statements.
		3) Parse statements into expressions.
		4) 
	*/
	return 0;
}
