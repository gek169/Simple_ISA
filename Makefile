CC=cc
#CC= gcc
#CC= tcc
#CC= clang
CCC= g++
CFLAGS= -O3 -std=c89 -pedantic -mtune=native
#CFLAGS= -Os -lm -std=c89 -pedantic
CASMFLAGS= -Os -lm -std=c89 -pedantic
CPPFLAGS= -Os -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno
all: main asm_programs

main:
	$(CC) $(CFLAGS) isa.c -o isa
	$(CC) $(CFLAGS) rbytes.c -o rbytes
	$(CC) $(CASMFLAGS) assembler.c -o asm

cpp_program:
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: main
	./asm_compile.sh

clean:
	rm -f *.exe *.out *.o *.bin isa isa_constexpr rbytes asm
	clear
