#ifndef BANK_ACCOUNT_HPP
#define BANK_ACCOUNT_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 10/17/2020
Lab 2: 
    
*/
#include <stdio.h>
#include <atomic>

class Account{
    private: 
    float balance;     

    public: 
    int id; 
    
    //Constructor
    Account();
    Account(int id);
    Account(int id, float starting_balance); 

    //Getters
    // int getId(); 
    float getBalance(); 

    //Setters
    int getId(); 
    int updateBalance(float amount); 
    int __attribute__((transaction_safe))widthdraw(float amount); 
    int __attribute__((transaction_safe))deposit(float amount); 

    //Print
    void printAccount(); 
}; 

#endif