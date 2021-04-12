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
	a@ b@ + c@ + return;
}
#Offset store example.
#Notice how a is "at"-ed. Since a is a pointer, a's name evaluates to a pointer to a pointer.
#it must be dereferenced to get the actual pointer.
short writeshort(short* a){
	a@ 3 5 !_ 0 return;
}
short* offsetwriteshort(short* a, short offset){
#!_ requires pointer, short(index), value.
#the typeid of value must be a.typeid.pointer_level-1
#(the first argument is first tested to see if its pointer_level is zero)
	a@ offset@ 3 !_ return;
}
short not(short a){
	a@ ~ return;
}
short* to_pointer(byte high, short low){
#demonstrating the to-pointer syntax.
#a pointer is just two shorts with the uppermost byte
	high@ low@ join(short*) return;
}
short pointer_to_short(short* a){
	a@ split return;
}
byte pointer_high_byte_retrieval(short* a){
	a@ split void return;
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
	a b myfunction;

	Operators' types are compiletime-determined.

	A variables name is considered to be a pointer to it in expressions, (for arrays, a pointer to the first element.)
	
5) EMBEDDED DATA

You may specify a raw data file to embed into your binary
DATA(filename)[memory location where it should be written]

You may also specify character literals like this:
[memorylocation]"Hello world!"

7) TYPE CONVERSION
	All byte types are promoted to shorts automatically.
	You can split a pointer into two shorts with split
	you can join two shorts into a pointer with join(type)
8) CONSTANT DECLARATION
	you can declare a constant like this
	const type = value
	it effectively equates to a macro.

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
	unsigned char name[TOKEN_NAME_MAX + 1];
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

#define NUM_RESERVED_KEYWORDS 31
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
	"!","||",
	"&&","!=",
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



int main(int argc, char** argv){
	unsigned long filelen; unsigned int i;
	char* filetext = read_file_into_alloced_buffer(stdin, &filelen);
	if(filelen == 1){
		puts("<FIFTH ERROR> empty stdin input. Cannot compile.");
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
		else {i++; continue;}
		/*Count stars*/
		current.return_type.pointer_level = 0;
		while(strprefix("*",filetext+i)) current.return_type.pointer_level++;
		/*Skip whitespace*/
		while(strprefix(" ",filetext+i) || strprefix("\n",filetext+i) || strprefix("\t",filetext+i) ) i++;
		/*The next part will be its name! but first, make sure that this thing doesn't end here.*/
		if(i>=filelen){
			printf("\n<FIFTH ERROR> You began declaring something at the end of the file...\n");
			return 1;
		}
		/*Check if this is a global variable declaration.*/
		if(filetext[i] == '[')continue;
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
		if(filetext[i] != ' ' && 
			filetext[i] != '\t' &&
			filetext[i] != '\n' &&
			filetext[i] != '{' /*}*/
		){continue;} /*This is most likely a variable declaration.*/
		
		memcpy(current.name, filetext+i, nameend-namestart);
		/*TODO: This is a name so we have to check and make sure the user didn't try to use a pre-existing global symbol.*/
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
