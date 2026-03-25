#!/bin/bash

# For running hardest on all combinations of f, t, r 
# where f + t + r = n, with lowSum <= n <= highSum, and r fixed.

# Usage: ./shardestr2.sh <lowSum> <highSum> <r>
# Example: ./shardestr2.sh 19 20 1

if [ $# -lt 3 ]; then
    echo "Usage: $0 <lowSum> <highSum> <r>"
    exit 1
fi

lowSum=$1
highSum=$2
r=$3

for (( n = lowSum; n <= highSum; n++ )); do
    t=$(( n - r ))
    f=0
    while (( f <= t )); do
        echo "./hardest -f $f -t $t -r $r -b 0 -i 0"
        ./hardest -f "$f" -t "$t" -r "$r" -b 0 -i 0
        (( t-- ))
        (( f++ ))
    done
done
