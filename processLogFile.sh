#!/bin/bash

output=pollerResultsFile
path="$PWD/$1"

if [ -e "$path" ] && [ -r "$path" ]; then 
    awk '!seen[$1, $2]++ { count[$3]++ } END {for (key in count) print key, count[key]} ' "$path" | sort > "$output"               #with seen  avoid duplicates keeping only the first occurance
else 
    echo"File does not exist or does not have proper rights"
    exit 1
fi