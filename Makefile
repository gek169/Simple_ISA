CC= gcc
#CC= clang
CFLAGS= -O3 -lm -fopenmp -Wno-unused-function -Wno-absolute-value -fgnu89-inline -std=gnu11 -finline-limit=64000 -fno-math-errno

all: main

main:
	$(CC) $(CFLAGS) *.c -o isa.out

clean:
	rm -f *.exe *.out *.o
