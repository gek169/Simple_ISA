#!/bin/bash
IFS=$'\n' ASMFILE=$(ls -1|grep '.\.asm')
for filename in $ASMFILE
do
 echo "<SHELL> Assembling $filename"
 name=$(echo "$filename" | cut -f 1 -d '.')
 sisa16_asm -i $filename -o $name.bin
 #./asm -i $filename -o $name.bin -DBG
done
