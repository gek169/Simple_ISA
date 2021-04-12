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
		short myvariable[arraysize]
		byte myvariable[arraysize]
		short* myvariable;
		byte* myvariable;
2) PROCEDURE DECLARATIONS
short|byte|short*|byte*|void proc myfunction(argument description)
	<STATEMENT>
3) ARGUMENT DESCRIPTION
By example:
short add3 (short a, byte b, short c){
	@a @b + @c + return;
}
short writeshort(short* a){
	3 @a + !* 0 return;
}
short* offsetshort(short* a, short offset){
#this also demonstrates pointer arithmetic- only supported for short* or byte* with a short or byte.
	@offset @a + return;
}
short not(short a){
	@a ~ return;
}
4) STATEMENT
	Form 1: compound statement
	{
		<STATEMENT>
		<STATEMENT>
		<...>
	}
	Form 2: "if"
	<expression> if
		<STATEMENT>
	1 if{
		3 4 myfunc;
	}
	Form 3: Expression on its own.
		<EXPRESSION>
	Form 4: Label
		<LABEL>:
	Form 5: Goto
		goto <LABEL>;
		You may only GOTO a label in the current stack frame.
		if you wish to have an if statement which gotos, you must *not* begin a stack frame.
	@myvar if
		goto mylabel;
	
5) EXPRESSION
	Postfix notation is used, like a calculator.
	Forth-like load/store semantics are used.
	to call a function with two short arguments a and b...
	a b myfunction;

	Operators' types are compiletime-determined.

	An array's name is considered to be a pointer to its first element- like C.
	if you try to load or store to an index in it, that index must be a compiletime constant.
6) EMBEDDED DATA

You may specify a raw data file to embed into your binary
DATA(filename)[memory location where it should be written]

You may also specify character literals like this:
[memorylocation]"Hello world!"

7) TYPE CONVERSION
	All byte types are promoted to shorts automatically- all math is done either on pointers
	or 
	additions and subtractions with a pointer can only be done 
*/


typedef struct {
	char is_operator; //Is this an operator to execute on things on the stack, or is this a
					//value/variable
	char is_stack; //does this variable exist on the stack?
	char is_constant; //is this variable a constant?
	char type; //0 for byte, 1 for short, 2 for short pointer, 3 for byte pointer.
	unsigned int value; //All variables not on the stack are "far-indexed"
						//this includes goto labels.
						//if this is a constant, this holds its value.
						//if this is a variable, this holes its address.
	unsigned char name[500]; //the name of this token.
} token;

typedef struct stackframe{
	unsigned int local_depth; /*Depth not including children.*/
	variable stack_variables[500]; //you may have at most 500 variables in the stack frame.
};

const char* reserved_keywords[10] = {
	"if",
	"goto",
	"{",
	"}",
}

variable global_vars[10000]; /**/
unsigned int nglobals = 0;

int main(int argc, char** argv){
	unsigned long linelen;
	char* filetext = read_file_into_alloced_buffer(stdin, &linelen);
	/*
		The source code has been received.
	*/
}
