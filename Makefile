CC= tcc
CCC= g++
#CC= clang
CFLAGS= -O3 -lm -std=c89 -pedantic
CASMFLAGS= -O3 -lm -std=c99 -pedantic
CPPFLAGS= -O3 -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno
all: main asm_programs

main:
	$(CC) $(CFLAGS) isa.c -o isa
	$(CC) $(CFLAGS) rbytes.c -o rbytes
	$(CC) $(CASMFLAGS) assembler.c -o asm
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: main
	./asm_compile.sh

clean:
	rm -f *.exe *.out *.o *.bin isa isa_constexpr rbytes asm
	clear
