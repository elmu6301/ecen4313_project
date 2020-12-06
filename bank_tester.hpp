#ifndef BANK_TESTER_HPP
#define BANK_TESTER_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/21/2020
Lab 1: 
    
*/

using namespace std;

#include <stdio.h>
#include <string>
#include <vector>
#include <list>

#include "bank.hpp"

struct TXN_t{
    string action; 
    int toID; 
    int fromID; 
    float amt; 
}; 


struct bank_thread_args {
    size_t tid; //Thread id
    std::vector<TXN_t> array;
}; 



int bank_tester(int num_threads,  Bank &myBank, std::vector <TXN_t> &data ); 

#endif