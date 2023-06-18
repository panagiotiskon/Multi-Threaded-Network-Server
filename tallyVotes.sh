#PANAGIOTIS KONTOEIDIS
#1115201900266

#!/bin/bash

output="$1"
path="$PWD/inputFile"

if [ -e "$path" ] && [ -r "$path" ]; then           #check if file exists and has read rights
    awk '!seen[$1, $2]++ { count[$3]++ } END {for (vote in count) print vote, count[vote]} ' "$path" | sort -k2 -rn > "$output"  #with seen avoid duplicates keeping only the first occurance
else                                                                                                                            # and count the number of occurances 
    echo "File does not exist or does not have proper rights"                                                                   # and then sort them in the right order
    exit 1
fi
