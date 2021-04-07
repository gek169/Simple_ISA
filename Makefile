CC= gcc
CCC= g++
#CC= clang
CFLAGS= -O3 -lm -std=c89 -pedantic
CPPFLAGS= -O3 -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main

main:
	$(CC) $(CFLAGS) ISA_example.c -o isa
	$(CC) $(CFLAGS) rbytes.c -o rbytes
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

clean:
	rm -f *.exe *.out *.o isa isa_constexpr rbytes
