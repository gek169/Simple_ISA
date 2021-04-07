/*See program.bin for compiled version.*/
/*Load the value at 1e8 into register A*/
[0]1
[1]1
[2]e8
/*Set register B to 1*/
[3]4
[4]1
/*Load 0 into C*/
[5]5
[6]0
[7]0
/*A+=B*/
[8]8
/*store a into 1e8*/
[9]6
[a]1
[b]e8


/*Set register B to a value- 0x41 which is 65.*/
[c]4
[d]41
/*Compare- A and B being equal sets A to 1. 0 if less, 2 if greater */
[e]d
/*store zero int B*/
[f]4
[10]0
/*Do another compare.*/
[11]d
/*Jump to zero*/
[12]e
/*Load the value at 1e8 into register A*/
[13]1
[14]1
[15]e8
/*Print it*/
[16]11
/*Halt execution*/
[17]0
