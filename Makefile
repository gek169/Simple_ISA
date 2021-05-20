#USE-NCURSES
#CC=cc
CC= gcc
#CC= tcc
INSTALL_DIR=/usr/bin
MAN_INSTALL_DIR=/usr/share/man/man1
#CC= clang
CCC= g++
CFLAGS_SMALL= -Os -s
MORECFLAGS=
CFLAGS= $(MORECFLAGS) -O3 -s -std=c89 -pedantic
CASMFLAGS=  $(MORECFLAGS) -O3 -s -lm -std=c89 -pedantic
CPPFLAGS= $(MORECFLAGS) -Os -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main asm_programs

sisa16:
	$(CC) $(CFLAGS) -DUSE_NCURSES isa.c -o sisa16 -lncurses -DUSE_UNSIGNED_INT || $(CC) $(CFLAGS) isa.c -o sisa16 -DUSE_UNSIGNED_INT
	@echo "~~Built emulator."
sisa16_asm:
	$(CC) $(CASMFLAGS) assembler.c -o sisa16_asm
	@echo "~~Built assembler"
rbytes:
	$(CC) $(CFLAGS) rbytes.c -o rbytes

main: sisa16_asm sisa16

fifth:
	$(CC) $(CFLAGS) fifth.c -o fifth

cpp_program:
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: sisa16_asm
	./asm_compile.sh

install: main manpage_install
	@cp ./sisa16 $(INSTALL_DIR)/;echo "Installed sisa16 into INSTALL_DIR directory." || @cp ./sisa16.exe $(INSTALL_DIR)/;echo "Installed sisa16.exe into INSTALL_DIR directory." || @echo "ERROR!!! Cannot install sisa16"
	@cp ./sisa16_asm $(INSTALL_DIR)/;echo "Installed sisa16_asm into INSTALL_DIR directory." || @cp ./sisa16_asm.exe $(INSTALL_DIR)/;echo "Installed sisa16_asm.exe into INSTALL_DIR directory." || @echo "ERROR!!! Cannot install sisa16_asm"
manpage_install:
	@cp ./*.1 $(MAN_INSTALL_DIR)/


uninstall:
	rm -f $(INSTALL_DIR)/sisa16
	rm -f $(INSTALL_DIR)/sisa16_asm
	rm -f $(MAN_INSTALL_DIR)/sisa16.1
	rm -f $(MAN_INSTALL_DIR)/sisa16_asm.1
	@echo "Uninstalled from INSTALL_DIR."

clean:
	rm -f *.exe *.out *.o *.bin sisa16 isa_constexpr rbytes sisa16_asm fifth
	clear
