#ifndef BANK_HPP
#define BANK_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 10/17/2020
Lab 2: 
    
*/
using namespace std;

#include <stdio.h>
#include <string>
#include <atomic>
#include <vector>
#include "../account/bank_account.hpp"


enum counter_imp{
    SGL, 
    PHASE_2,
    STM, 
    HTM_SGL,
    HTM_OPTIMIST
}; 


class Bank{
    private: 
    int NUM_ACCOUNTS; 
    Account * accounts; 
    float total; 
    int transfer_method; 

    public: 
    //Constructor
    Bank(int num_acnts);
    Bank(int num_acnts, int transfer_method);
    
    void initAccounts(std::vector <float> &startingBalances); 

    void printBank(); 
    // int deposit(int accountID, float amount); 
    // int withdraw(int accountID, float amount); 
    // int transfer(int fromAccountID, int toAccountID, float amount); 

}; 

#endif