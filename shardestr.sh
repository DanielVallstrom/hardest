#!/bin/bash

# For running hardest on all combinations of f and t 
# where f + t + r = n, with fixed n and r.

# Usage: ./shardestr.sh <n> <r>
# Example: ./shardestr.sh 19 1

if [ $# -lt 2 ]; then
    echo "Usage: $0 <n> <r>"
    exit 1
fi

n=$1
r=$2

t=$(( n - r ))
f=0
while (( f <= t )); do
    echo "./hardest -f $f -t $t -r $r -b 0 -i 0"
    ./hardest -f "$f" -t "$t" -r "$r" -b 0 -i 0
    (( t-- ))
    (( f++ ))
done
