#!/bin/bash

function center ()
{
 _STR=$1
 _ROW=$2
 LEN=`echo "$_STR" | wc -c`
 COLS=`tput cols`
 HOLD_COL=`expr $COLS - $LEN`
 NEW_COL=`expr $HOLD_COL / 2`
 tput cup $_ROW $NEW_COL
 echo -e -n "\033[46;37m $_STR \033[0m"
}
function ech ()
{
 _STR=$1
 echo -e "\033[46;37m $_STR \033[0m"
}
clear
center "###############################################" 2
center "#           Special riscv simulator           #" 3
center "#   for running and debugging risc-v program  #" 4
center "#    Author: Huang,Wang,Xiao  Modified: too   #" 5
center "###############################################" 6
center "the test program name:" 7
read filename
clear
if [ ! -x "$(pwd)/$filename" ]; then
echo "$filename didn't exist"
exit
else
ech "1.Run"
ech "2.Debug"
ech "3.Pace"
ech "4.Trace"
ech "5.Count"
ech ""
ech "Copy Right®2016 Group627"
ech "All rights reserved"
read var
clear
if [[ var -eq "1" ]]; then
./riscv-simulator -r "$filename"
elif [[ var -eq "2" ]]; then
./riscv-simulator -d "$filename"
elif [[ var -eq "3" ]]; then
./riscv-simulator -p "$filename"
elif [[ var -eq "4" ]]; then
./riscv-simulator -t "$filename"
elif [[ var -eq "5" ]]; then
./riscv-simulator -c "$filename"
else
ech "wrong instruction!"
fi
fi
