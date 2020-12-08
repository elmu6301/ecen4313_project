#ifndef BANK_TESTER_HPP
#define BANK_TESTER_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 12/1/2020
Final Project

    
*/
/*************************************************
	FILE INCLUDES
**************************************************/
using namespace std;

#include <stdio.h>
#include <string>
#include <vector>
#include <list>

#include "bank.hpp"

/*************************************************
	STRUCTURES
**************************************************/
struct TXN_t{
    string action; 
    int toID; 
    int fromID; 
    float amt; 
    int txnID; 
}; 


struct bank_thread_args {
    size_t tid; //Thread id
    std::vector<TXN_t> array;
    unsigned long long elapsed_ns; 
    double elapsed_s ; 
    float throughput_ns; 
    float throughput_s; 
}; 

/*************************************************
	FUNCTION DECLARATIONS
**************************************************/

int bank_tester(int num_threads,  Bank &myBank, std::vector <TXN_t> &data ); 

#endif