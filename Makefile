#USE-NCURSES
#CC=cc
CC= gcc
#CC= tcc
INSTALL_DIR=/usr/bin
#CC= clang
CCC= g++
CFLAGS_SMALL= -Os -s -Wl,--gc-sections
MORECFLAGS=
CFLAGS= $(MORECFLAGS) -O3 -std=c89 -pedantic
CASMFLAGS=  $(MORECFLAGS) $(CFLAGS_SMALL) -lm -std=c89 -pedantic
CPPFLAGS= $(MORECFLAGS) -Os -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main asm_programs

sisa16:
	$(CC) $(CFLAGS) -DUSE_NCURSES isa.c -o sisa16 -lncurses || $(CC) $(CFLAGS) isa.c -o sisa16
rbytes:
	$(CC) $(CFLAGS) rbytes.c -o rbytes
sisa16_asm:
	$(CC) $(CASMFLAGS) assembler.c -o sisa16_asm

main: sisa16 sisa16_asm

fifth:
	$(CC) $(CFLAGS) fifth.c -o fifth

cpp_program:
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: sisa16_asm
	./asm_compile.sh

install: main
	@cp ./sisa16* $(INSTALL_DIR)/;echo "Installed into INSTALL_DIR directory." || @echo "ERROR!!! Cannot install sisa16 tools."
	

uninstall:
	rm -f $(INSTALL_DIR)/sisa16
	rm -f $(INSTALL_DIR)/sisa16_asm
	@echo "Uninstalled from INSTALL_DIR."

clean:
	rm -f *.exe *.out *.o *.bin sisa16 isa_constexpr rbytes sisa16_asm fifth
	clear
