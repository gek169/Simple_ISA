# Simple, Embeddable, Elegant 16 bit instruction set simulator

The C source is obfuscated but the C++ is not and runs at compiletime

The included assembly programs and assembler are designed for the C code.

The included makefiles and shell script show how to use the emulator and the assembler

The emulator and assembler are confirmed to compile *and run correctly* on linux with GCC, clang, and tinyc.

The emulator itself has been confirmed to *compile* using compiler explorer on...

* Intel Cilk Compiler

* GCC arm and Clang on virtually every platform.

* MSVC

* a myriad of unusual and strange compilers

The emulator itself is C89 compliant, but the assembler uses C99 due to it using "unsigned long long".

It could be converted to C89 as well but it would require re-writing my string library. I decided not to bother.

These are the supported instructions in the C version, the C++ has a tiny fraction and is
mostly a proof-of-concept that you can have a compiletime machine emulator.
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
farillda: load short using far memory indexing at [(u32)c<<8 + (u32)b] (1 byte) (3E)
faristla: store short into far memory indexing at [(u32)c<<8 + (u32)b] (1 byte) (3F)
farilldb: load short using far memory indexing at [(u32)c<<8 + (u32)a] (1 byte) (40)
faristlb: store short into far memory indexing at [(u32)c<<8 + (u32)a] (1 byte) (41)
NOTE: Page size is 256 bytes.
farpagel: copy 256 bytes from page indexed by c to local page indexed by a (1 byte) (42)
farpagest: copy 256 bytes to page indexed by c from local page indexed by a (1 byte) (43)

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
farilda: load byte using far memory indexing at [(u32)c<<8 + (u32)b] (1 byte) (47)
farista: store byte into far memory indexing at [(u32)c<<8 + (u32)b] (1 byte) (48)
farildb: load byte using far memory indexing at [(u32)c<<8 + (u32)a] (1 byte) (49)
faristb: store byte into far memory indexing at [(u32)c<<8 + (u32)a] (1 byte) (4A)
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
blpop: pop short register b from the stack. (1 byte) (60)
cpop: pop short register c from the stack. (1 byte) (60)
apop: pop byte register a from the stack. (1 byte) (60)
bpop: pop byte register b from the stack. (1 byte) (60)

The rest: halt duplicates, free for expansion (1 byte)
```
There are plenty of free instruction spots for you to play around with in your experimentation.

You may want to add more instructions than there are currently free slots,
just change the "&127" to "&255" and add 128 more entries into the switch case.

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

## Programming conventions

Stack:
The stack pointer (which always resides in the first 64k) points to a *free* location on the stack.
The stack pointer *increments* rather than decrements to increase the stack size.

```
Written by
~~~DMHSW~~~
for the public domain
```
