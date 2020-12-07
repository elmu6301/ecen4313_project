#ifndef BANK_HPP
#define BANK_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 10/17/2020
Lab 2: 
    
*/


#include <stdio.h>
#include <string>
#include <vector>

//Developer includes
#include "ticket_lock.hpp"

using namespace std;

enum TXN_IMP{
    SGL, 
    PHASE_2,
    STM, 
    HTM_SGL,
    OPTIMIST
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
        // Bank() = delete; 
        Bank(int txn_method, std::vector <float> &startingBalances); 
        ~Bank(); 
        Bank(const Bank&); 

        void transfer(int fromId, int toId, float amt); 
        void deposit(int id, float amt); 
        void withdraw(int id, float amt); 
        float computeTotal(); 
        float getTotal(); 
        float getAccountBalance(int id); 
        int getNumAccounts(); 
        void printBank(); 
        
}; 

#endif