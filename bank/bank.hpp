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


enum TXN_IMP{
    SGL, 
    PHASE_2,
    STM, 
    HTM_SGL,
    HTM_OPTIMIST
}; 

struct Account_t{
    int id; 
    float bal; 
}; 

struct Bank_t{
    int NUM_ACCOUNTS_b; 
    Account_t * accounts_b;
    float total_b; 
    int TXN_METHOD_b; 
    pthread_mutex_t sg_lock; 
    pthread_mutex_t * account_locks; 
}; 


void initBank(int txn_method, std::vector <float> &startingBalances); 
void __attribute__((transaction_safe))account_deposit(int id, float amt); 
int __attribute__((transaction_safe))account_withdraw(int id, float amt); 
void transfer_b(int fromId, int toId, float amt); 
void printBank_b(); 
void delBank(); 

#endif