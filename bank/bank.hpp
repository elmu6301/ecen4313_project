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

//Developer includes
#include "../lock/ticket_lock.hpp"

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

class Bank{
    private:
        int NUM_ACCOUNTS; 
        Account_t * accounts;
        float total; 
        int TXN_METHOD; 
        TicketLock sg_lock; 
        TicketLock * account_locks; 
    
        void __attribute__((transaction_safe))account_deposit(int id, float amt); 
        int __attribute__((transaction_safe))account_withdraw(int id, float amt); 

    public:
        Bank(int txn_method, std::vector <float> &startingBalances); 
        ~Bank(); 
        void transfer(int fromId, int toId, float amt); 
        void deposit(int id, float amt); 
        void withdraw(int id, float amt); 
        float computeTotal(); 
        void printBank(); 
}; 

#endif