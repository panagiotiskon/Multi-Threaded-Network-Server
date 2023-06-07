#!/bin/bash

numoflines=$2
filepath=$1 
num=$#


if [ -f "inputFile" ]; then    # if the file exists delete the old one
    rm "inputFile"
fi

if [ "$num" != 2 ]; then 
    echo "Wrong Input"
    exit -1
fi 

touch inputFile

for((i = 0; i< numoflines; i++))
    do
        name_length=$((RANDOM % 10 + 3))  #random name length
        surname_length=$((RANDOM % 10 + 3))  #random surname length

        name_first=$(cat /dev/urandom | tr -dc 'A-Z'| fold -w 1| head -n 1)     
        name_rest=$(cat /dev/urandom | tr -dc 'a-z'| fold -w "$(($name_length-1))"| head -n 1)
        name="$name_first$name_rest"

        surname_first=$(cat /dev/urandom | tr -dc 'A-Z'| fold -w 1| head -n 1)
        surname_rest=$(cat /dev/urandom | tr -dc 'a-z'| fold -w "$(($surname_length-1))"| head -n 1)
        surname="$surname_first$surname_rest"

        vote=$(shuf -n 1 "$filepath")   # pick a Party
        new_vote="$name $surname $vote"
        echo "$new_vote" >> inputFile
    done