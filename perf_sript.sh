#!/bin/bash

#Test against 
test_case=1
threads=10
while getopts t:c: flag
do 
    case "${flag}" in
        t) threads=${OPTARG};; 
        c) test_case=${OPTARG};; 
    esac
done

#Files
test_in="test_files/test_case$test_case.txt"; 
test_sol="test_files/test_soln$test_case.txt"; 
test_out="test_files/output$test_case.txt"

echo "Running mysort with:"
echo "Threads: $threads"
echo "Test Case: $test_case"
echo "Input File: $test_in"
echo "Output File: $test_out"
echo "Solution File: $test_sol"

test_res=('SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS' 'SUCCESS')
i=0
n=8
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=pthread --lock=pthread"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=pthread --lock=pthread
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 


echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=pthread --lock=tas"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=pthread --lock=tas
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=pthread --lock=ttas"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=pthread --lock=ttas
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=pthread --lock=ticket"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=pthread --lock=ticket
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

# Sense Barrier Test Case
echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=sense --lock=pthread"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=sense  --lock=pthread
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=sense --lock=tas"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=sense  --lock=tas
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=sense --lock=ttas"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=sense --lock=ttas
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
i=$((i + 1))
echo 

echo "Test Case ${test_case}:${i}-> -t ${threads} --bar=sense --lock=ticket"
rm ${test_out}
perf stat -e L1-dcache-load-misses -e L1-dcache-load -e branch-misses -e branches ./mysort ${test_in} -o ${test_out}  -t ${threads} --alg=bucket --bar=sense --lock=ticket
cmp ${test_out} ${test_sol} && test_res[$i]="SUCCESS" || test_res[$i]="FAILED"
echo 

echo "Results:"
i=0
while [ $i -lt $n ]
do
    echo "TEST[${i}]--> ${test_res[$i]}"
    i=$((i + 1))
done