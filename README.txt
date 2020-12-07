ECEN 4313 Concurrent Programming
Final Project Write Up
Elena Murray

1. Overview
2. File Description
3. Compliation Instruction
4. Execution Instruction
5. Known Bugs

cmp --silent test_files/ledger.txt test_files/high_contention/ledger_soln.txt && echo "Same!" || echo "Different"

./bank --init test_files/high_contention/init_data.txt --txn test_files/high_contention/txn_data.txt -o test_files/ledger.txt -t 3 --alg=sgl


./bank --init test_files/high_contention/init_data.txt --txn test_files/high_contention/txn_data.txt -o test_files/ledger.txt -t 3 --alg=sgl