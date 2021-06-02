#USE-NCURSES
#CC=cc
CC= gcc
#CC= tcc
#CC= clang
INSTALL_DIR=/usr/bin
MAN_INSTALL_DIR=/usr/share/man/man1
CCC= g++
OPTLEVEL= -Os -march=native
#make the emulator safe for fuzz testing.
MORECFLAGS=-DFUZZTEST
CFLAGS= $(MORECFLAGS) $(OPTLEVEL) -s -std=c89 -pedantic
CASMFLAGS=  $(MORECFLAGS) $(OPTLEVEL) -s -std=c89 -pedantic
CPPFLAGS= $(MORECFLAGS) $(OPTLEVEL) -lm -Wno-unused-function -Wno-absolute-value -std=c++17 -finline-limit=64000 -fno-math-errno

all: main asm_programs

sisa16_emu:
	$(CC) $(CFLAGS) -DUSE_NCURSES isa.c -o sisa16_emu -lncurses -DUSE_UNSIGNED_INT || $(CC) $(CFLAGS) isa.c -o sisa16_emu -DUSE_UNSIGNED_INT
	@echo "~~Built emulator."
sisa16_asm:
	$(CC) $(CASMFLAGS) assembler.c -o sisa16_asm -DUSE_UNSIGNED_INT -DUSE_NCURSES -lncurses || $(CC) $(CASMFLAGS) assembler.c -o sisa16_asm -DUSE_UNSIGNED_INT 
	@echo "~~Built assembler (Which has an emulator built into it.)"
rbytes:
	$(CC) $(CFLAGS) rbytes.c -o rbytes

main: sisa16_asm sisa16_emu

check: asm_programs

cpp_program:
	$(CCC) $(CPPFLAGS) *.cpp -o isa_constexpr

asm_programs: sisa16_asm
#effectively, a check.
	./asm_compile.sh
	./sisa16_asm -C
	./sisa16_asm --help

	
install: main
	@cp ./sisa16_emu $(INSTALL_DIR)/ || cp ./sisa16_emu.exe $(INSTALL_DIR)/ || echo "ERROR!!! Cannot install sisa16_emu"
	@cp ./sisa16_asm $(INSTALL_DIR)/ || cp ./sisa16_asm.exe $(INSTALL_DIR)/ || echo "ERROR!!! Cannot install sisa16_asm"
	@echo "Attempting manpage install."
	@cp ./*.1 $(MAN_INSTALL_DIR)/ || echo "Could not install manpages."


uninstall:
	rm -f $(INSTALL_DIR)/sisa16*
	rm -f $(INSTALL_DIR)/sisa16
	rm -f $(INSTALL_DIR)/sisa16_emu
	rm -f $(INSTALL_DIR)/sisa16_stdin
	rm -f $(INSTALL_DIR)/sisa16_asm
	rm -f $(MAN_INSTALL_DIR)/sisa16_emu.1
	rm -f $(MAN_INSTALL_DIR)/sisa16.1
	rm -f $(MAN_INSTALL_DIR)/sisa16_asm.1
	@echo "Uninstalled from INSTALL_DIR."

clean:
	rm -f *.exe *.out *.o *.bin sisa16_emu sisa16 sisa16_stdin isa_constexpr rbytes sisa16_asm
	clear
