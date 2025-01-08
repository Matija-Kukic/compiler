#!/bin/bash

# Loop through numbers 1 to 21
for i in $(seq 1 25); do
    # Format the number to always have two digits
    test_number=$(printf "%02d" $i)
    
    # Run the command
    ./analizatorL < "../integration3/primjer$test_number"/test.c | ./analizatorS > "../integration3/primjer$test_number/test.in"
done
