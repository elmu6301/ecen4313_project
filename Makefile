# ECEN 4313: Concurrent Programming
# Author: Elena Murray
# Date: X/XX/2020
# Lab X: 
# 	Makefile

all: main #accountUT

#Executable for main
main: main.o bank_account.o bank.o bank_tester.o 
	g++ main.o bank_account.o bank.o bank_tester.o -pthread -g -o main
	# g++  -pthread -g -o mysort

#Object Files
main.o: main.cpp 
	g++ -c main.cpp

bank_account.o: account/bank_account.cpp account/bank_account.hpp
	g++ -c account/bank_account.cpp

bank.o: bank/bank.cpp bank/bank.hpp
	g++ -c bank/bank.cpp

bank_tester.o: tester/bank_tester.cpp tester/bank_tester.hpp
	g++ -c tester/bank_tester.cpp


#Clean
clean:
	rm *.o main