# Simple, Embeddable, Performant Virtual Machine Language

SISA16 is an extremely lightweight virtual machine designed to replace languages like Lua for program extension.

it executes identically on every major platform and computer architecture. 

## Why is SISA16 special?

* Minimal dependence on the host operating system. A very minimal C standard library will build the assembler
	and emulator just fine.

* CPU architecture independence- With the exception of floating point arithmetic (And to a limited extent, signed integer division) all SISA16 operations are fully
	architecture independent. A softFP implementation is planned. 
	Fixed point arithmetic is provided as an alternative to floating point if needed, and
	signed integer division can be simulated in software if needed.

	The emulator and assembler are confirmed to work on many different architectures of varying endiannesses.

* Unique. SISA16 is no ordinary virtual machine language...

	* Self-sandboxing- run code while handling errors and handling side effects.

	* Tiny- Less than 100 kilobytes for a full emulator and assembler toolchain!

	* Hackable- More instructions can be added at a whim.

* Trivially embeddable. Implement five small and easy-to-understand functions in a single header file
	to add SISA16 scripting to any system.

* Ready for deployment. the assembler can build, execute, and disassemble binaries for debugging.

* Documented. Manpages and developer manual are provided in this repository along with examples.

* Public domain. 

	* (NOTE: The provided example textmode driver includes four lines under the Ncurses license. See d.h.)

Build Statuses:

gek169/Simple_ISA:

Ubuntu (x86_64):
![build personal](https://github.com/gek169/Simple_ISA/actions/workflows/c-cpp.yml/badge.svg)

C-Chads/Simple_ISA:

Ubuntu (x86_64):
![build cchads](https://github.com/C-Chads/Simple_ISA/actions/workflows/c-cpp.yml/badge.svg)

This repository contains the SISA16 standalone emulator source, 
the macro assembler/disassembler which doubles as an emulator,

# What does the Assembly language look like?

a simple function which prints a byte's value as two hex-digits:

![assembler screenshot](Assembler_Language_Screenshot.png)

Advanced example with macros, demonstrating compiletime macro calling with asm_call:

![assembler screenshot 2](Assembler_Language_Screenshot_2.png)

A portion of a disassembled binary:

![disassembly_out](Disassembler_Output_1.png)

Another binary disassembled:

![disassembly_out](Disassembler_Output_3.png)

'/dev/urandom' disassembly:

![disassembly_out](Disassembler_Output_2.png)

# Notable Features and Limitations

* 'emulate' instruction which allows for sandboxed execution.

* Infinite 'segment' which allows for access to more than 16 megs of memory.

* 'emulate_seg' functions the same but shares the infinite segment with the sandboxed process.

* the registers a, b, c, and the program counter are 16 bit. the program conter region is 8 bit.

* RX0-3 are 32 bit.

* No MOV instruction, SISA16 is a load/store RISC.

* 16 bit segmented memory model

* Single-threaded

* Roughly similar performance in the virtual machine with unoptimized or optimized builds of the VM.

* Uses computed goto on supported compilers, define USE_COMPUTED_GOTO

# How portable?

The emulator and assembler have been compiled and tested on the following architectures and platforms:

```c
	IA32 (using: Debian linux, Alpine linux, on: native, jslinux)
	i536 (using: Debian linux, Alpine linux, on: native, jslinux)
	x86_64 (using: Debian linux, Windows 10, on: native)
	riscv64 (Buildroot linux, jslinux)
	armhf 32 (using: Debian linux, raspberry pi)
```

The included assembly programs are written for the architecture and can be executed just like scripts!

(the cpp file in this repository is a proof-of-concept unmaintained alpha version of this project.)

Here is how you use the emulator on a unix system:
```bash
#compile the emulator
#most systems have UNSIGNED_INT as four byte unsigned types, that's what the define is for.
cc isa.c -o sisa16_emu -DUSE_UNSIGNED_INT
#compile the assembler
cc assembler.c -o sisa16_asm -DUSE_UNSIGNED_INT
#assemble your program
./sisa16_asm -i program.asm -o program.bin
#run your program
./sisa16_emu program.bin
#install sisa16
sudo cp sisa16_asm /usr/bin/
sudo cp sisa16_emu /usr/bin/

#run asm program starting with shebang as script
#the shebang looks like this for the default INSTALL_DIR:
#!/usr/bin/sisa16_asm -run
#you put that at the top of your file and chmod+x the .asm file, and it will run.
./program.asm
```
On a windows machine, the compiled C programs would be EXEs rather than lacking an extension, but your
SISA16 programs should be binary compatible.

DuckDuckGo is your friend if you need to learn how to compile C programs on your target platform.

# How to install SISA16 on your machine.

On most unix-based machines:
```sh
make
#or su - and login, no sudo...
sudo make install
#to uninstall
sudo make uninstall
```

you can run `sudo make -B install` and `sudo make -B uninstall` to install sisa16_emu and sisa16_asm (with those names) into INSTALL_DIR 

you can set the INSTALL_DIR by using `sudo make -B install INSTALL_DIR=/my/directory`

but make sure that while uninstalling you pass INSTALL_DIR again.

Alternatively, you can set INSTALL_DIR in the makefile yourself.

once you've installed the emulator and the assembler, you can quickly run assembly programs like this:

`sisa16_asm -run my_asm_file.asm`

if you want to use sisa16 assembler as a scripting language, you can add a hashbang to your files.

for the default INSTALL_DIR which is `/usr/bin`:

`#!/usr/bin/sisa16_asm -run`

The emulator and assembler are confirmed to compile *and run correctly* on linux with GCC, clang, and tinyc on
x86_64, x86, aarch64, riscv, and several other architectures.

Testing with the compcert compiler is planned.

The emulator has the best performance compiled with gcc at -Os.

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

Zero 'Home' or 'Stack' Region: the topmost region, where the stack pointer is stuck and where normal non-far loads and stores happen.

You can consider the home region to be "Scratchpad Memory"

Bus or Device: 
the implementation of gch() and pch() from d.h which is used for I/O and accessible with getchar/putchar
in the assembly language.

Segment: 
The additional region of memory outside the normal SISA16 address space, which is 
allocated at runtime, used for mass storage (the heap). Typically for loading files or extremely large structures,
but can also be used to hold code.
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
farpagel: copy 256 bytes from any page indexed by c to page indexed by a (1 byte) (42)
farpagest: copy 256 bytes to any page indexed by c from page indexed by a (1 byte) (43)

/*68 dec so far*/

//Far call procedure:

lfarpc: pp = a; pc=0; move the program counter offset to a different 64k region of memory. Set PC to 0. (1 byte) (44)
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
interrupt: pass the registers to the user defined "interrupt" function (1 byte) (65)
clock: set register a to the execution time in thousandths of a second. (1 byte) (66)
Register B will be the time in seconds. 
C will be the raw number of clock cycles.
arx0: load RX0 into A. (1 byte) (67)
brx0: load RX0 into B. (1 byte) (68)
crx0: load RX0 into C. (1 byte) (69)
rx0a: load A into RX0. (1 byte) (6A)
rx0b: load B into RX0. (1 byte) (6B)
rx0c: load C into RX0. (1 byte) (6C)

arx1: load RX1 into A. (1 byte) (6D)
brx1: load RX1 into B. (1 byte) (6E)
crx1: load RX1 into C. (1 byte) (6F)
rx1a: load A into RX1. (1 byte) (70)
rx1b: load B into RX1. (1 byte) (71)
rx1c: load C into RX1. (1 byte) (72)

arx2: load RX2 into A. (1 byte) (73)
brx2: load RX2 into B. (1 byte) (74)
crx2: load RX2 into C. (1 byte) (75)
rx2a: load A into RX2. (1 byte) (76)
rx2b: load B into RX2. (1 byte) (77)
rx2c: load C into RX2. (1 byte) (78)

arx3: load RX3 into A. (1 byte) (79)
brx3: load RX3 into B. (1 byte) (7A)
crx3: load RX3 into C. (1 byte) (7B)
rx3a: load A into RX3. (1 byte) (7C)
rx3b: load B into RX3. (1 byte) (7D)
rx3c: load C into RX3. (1 byte) (7E)

rx0_1: RX0 = RX1; (1 byte) (7F)
rx0_2: RX0 = RX2; (1 byte) (80)
rx0_3: RX0 = RX3; (1 byte) (81)

rx1_0: RX1 = RX0; (1 byte) (82)
rx1_2: RX1 = RX2; (1 byte) (83)
rx1_3: RX1 = RX3; (1 byte) (84)

rx2_0: RX2 = RX0; (1 byte) (85)
rx2_1: RX2 = RX1; (1 byte) (86)
rx2_3: RX2 = RX3; (1 byte) (87)

rx3_0: RX3 = RX0; (1 byte) (88)
rx3_1: RX3 = RX1; (1 byte) (89)
rx3_2: RX3 = RX2; (1 byte) (8A)

lrx0: load 4 constant bytes (highest, high, low, lowest) into RX0. (5 bytes) (8B)
lrx1: load 4 constant bytes (highest, high, low, lowest) into RX1. (5 bytes) (8C)
lrx2: load 4 constant bytes (highest, high, low, lowest) into RX2. (5 bytes) (8D)
lrx3: load 4 constant bytes (highest, high, low, lowest) into RX3. (5 bytes) (8E)

farildrx0: load RX0 from memory location M[(c&255)<<16 + a] (1 byte) (8F)
farildrx1: load RX1 from memory location M[(c&255)<<16 + a] (1 byte) (90)
farildrx2: load RX2 from memory location M[(c&255)<<16 + a] (1 byte) (91)
farildrx3: load RX3 from memory location M[(c&255)<<16 + a] (1 byte) (92)

faristrx0: store RX0 at memory location M[(c&255)<<16 + a] (1 byte) (93)
faristrx1: store RX1 at memory location M[(c&255)<<16 + a] (1 byte) (94)
faristrx2: store RX2 at memory location M[(c&255)<<16 + a] (1 byte) (95)
faristrx3: store RX3 at memory location M[(c&255)<<16 + a] (1 byte) (96)

rxadd: RX0 += RX1;(1 byte) (97)
rxsub: RX0 -= RX1;(1 byte) (98)
rxmul: RX0 *= RX1;(1 byte) (99)
rxdiv: RX0 /= RX1;(1 byte) (9A)
rxmod: RX0 %= RX1;(1 byte) (9B)
rxrsh: RX0 >>= RX1;(1 byte) (9C)
rxlsh: RX0 <<= RX1;(1 byte) (9D)

rx0push: push 32 bit RX0 register onto the stack. (1 byte) (9E)
rx1push: push 32 bit RX1 register onto the stack. (1 byte) (9F)
rx2push: push 32 bit RX2 register onto the stack. (1 byte) (A0)
rx3push: push 32 bit RX3 register onto the stack. (1 byte) (A1)

rx0pop: pop 32 bit RX0 register from the stack. (1 byte) (A2)
rx1pop: pop 32 bit RX1 register from the stack. (1 byte) (A3)
rx2pop: pop 32 bit RX2 register from the stack. (1 byte) (A4)
rx3pop: pop 32 bit RX3 register from the stack. (1 byte) (A5)

rxand: RX0 &= RX1(1 byte) (A6)
rxor: RX0 |= RX1(1 byte) (A7)
rxxor: RX0 ^= RX1(1 byte) (bitwise XOR) (A8)
rxcompl: RX0 = ~RX0; bitwise inversion, 1's complement.(1 byte) (A9)
rxcmp:	if(RX0<RX1)a=0;else if(RX0>RX1)a=2;else a=1;  (1 byte) (AA)


seg_ld: load segment RX1 into page RX0 of Main Memory. (1 byte) (AB)
seg_st: store segment RX0 from main memory into page RX1 in the segment. (1 byte) (AC)
seg_realloc: resize the segment to size specified by RX0. if RX0 is 0, then an error flag is returned. (1 byte) (AD)

fltadd: floating point addition, RX0 += RX1, disabled with -DNO_FP (1 byte) (AE)

fltsub: (1 byte) (AF)

fltmul: (1 byte) (B0)

fltdiv: divide by zero is erroneous. (1 byte) (B1)

fltcmp: if(RX0<RX1)a=0;else if(RX0>RX1)a=2;else a=1;, but RX0 and RX1 are treated as floating point (1 byte) (B2)

seg_pages: get the number of pages in the segment in RX0. (1 byte) (B3)

ildrx0_1: indirectly load rx0 by using rx1. (1 byte) (B4)

ildrx0_0: indirectly load rx0 by using rx0. (1 byte) (B5)

farjmprx0: jump to location pointed to by rx0. (1 byte) (B6)

istrx0_1: store rx0 at the location pointed to by rx1. (1 byte) (B7)

istrx1_0: store rx1 at the location pointed to by rx0. (1 byte) (B8)

cbrx0: c = RX0>>16; b = RX0 & 0xffFF; (1 byte) (B9)

carx0: c = RX0>>16; a = RX0 & 0xffFF; (1 byte) (BA)

rxidiv: RX0 = (signed)RX0 / (signed)RX1 (1 byte) (BB)

rximod: RX0 = (signed)RX0 % (signed)RX1 (1 byte) (BC)

farldrx0: load RX0 from constant memory location (4 bytes) (BD)

farldrx1: (4 bytes) (BE)

farldrx2: (4 bytes) (BF)

farldrx3: (4 bytes) (C0)

farllda: (4 bytes) (C1)

farlldb: (4 bytes) (C2)

farldc: (4 bytes) (C3)

farstrx0: store rx0 to constant memory location (4 bytes) (C4)

farstrx1: (4 bytes) (C5)

farstrx2: (4 bytes) (C6)

farstrx3: (4 bytes) (C7)

farstla: (4 bytes) (C8)

farstlb: (4 bytes) (C9)

farstc: (4 bytes) (CA)

aincr: (1 byte) (CB)

adecr: (1 byte) (CC)

rxincr: (1 byte) (CD)

rxdecr: (1 byte) (CE)

emulate: Run a SISA16 sandboxed machine sharing the segment and a single page of choice according to register A (1 byte) (CF)
	Errors in the sandboxed machine are trapped and returned to register A.

	The segment is also sandboxed and a new 256 byte segment is allocated for the child program.
	
	the maximum recursion allowed by the implementation is defined by SISA16_MAX_RECURSION_DEPTH in isa_pre.h

rxitof: convert RX0 from signed int to float. (1 byte) (D0)

rxftoi: convert RX0 from float to signed int. (1 byte) (D1)

emulate_seg: Same as Emulate, but the segment is passed down to the child. (1 byte) (D2)

rxicmp: comparison like rxcmp, but for signed 32 bit integers. (1 byte) (D3)

The rest: nop duplicates, free for expansion (1 byte)
```

Read the documentation for more information.

```
Written by
~~~DMHSW~~~
for the public domain
```
