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
sc %%;#set to zero
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

sc %0xA0D0%;
lla %0x1%
//these operations should do nothing.
alpush;alpush;
lla %0x0%;
alpop;alpop;
farcall;
#we should see this when we return.
la 0x41
putchar;putchar;putchar;
halt



section 0xB000
!QQQQQQQQQQQQQQQQQQ
section 0xC000
!FFFFFFFFFFFFFFFFF

//defining a function out here.
//this function prints Qs to the screen.
section 0x1A0D0
la 0xa
putchar;putchar;
lla %0xB000%;illdaa;
putchar;putchar;
putchar;putchar;

//test far memory indexing functionality
lla %0xffea%;
sc %0xff%;
lb 0x59
faristlb;
lb 0xa
farilldb;
ab;
putchar;putchar;
putchar;


//test far memory indexing functionality for bytes
lla %0xeeff%;
sc %0xed%;
lb 0x57
faristb;
lb 0xa
farildb;
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
farret;#return back to the zero page.


//Testing variable addressing.
//This code is never executed.
section @+1+
sc $+1+;sc $+1+;sc $+1+;sc $+1+;sc $+1+;sc $+1+;
section 0xFFFF00
!KAKAKAIII
imul3.13

VAR#somethingg#$
bytes somethingg
sc %5%;
bytes somethingg
