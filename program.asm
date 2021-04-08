#Compile this program using asm. ./asm -i program.asm -o program.bin
section 0
lda 1,0xe8
lb 1
sc 0,0
add
sta 1,0xe8


#Set register B to a value- 0x41 which is 65.
lb 0x41
#Compare- A and B being equal sets A to 1. 0 if less, 2 if greater
cmp
#store zero int B
lb 0
#Do another compare.
cmp
#Jump to zero
jmpifeq
#Load the value at 1e8 into register A
lda 1,0xe8
#Print it
putchar
#Halt execution
#alternatively, halt by dividing by zero
la 1
lb 0
mod
halt
