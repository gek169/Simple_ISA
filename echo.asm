//Load 0 into register C
//Jump to code location
[0]05
[1]01
[2]F0
//store 1 into A
[11]2
[12]1
//jump
[13]E

<01F0>
[0]05
[1]01
[2]F0
//Read character from stdin into A
[3]10
//Echo it back
[4]11
//some nops for room.
[5]1E
[6]1E
[7]1E
[8]1E
[9]1E
[10]1E
//store 1 into A
[11]2
[12]1
//jump to 0
[13]E
//Unreachable halt.
[14]0
