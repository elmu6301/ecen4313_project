#!/bin/bash
# Runs all transaction algorithms for a given input, transaction files

#Test against 
threads=2
while getopts i:d:t:l: flag
do 
    case "${flag}" in
        i) init=${OPTARG};; 
        d) txns=${OPTARG};;  
        t) threads=${OPTARG};; 
        l) test_soln=${OPTARG};; 
    esac
done

#Files

test_out="test_files/ledger.txt"
echo "Running bank with:"
echo "Threads: $threads"
echo "Initialization File: $init"
echo "Transaction File: $txns"
echo "Outputting to $test_out"
echo "Comparing against: $test_soln"

echo 


alg=('sgl' 'p2l' 'stm' 'htm' 'opt')
test_res=('SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS')
test_case=1
n=1 #5
i=0
while [ $i -lt $n ]
do
        
    echo "Test Case ${test_case}-> -t ${threads} --alg=${alg[$i]}"
  

    # rm ${test_out}

    ./bank --init ${init} --txn ${txns} -o ${test_out} -t ${threads} --alg=${alg[$i]} 
    # cmp ${test_out} ${test_sol} && echo "SUCCESS" || "FAIL" #test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
    
    test_case=$((test_case + 1))
    i=$((i + 1))
    echo 
done

#Print results
echo "Results:"
i=0
while [ $i -lt $n ]
do
    p=$((i + 1))
    echo "TEST[$p]--> ${test_res[$i]}"
    i=$((i + 1))
done