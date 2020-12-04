/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/30/2020
Lab 2: 
    
*/

/*************************************************
	FILE INCLUDES
**************************************************/
//Library includes
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>

using namespace std;

//Developer includes
#include "bank_account.hpp"

/*************************************************
	FUNCTIONS
**************************************************/


/*************************************************
	CLASS FUNCTIONS
**************************************************/

/*
    Constructor
*/
Account::Account(){
    this->id = 0; 
    this->balance = 0; 
}

Account::Account(int id){
    this->id = id; 
    this->balance = 0; 
}

Account::Account(int id, float starting_balance){
    this->id = id; 
    this->balance = starting_balance; 
}

int Account::getId(){
    return this->id; 
}

float Account::getBalance(){
    return this->balance; 
}

int Account::updateBalance(float amount){
    //Attempting to widthraw more than available
    if(amount < 0 && amount > this->balance){
        return -1; 
    }else{
        this->balance += amount; 
        return 1; 
    }
    
}

int __attribute__((transaction_safe)) Account::widthdraw(float amount){
    //Attempting to widthraw more than available
    if(amount > this->balance){
        return -1; 
    }else{
        this->balance -= amount; 
        return 1; 
    }
    
} 

int __attribute__((transaction_safe)) Account::deposit(float amount){
    //Attempting to widthraw more than available
    if(amount < 0){
        return -1; 
    }else{
        this->balance += amount; 
        return 1; 
    }
    
}

void Account::printAccount(){
    printf("\nAccount[%d] balance is $%.2f", id, balance); 
}