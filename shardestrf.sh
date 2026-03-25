#!/bin/bash

# For running hardest on all combinations of f, t, r 
# where f + t + r = n, with lowSum <= n <= highSum, and r and f fixed.

# Usage: ./shardestrf.sh <lowSum> <highSum> <r> <f>
# Example: ./shardestrf.sh 100 256 1 0

if [ $# -lt 4 ]; then
    echo "Usage: $0 <lowSum> <highSum> <r> <f>"
    exit 1
fi

lowSum=$1
highSum=$2
r=$3
f=$4

for (( t = lowSum - r - f; t <= highSum - r - f; t++ )); do
    echo "./hardest -f $f -t $t -r $r -b 0 -i 0"
    ./hardest -f "$f" -t "$t" -r "$r" -b 0 -i 0
done
