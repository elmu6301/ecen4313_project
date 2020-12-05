# ECEN 4313: Concurrent Programming
# Author: Elena Murray
# Date: X/XX/2020
# Lab X: 
# 	Makefile

all: main #accountUT

#Executable for main
main: main.o bank.o bank_tester.o 
	g++ main.o  bank.o bank_tester.o -pthread -fgnu-tm -mrtm -g -o main
	# g++  -pthread -g -o mysort

#Object Files
main.o: main.cpp 
	g++ -c main.cpp

bank.o: bank/bank.cpp bank/bank.hpp
	g++ -c bank/bank.cpp -fgnu-tm -mrtm

bank_tester.o: tester/bank_tester.cpp tester/bank_tester.hpp
	g++ -c tester/bank_tester.cpp -fgnu-tm


#Clean
clean:
	rm *.o main