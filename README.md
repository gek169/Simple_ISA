# Simple, Embeddable, Performant Virtual Machine Language

SISA16 is an extremely lightweight virtual machine designed to replace languages like Lua for program extension.

it executes identically on every major platform and computer architecture. 

Platforms Tested and confirmed for 100% compliance:

```
	Windows 10 x86_64 (i7-6700) on MSYS2
	Windows 10 x86_64 (i7-6700) on WSL2
	Debian 11 x86_64 (i7-6700)
	Raspbian ArmV8 (BCM2837B0)
	Debian 11 IA32 (i7-6700)
	Alpine Linux IA32 (JSlinux)
	Buildroot Linux Riscv64 (JSLinux)
```

Platforms planned to be tested:

```
	Debian ppc32
	(any) ppc64
	(any) ppc64le
	(any) Mips64el
	(any) Mips64be
	(any) Mips64le
	Sega Naomi Hitachi Sh-4
```

## Why is SISA16 special?

* Minimal dependence on the host operating system. A very minimal C standard library will build the assembler
	and emulator just fine.

* CPU architecture independence- The behavior of the VM is host-independent. There are only two behaviors which may vary by architecture:
	* Floating point numbers. Their layout in memory may vary, although IEEE-754 compliance is nearly ubiquitous...
	* Signed integers. Non-twos-complement architectures are not supported, and the sign bit must be the highest bit.

	Luckily, virtually every architecture around today guarantees both of these things.

	The emulator and assembler are confirmed to work on literally dozens of architectures and operating systems.

* Unique. SISA16 is no ordinary virtual machine language...

	* Privileged- Has a "privileged" and "user" mode, w

	* Tiny- Less than 100 kilobytes for a full emulator and assembler toolchain!

	* Hackable- More instructions can be added at a whim.

	* Fast. a 2.2 GHZ Ryzen 7 will execute code with branches at ~1.09 Giga-IPS. 
		a 4.0 ghz i7-6700 will execute that same code at ~1.15 Giga-IPS.

* Trivially embeddable. Implement five small and easy-to-understand functions in a single header file
	to add SISA16 scripting to any system.

* Ready for deployment. Assembler, Debugger, Disassembler, manuals. Everything is here.

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

See the included programs. they are named ".asm"

libraries are postfixed with ".hasm"

The disassembly looks like this:

![disassembly output](disassembly.png)


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

Here is how you use the emulator on a unix system. Note that Ncurses is optional, but highly recommended.

You should read the pdf or odt manual in this repository for more compilation flags to improve emulation performance and access more features.
```bash
#compile the emulator
#most systems have UNSIGNED_INT as four byte unsigned types, that's what the define is for.
cc isa.c -o sisa16_emu -DUSE_UNSIGNED_INT -DUSE_NCURSES -lncurses
#compile the assembler
cc assembler.c -o sisa16_asm -DUSE_UNSIGNED_INT  -DUSE_NCURSES -lncurses
#compile the debugger
cc debugger.c -o sisa16_dbg -DUSE_UNSIGNED_INT  -DUSE_NCURSES -lncurses
#assemble your program
./sisa16_asm -i program.asm -o program.bin
#run your program
./sisa16_emu program.bin
#debug your program
./sisa16_dbg program.bin
#install sisa16
sudo cp sisa16_asm /usr/bin/
sudo cp sisa16_emu /usr/bin/
sudo cp sisa16_dbg /usr/bin/

#run asm program starting with shebang as script
#the shebang looks like this if you use /usr/bin/ (usr/local/bin/ is the default for sisa16)
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


Read the documentation for more information.

sisa16_asm.1

Manual.pdf

```
Written by
~~~DMHSW~~~
for the public domain
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~Let all that you do be done with love~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

```
