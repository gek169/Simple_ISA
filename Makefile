#CC=cc
#CC= gcc
#CC= tcc
CC= clang
CCC= g++
CFLAGS= -Ofast -std=c89 -pedantic -g
#CFLAGS= -Os -std=c89 -pedantic
CASMFLAGS= -Os -lm -std=c89 -pedantic
CPPFLAGS= -Os -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main asm_programs

isa:
	$(CC) $(CFLAGS) isa.c -o isa -lncurses
rbytes:
	$(CC) $(CFLAGS) rbytes.c -o rbytes
assembler:
	$(CC) $(CASMFLAGS) assembler.c -o asm

main: isa rbytes assembler

fifth:
	$(CC) $(CFLAGS) fifth.c -o fifth

cpp_program:
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: assembler
	./asm_compile.sh

clean:
	rm -f *.exe *.out *.o *.bin isa isa_constexpr rbytes asm fifth
	clear
