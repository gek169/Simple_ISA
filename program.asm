#Compile this program using asm. ./asm -i program.asm -o program.bin
#section 0
#fill 0xFFFF,0

section 0xF000
!hello world!!! You should see this print.
bytes 0xA, 0xA;
!Another string.
bytes 0xA;
bytes 0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42;
bytes 0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42;
bytes 0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42;
bytes 0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42;

section 0
#loop control
lda %0x1e8%
lb 1
sc %0%
add
#using split syntax
sta %0x1e8%
#grab our character from our array of characters
llb %1%;sub;sub;
llb %0xF000%
add
illdaa;
llb %255%;and;
putchar



#perform the loop check
lda 1,0xe8
lb 0x41
cmp;lb 0;cmp;jmpifeq


//zero the stack pointer and then set it to a sensible value
astp;popa;lla %0xE000%;pusha;
lda %0x1e8%;putchar;
sc %0xA000%;
call;
#we should see this when we return.
la 0x45
putchar;putchar;putchar;
halt

section 0xB000
!QQQQQQQQQQQQQQQQQQ
section 0xC000
!FFFFFFFFFFFFFFFFF

//defining a function out here.
//this function prints Qs to the screen.
section 0xA000
la 0xa
putchar;putchar;
lla %0xB000%;illdaa;
putchar;putchar;
putchar;putchar;

//test far memory indexing functionality
sc %0xffff%;asm_print;
lla 0,0xea;asm_print;
lb 0x59
faristlb;
lb 0xa
farilldb;
ab;
putchar;putchar;
putchar;
//test farpagel and farpagest
lla%0xee%;
sc %0xffff%
farpagel;
//Copy that page around some so we can see it.
sc %0xfffe%
farpagest;
sc %0xfffd%
farpagest;
sc %0xfffc%
farpagest;
sc %0xfffb%
farpagest;
la 0xa
putchar;putchar;
ret
//Testing variable addressing.
section @+1+

//set up a loop


sc $+1+;

section 0xFFFF00
!KAKAKAIII
