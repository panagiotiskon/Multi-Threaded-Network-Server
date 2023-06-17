#!/bin/bash

output="$1"
path="$PWD/inputFile"

if [ -e "$path" ] && [ -r "$path" ]; then           #check if file exists and has read wrights
    awk '!seen[$1, $2]++ { count[$3]++ } END {for (key in count) print key, count[key]} ' "$path" | sort -k2 -rn > "$output"               #with seen  avoid duplicates keeping only the first occurance
else 
    echo "File does not exist or does not have proper rights"
    exit 1
fi
