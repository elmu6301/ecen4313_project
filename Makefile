# ECEN 4313: Concurrent Programming
# Author: Elena Murray
# Date: X/XX/2020
# Lab X: 
# 	Makefile

all: bank

#Executable for bank
bank: main.o bank.o bank_tester.o ticket_lock.o
	g++ main.o bank.o bank_tester.o ticket_lock.o -pthread -fgnu-tm -mrtm -g -o bank

#Object Files
main.o: main.cpp 
	g++ -c main.cpp

bank.o: bank.cpp bank.hpp
	g++ -c bank.cpp -fgnu-tm -mrtm 

bank_tester.o: bank_tester.cpp bank_tester.hpp
	g++ -c bank_tester.cpp -fgnu-tm

ticket_lock.o: ticket_lock.cpp ticket_lock.hpp
	g++ -c ticket_lock.cpp 

#Clean
clean:
	rm *.o bank