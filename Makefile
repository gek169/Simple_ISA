CC = gcc

INSTALL_DIR     = /usr/bin
MAN_INSTALL_DIR = /usr/share/man/man1

GIT_HASH = $(shell git rev-parse > /dev/null 2>&1 && git rev-parse --short HEAD || echo no)
OPTLEVEL    = -O3 -s -march=native -DSISA_GIT_HASH=\"$(GIT_HASH)\" 
#" dont remove this it fixes syntax highlighting in my editor

MORECFLAGS  =  -DUSE_TERMIOS -DUSE_UNSIGNED_INT -DATTRIB_NOINLINE -DUSE_COMPUTED_GOTO
CFLAGS_NOERR = -Wno-pointer-sign -Wno-format-security
CFLAGS      = $(MORECFLAGS) $(OPTLEVEL) $(CFLAGS_NOERR)

STATIC = -static

# ------=========------ COLORS ------=========------ #
ifndef NO_COLORS
COLOR_RED     = "\033[0;31m"
COLOR_GREEN   = "\033[0;32m"
COLOR_YELLOW  = "\033[0;33m"
COLOR_BLUE    = "\033[0;34m"
COLOR_PURPLE  = "\033[0;35m"
COLOR_CYAN    = "\033[0;36m"
# TODO: less repetitive
COLOR_BRED    = "\033[1;31m"
COLOR_BGREEN  = "\033[1;32m"
COLOR_BYELLOW = "\033[1;33m"
COLOR_BBLUE   = "\033[1;34m"
COLOR_BPURPLE = "\033[1;35m"
COLOR_BCYAN   = "\033[1;36m"

COLOR_RESET   = "\033[0;0m"
endif

all: main #asm_programs
	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Done building." $(COLOR_RESET)

#all_sdl2: main_sdl2 #asm_programs
#	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Done building with SDL2." $(COLOR_RESET)

assembler2: 
	$(CC) $(CFLAGS) $(STATIC) assembler2.c -o s16asm2
	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Built assembler2." $(COLOR_RESET)

sisa16_emu:
	$(CC) $(CFLAGS) $(STATIC) isa.c -o sisa16_emu 
	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Built emulator." $(COLOR_RESET)

sisa16_asm:
	$(CC) $(CFLAGS) $(STATIC) assembler.c -o sisa16_asm 
	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Built assembler (Which has an emulator built into it.)" $(COLOR_RESET)

sisa16_dbg:
	$(CC) $(CFLAGS) $(STATIC) debugger.c -o sisa16_dbg
	@printf "%b%s%b\n" $(COLOR_BCYAN) "~~Built debugger." $(COLOR_RESET)

# SDL2 versions, if you want to mess with graphics and audio


main: sisa16_asm sisa16_emu sisa16_dbg
#main_sdl2: sisa16_sdl2_asm sisa16_sdl2_emu sisa16_sdl2_dbg

asm: sisa16_asm
	./asm_compile.sh

#used by github actions.
check:
#effectively, a check.
	sudo $(MAKE) -B libc
	sudo $(MAKE) clean
	./asmbuild.sh
	sisa16_asm -C
	sisa16_asm -v
	sisa16_asm -dis clock.bin 0x10000
	sisa16_asm -dis clock.bin 0x20000
	sisa16_asm -fdis echo.bin 0
	sisa16_asm -dis echo2.bin 0x20000
	sisa16_asm -fdis switchcase.bin 0
	sisa16_asm -fdis controlflow_1.bin 0
	
install: sisa16_asm sisa16_emu sisa16_dbg
	@cp ./sisa16_emu $(INSTALL_DIR)/ || cp ./sisa16_emu.exe $(INSTALL_DIR)/ || echo "ERROR!!! Cannot install sisa16_emu"
	@cp ./sisa16_dbg $(INSTALL_DIR)/ || cp ./sisa16_dbg.exe $(INSTALL_DIR)/ || echo "ERROR!!! Cannot install sisa16_dbg"
	@cp ./sisa16_asm $(INSTALL_DIR)/ || cp ./sisa16_asm.exe $(INSTALL_DIR)/ || echo "ERROR!!! Cannot install sisa16_asm"
	@mkdir /usr/include/sisa16/ || echo "sisa16 include directory either already exists or cannot be created."
	@cp *.hasm /usr/include/sisa16/ || echo "ERROR!!! Cannot install libc.hasm. It is the main utility library for sisa16"
	@cp ./*.1 $(MAN_INSTALL_DIR)/ || echo "Could not install manpages."

libc: install
	sisa16_asm -i libc.asm -o libc.bin
	@mkdir /usr/include/sisa16/ || echo "sisa16 include directory either already exists or cannot be created."
	@cp *.hasm /usr/include/sisa16/ && echo "Installed libraries." || echo "ERROR!!! Cannot install headers"
	@cp libc.bin.hasm.tmp /usr/include/sisa16/libc_pre.hasm && echo "Installed libraries." || echo "ERROR!!! Cannot install libc_pre.hasm. It is the main utility library for sisa16"
	@cp libc.bin /usr/include/sisa16/libc_pre.bin && echo "Installed libraries." || echo "ERROR!!! Cannot install libc_pre.bin It is the main utility library for sisa16"

uninstall:
	rm -f $(INSTALL_DIR)/sisa16*
	rm -f $(MAN_INSTALL_DIR)/sisa16_emu.1
	rm -f $(MAN_INSTALL_DIR)/sisa16_dbg.1
	rm -f $(MAN_INSTALL_DIR)/sisa16_asm.1
	@echo "Uninstalled."
	@echo "Note that if you have libraries under /usr/include/sisa16/, they were *not* removed."

clean:
	rm -f *.exe *.dbg *.out *.dsk *.o *.bin *.tmp sisa16_emu sisa16_asm sisa16_dbg sisa16_sdl2_emu sisa16_sdl2_asm sisa16_sdl2_dbg s16asm2
# clear || echo "cannot clear?"



q:
	admin $(MAKE) -B libc
	admin $(MAKE) clean
	git add .
	git commit -m "Developer time" || echo "nothing to commit"
	git push --force || echo "nothing to push"

d:
	admin $(MAKE) -B libc
	admin $(MAKE) clean
	./asmbuild.sh


