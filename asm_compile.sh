#!/bin/bash
IFS=$'\n' ASMFILE=$(ls -1|grep '.\.asm')
for filename in $ASMFILE
do
 echo "Assembling $filename"
 name=$(echo "$filename" | cut -f 1 -d '.')
 ./asm -i $filename -o $name.bin
done
