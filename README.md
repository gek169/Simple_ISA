# Simple, Embeddable, Elegant 16 bit instruction set simulator

C program to simulate the SISA-16 virtual portable computer architecture.

The included assembly programs and assembler are designed for isa.c.

(the cpp file in this repository is a proof-of-concept unmaintained alpha version of this project.)

Here is how you use the emulator on a unix system:
```bash
#compile the emulator
cc isa.c -o isa
#compile the assembler
cc assembler.c -o asm
#assemble your program
./asm -i program.asm -o program.bin
#run your program
./isa program.bin
```
On a windows machine, the compiled C programs would be EXEs rather than lacking an extension.

DuckDuckGo is your friend if you need to learn how to compile C programs on your target platform.


The emulator and assembler are confirmed to compile *and run correctly* on linux with GCC, clang, and tinyc.

The emulator has the best performance compiled with clang at -Ofast.

The emulator itself has been confirmed to *compile* using compiler explorer on...

* Intel Cilk Compiler

* GCC and Clang on virtually every platform.

* MSVC

* a myriad of unusual and strange compilers on dozens of architectures.

Both the emulator and the assembler are c89 compliant.

I plan on making the assembler self-hosting at some point.

Terminology:
```
Address: Location where data is stored in main memory

Register: a very fast form of memory which is not part of main memory

Page: 256 bytes starting at an address whose low byte is zero.

Region: 64 kilobytes starting at an address whose 2 low bytes is zero.

Zero 'Home' Region: the topmost region, where the stack pointer is stuck and where normal non-far loads and stores happen.

Bus or Device: 
the implementation of gch() and pch() from d.h which is used for I/O and accessible with getchar/putchar
in the assembly language.
```
Supported instructions:
```
halt- end execution (1 byte) (0)
lda- load byte into register a,zero extend (3 bytes)(1)
la - set register a to value (2 bytes) (2)
ldb - load byte into register b, zero extend (3 bytes)(3)
lb - set register b to value (2 bytes) (4)
sc- load 2 constant bytes into c (3 bytes) (5)
sta- store a to location (3 bytes) (6)
stb- store b to location (3 bytes) (7)
add- a = a + b (1 byte) (8)
sub- a = a - b (1 byte) (9)
mul- a = a * b (1 byte) (A)
div- a = a / b (1 byte) (B)
mod- a = a % b (1 byte) (C)
cmp- if(a<b) a = 0; else if(a>b)a=2; else a=1; (1 byte) (D)
jmpifeq- set program counter to c if a == 1 (1 byte) (E)
jmpifneq- set program counter to c if a is not 1 (1  byte) (F)
getchar- read short from device. (Usually standard out) (1 byte) (10)
putchar- write register A as short to device (Usually standard out) (1 byte)(11)
and- a = a & b (1 byte)(12)
or- a = a | b (1 byte)(13)
xor- a = a ^ b (1 byte)(14)
lsh- a <<= b&7 (1 byte)(15)
rsh- a >>= b&7 (1 byte)(16)
ilda- a = read(c), indirectly access memory through register c and load it into a (1 byte)(17)
ildb- b = read(c), indirectly access memory through register c and load it into b (1 byte)(18)
cab- c = a<<8 + b, make c the combination of a and b. (1 byte)(19)
ab- a = b (1 byte)(1A)
ba- b = a (1 byte)(1B)
alc- a = low half of c (1 byte)(1C)
ahc- a = the high half of c (1 byte)(1D)
nop- (1byte) (1E)
cba c = b<<8 + a, make c the other combination of a and b. (1 byte) (1F)
~~16 bit ops
lla, Large Load 2 constant bytes into A (3 bytes)(20)
illda, a = [c], indirectly load 2 bytes into A through C (1 byte)(21)
llb, Large Load 2 constant bytes into B (3 bytes)(22)
illdb b = [c], indirectly load 2 bytes int B through C (1 byte)(23)
illdaa, a = [a] indirectly load 2 bytes into A through A (1 byte)(24)
illdbb, b = [b] indirectly load 2 bytes into B through B (1 byte)(25)
illdab, a = [b] indirectly load 2 bytes into A through B (1 byte)(26)
illdba, b = [a] indirectly load 2 bytes into B through A (1 byte)(27)
~~Moving to and from register c as shorts.
ca c=a (1 byte)(28)
cb c=b (1 byte)(29)
ac a=c (1 byte)(2A)
bc b=c (1 byte)(2B)

ista, [c] = a, (as byte) store a at the location pointed to by C (1 byte)(2C)
istb, [c] = b, (as byte) store b at the location pointed to by C (1 byte)(2D)
~~More 16 bit ops
istla, [c] = a, (as short) indirectly store large a at the location pointed to by C(1 byte)(2E)
istlb, [c] = b, (as short) indirectly store large b at the location pointed to by C(1 byte)(2F)

jmp, unconditionally jump to the location pointed to by c. (1 byte) (30)
stla, store large A at constant address (3 bytes) (31)
stlb, store large B at constant address (3 bytes) (32)
stc, store C at constant address (3 bytes) (33)
/*52 dec so far.*/
push, stp+=val, add to the stack pointer by a number of bytes (3 bytes) (34)
pop, stp-=val, subtract from the stack pointer by a number of bytes (3 bytes) (35)
pusha, stp+=a, add a to the stack pointer (1 byte) (36)
popa, stp-=a, sub a from the stack pointer (1 byte) (37)
astp, a = stp (1 byte) (38)
bstp, b = stp (1 byte) (39)

/*58 decimal so far.*/
compl, a = ~a (1 byte) (3A)
/*59 dec*/

cpc, c = the program counter. (1 byte) (3B)
call: (1 bytes)(3C)
write the program counter to the stack pointer. Push the stack pointer by 2. Jump to c.
ret: (1 byte)(3D)
subtract 2 from the stack pointer. load the program counter from the stack pointer.
farillda: load short using far memory indexing at [(u8)c<<16 + (u16)b] (1 byte) (3E)
faristla: store short into far memory indexing at [(u8)c<<16 + (u16)b] (1 byte) (3F)
farilldb: load short using far memory indexing at [(u8)c<<16 + (u16)a] (1 byte) (40)
faristlb: store short into far memory indexing at [(u8)c<<16 + (u16)a] (1 byte) (41)
NOTE: Page size is 256 bytes.
farpagel: copy 256 bytes from any page indexed by c to home region page indexed by a (1 byte) (42)
farpagest: copy 256 bytes to any page indexed by c from home region page indexed by a (1 byte) (43)

/*68 dec so far*/

//Far call procedure:

lfarpc: pp = a, move the program counter offset to a different 64k region of memory. (1 byte) (44)
farcall: (1 byte) (45)
write the program counter to the stack pointer. 
Push the stack pointer by 2.
Write the program counter offset to the stack pointer. Set the program counter offset to a. Jump to c.
farret: (1 byte) (46)
Subtract 1 from the stack pointer, assign the program counter offset from the stack pointer.
subtract 2 from the stack pointer. load the program counter from the stack pointer. (jump)
farilda: load byte using far memory indexing at [(u8)c<<16 + (u16)b] (1 byte) (47)
farista: store byte into far memory indexing at [(u8)c<<16 + (u16)b] (1 byte) (48)
farildb: load byte using far memory indexing at [(u8)c<<16 + (u16)a] (1 byte) (49)
faristb: store byte into far memory indexing at [(u8)c<<16 + (u16)a] (1 byte) (4A)
/*75 dec so far.*/

FIXED POINT OPERATIONS: 16 multiply instructions. add and subtract carry over.
ranging from 4B to 5A. All 1 byte.
imul15.1: fixed-point multiply of a and b, 15.1
imul14.2: fixed-point multiply of a and b
imul13.3: fixed-point multiply of a and b
imul12.4: fixed-point multiply of a and b
imul11.5: fixed-point multiply of a and b
imul10.6: fixed-point multiply of a and b
imul9.7: fixed-point multiply of a and b
imul8.8: fixed-point multiply of a and b
imul7.9: fixed-point multiply of a and b
imul6.10: fixed-point multiply of a and b
imul5.11: fixed-point multiply of a and b
imul4.12: fixed-point multiply of a and b
imul3.13: fixed-point multiply of a and b
imul2.14: fixed-point multiply of a and b
imul1.15: fixed-point multiply of a and b
imul.16: fixed-point multiply of a and b
alpush: push short register a onto the stack. (1 byte) (5B)
blpush: push short register b onto the stack. (1 byte) (5C)
cpush: push short register c onto the stack. (1 byte) (5D)
apush: push byte register a onto the stack. (1 byte) (5E)
bpush: push byte register b onto the stack. (1 byte) (5F)

alpop: pop short register a from the stack. (1 byte) (60)
blpop: pop short register b from the stack. (1 byte) (61)
cpop: pop short register c from the stack. (1 byte) (62)
apop: pop byte register a from the stack. (1 byte) (63)
bpop: pop byte register b from the stack. (1 byte) (64)

The rest: halt duplicates, free for expansion (1 byte)
```
There are plenty of free instruction spots for you to play around with in your experimentation.

The primary usecase for this is probably embedding a portable bytecode instruction set into a game,
or for educational purposes.

The emulator will print out its memory layout at the end of execution if you pass an additional argument
to it on the commandline (other than just the program name)
```bash
./isa program.bin literallyanything
```

## Device Interface

You can make the emulator work with any device you want by implementing your own abstraction layer.
Implement these functions in a d.h file:

void di() is the device initializer.
void dcl() is the devoice closing function.
unsigned short gch() is the "get" function which puts something into register a.
void pch(unsigned short) is the "put" function which gets something from register a.

The emulator requires a filename as the first parameter when running.

A very basic assembler is included with the ability to define sections, 
include arbitrary data,
and write your own multi-statement macros.
It also does a limited amount of error checking.

## What is this ISA called?

SISA-16, Simple-ISA-16.

## Specs?

The ISA is capable of indexing 64k of memory "normally" but by using "far" indexing, it can
access 16 megabytes in total.

You can use the included assembler to define ROMs which are up to the full 16 megabytes in size.

## Far memory system.

Note that the program counter is 16 bit (not 32) and so the program counter cannot normally access
more than the first 64 kilobytes.

To get around this, I have implemented an "offset" system.

THere is a special register called the "program counter offset" which is set implicitly in farcall and farret,
or set explicitly with lfarpc.

It is recommended you put your procedure definitions on page boundaries so that they can be easily copied and called.
You might want to define a standard for how the number of pages a procedure occupies is specified-
for instance, if a procedure occupies three pages,
you might want to store a single byte before the beginning of the procedure specifying its length as three pages.

If you use the farcall/farret system, be warned: you cannot use normal loads and stores to access memory
outside the first 64k. This means for instance that if you write self-modifying code, you will not write
to the actual location of instructions being executed.

This can be useful- you can consider the first 64k "stack only" or "scratchpad memory"

jumps and calls that happen after a farcall or lfarpc will always jump within the
current 64k that the program counter is inside of.

the first 64k is typically the fastest to access on the emulator since it is closest to the registers.

## Programming conventions established

Stack:

The stack pointer (which always resides in the first 64k) points to a *free* location on the stack.
The stack pointer *increments* rather than decrements to increase the stack size.

The stack can be, as most, 64 kilobytes.

Procedure calling:

Procedures freely clobber registers and do not necessarily leave a return value in the accumulator,
it would be much more dynamic to put them on the stack.

## SISA-16 Assembler

the SISA-16 assembler is very basic but very powerful. you can define variables, include arbitrary data in multiple ways,
and of course, invoke every single one of SISA-16's many instructions.

The assembler is a two-pass compiler. The first pass behaves like the second pass but doesn't actually write
to the output file. On the second pass, the assembler writes to the output file.

This is done so that macros for goto labels can be generated on the first pass and used on the second-
if you have something like this:
```
sc %myLabel%;jmp;
#later...

VAR#myLabel#$
```
then on the first pass, the split `%myLabel%` will be evaluated as zero on the first pass, but on the second pass,
the value will be used.

The assembler allows you to define macros like this
```
VAR#myVariableName# my variable definition
```
If the variable definition contains a builtin macro or assembly directive such as `$`, `@`, or `%%`, then 
they will be expanded before macro definition. This allows you to define goto labels, for instance.

Anywhere where "myVariableName" is encountered from then-on in the source code (including in the second pass) will be
expanded.

Note that you can create recursive or infinitely recursive macros, and the latter will undoubtedly segfault the program.
Is that a bug, or a feature?
my answer is YES to both, and i'm sure clever programmers will come up with ways to abuse macro recursion

I'm also extremely confident that you could somehow cause the second pass to be out-of-sync with the first pass
by abusing macro recursion. If you find such a bug, file an issue, I'll try to fix it.

The assembler will generate warnings for most situations that would warrant it:
* a split directive evaluates to zero (You could have just written "0,0" , it is most likely a misspelled variable)
* an inline comment invalidates the rest of a line (Just a warning)
* fill tag is set to zero when not explicitly declared as zero (the statement will do nothing)
* section tag is set to zero when not explicitly declared as zero (the statement moves the outputcounter to the beginning of the output file)

The assembler will generate unrecoverable errors for most situations that would warrant it:
* An unknown name is parsed as a command (Usually meaning you misspelled a macro name...)
* An invalid number of arguments is provided for an instruction or directive
* The bounds check for a region restriction or page/block restriction fails. This is useful when you want to
make sure that a subroutine fits inside of a memory region or page (64k or 256 bytes, aligned, respectively) or that
a piece of data which will be accessed as an array can be indexed "normally" using 16 bit math.
* a macro definition uses a reserved name, like an instruction name or anything beginning with ASM_ or asm_
* a macro definition would trample a built-in macro or reserved name. You cannot define "ll" for instance,
	because it would make "llb" and "lla" and "illdaa" unusable.
* a file is included that is larger than the entire SISA-16 address space.
* a file is included that is empty
* you attempt to define a string literal anywhere other than the start of a line.
* asm_halt was invoked on the second pass.
* an unreachable or unopenable file is included with ASM_data_include

### list of SISA-16 assembler reserved words
```
	<all the instruction names are reserved. See above.>
	asm_print- if on the second pass, print the output counter, the line, and the line post-processing.
	asm_halt- if on the second pass, halt assembly.
	ASM_*- reserved namespace
	asm_*- reserved namespace
	VAR#- define a macro with syntax VAR#name#definition. The macro must be the only thing on the line.
	!- string literal line. Must start at the beginning of a line with no preceding whitespace. 
		Macros are not expanded.
	$- expands to the current position of the line counter as an unsigned short, but split into two bytes.
		if the line counter is at 0xe9f2 then $ will evaluate to '233,242'
	ASM_data_include- include a file as raw bytes in the output. Macros are not expanded.
	bytes- include arbitrary bytes in the output file, 8 bit unsigned integers.
	shorts- include arbitrary pairs of bytes in the output file, 16 bit unsigned integers.
	" "- space macro, used to remove whitespace.
	"\t"- tab macro, used to remove tabs.
	asm_begin_region_restriction- record the current region of the output counter and emit an assemblytime error
		if an attempt to write bytes outside that region is made before an end tag.
	asm_begin_block_restriction- same as asm_begin_page_restriction
	asm_begin_page_restriction- record the current page of the output counter and emit an assemblytime error
		if an attempt to write bytes outside that page is made before an end tag.
		Note that setting a page restriction causes the current region restriction to be forgotten.
	asm_end_region_restriction- same as asm_end_page_restriction.
	asm_end_block_restriction- same as asm_end_page_restriction.
	asm_end_page_restriction- End the current restriction, do not emit errors.
```
### How to use the SISA-16 assembler

The assembler is compiled as `asm` by default on a *nix machine.
you can invoke the assembler on a source file `source.asm` and create `source.bin` like this:

`./asm -i source.asm -o source.bin`

if you are editting or debugging the assembler itself, you may find it useful to
view extended debug output, or use stdin as the input file. you can use -DBG as an argument to the assembler for this.

Note that the two passes *won't work* in stdin mode.

if you have bash on your system, or another compatible shell with typical core utils, then note that
any and all `.asm` files will be compiled and output to `.bin` files of the same name if they are placed
in the top level directory of this project when you invoke `make`, thanks to `asm_compile.sh`

### Static Linking
Through intelligent use of the assembler to compile libraries before programs that use them, one can achieve
static linking by using `ASM_data_include` and then creating macros to call functions defined in the file included,
or macros to refer to variables which are instantiated and used inside the library.

```
#you have some library written as myLibrary.asm
./asm -i myLibrary.asm -o myLibrary.bin
#in your program's asm file...
section 0xee0000
asm_begin_region_restriction;
ASM_data_include myLibrary.bin
asm_end_region_restriction;

#Note that ASM_data_include does not change where the code you are including
#decides to put its variables or subroutines- the assembler just sees bytes- you must be careful about that.

#You have a subroutine called "print" defined in your library which is at 0xff90 in the library.
VAR#callPrint#la0xee;sc%0xff90%;farcall;
#Your library defines a short variable "LibVar" at 0xb102a4 which it uses for something
VAR#getshrtLibVar#sc %0xb1%;llb 0x02a4;farillda;alpush;
VAR#putshrtLibVar#sc %0xb1%;llb 0x02a4;alpop;faristla;
```

### Integer literals
Integer literals are default evaluated as decimal, like any other programming language,
but by prefixing them with `0` (zero) you can make them be interpreted as octal, or `0x` (zero, lowercase x) to be interpreted as hexadecimal.


```
Written by
~~~DMHSW~~~
for the public domain
```
