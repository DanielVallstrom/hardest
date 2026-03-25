#!/bin/bash

# For running hardest on all combinations of f, t, r 
# where f + t + r = n, with lowSum <= n <= highSum.
# By Claude Opus after pseudo code. 

# Usage: ./shardest.sh <lowSum> <highSum>
# Example: ./shardest.sh 9 10

if [ $# -lt 2 ]; then
    echo "Usage: $0 <lowSum> <highSum>"
    exit 1
fi

lowSum=$1
highSum=$2

for (( n = lowSum; n <= highSum; n++ )); do
    for (( r = 1; r <= (n - 1) / 2; r++ )); do
        t=$(( n - r ))
        f=0
        while (( f <= t )); do
            echo "./hardest -f $f -t $t -r $r -b 0 -i 0"
            ./hardest -f "$f" -t "$t" -r "$r" -b 0 -i 0
            (( t-- ))
            (( f++ ))
        done
    done
done
