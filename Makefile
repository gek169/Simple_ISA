CC= gcc
CCC= g++
#CC= clang
CFLAGS= -O3 -lm -Wno-unused-function -Wno-absolute-value -std=gnu11 -finline-limit=64000 -fno-math-errno
CPPFLAGS= -O3 -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main

main:
	$(CC) $(CFLAGS) *.c -o isa.out
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr.out

clean:
	rm -f *.exe *.out *.o
