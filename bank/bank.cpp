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
#include <vector>
#include <pthread.h>

using namespace std;

//Developer includes
#include "bank.hpp"

/*************************************************
	Global Variables
**************************************************/
//Banking variables
// int NUM_ACCOUNTS_b; 
// Account_t * accounts_b;
// float total_b; 
Bank_t bank_b; 
//Concurrency Variables


/*************************************************
	FUNCTIONS
**************************************************/


void initBank(int txn_method, std::vector <float> &startingBalances){
    bank_b.NUM_ACCOUNTS_b = startingBalances.size(); 
    bank_b.TXN_METHOD_b = txn_method; 

    //Create accounts and assign id's and balances
    bank_b.accounts_b = new Account_t[bank_b.NUM_ACCOUNTS_b]; 
    for(int i = 0; i < bank_b.NUM_ACCOUNTS_b; i++){
        bank_b.accounts_b[i].id = i; 
        bank_b.accounts_b[i].bal = startingBalances[i]; 
        bank_b.total_b += startingBalances[i]; 
    }
     

    switch(bank_b.TXN_METHOD_b){
		case SGL: 
			bank_b.sg_lock = PTHREAD_MUTEX_INITIALIZER; 
			break;
		case PHASE_2: 
			
			break; 
		case STM: 
			
			break; 
		case HTM_SGL: 
			
			break;
		case HTM_OPTIMIST: 
			
			break;
		default: 
			
			break;
    }

}

void printBank_b(){
    printf("\n-------------- Bank -------------"); 
    for(int i = 0; i < bank_b.NUM_ACCOUNTS_b; i++){
        printf("\n\tAccount[%d]: $%.2f", bank_b.accounts_b[i].id, bank_b.accounts_b[i].bal); 
    }
    printf("\n\n\tBank Total: $%.2f", bank_b.total_b); 
    printf("\n"); 
}

void __attribute__((transaction_safe)) account_deposit(int id, float amt){
    bank_b.accounts_b[id].bal += amt; 
    bank_b.total_b +=amt;
}

int __attribute__((transaction_safe)) account_withdraw(int id, float amt){
    if(bank_b.accounts_b[id].bal < amt){
        return -1;
    }
    bank_b.accounts_b[id].bal -= amt; 
    bank_b.total_b -=amt; 
    return 1; 
}

void transfer_b(int fromId, int toId, float amt){
    if(amt < 0 || toId < 0 || toId >= bank_b.NUM_ACCOUNTS_b || fromId < 0 || fromId >= bank_b.NUM_ACCOUNTS_b){
        return; 
    }
    __transaction_atomic{
        if(account_withdraw(fromId,amt)==-1){
            return; 
        } 
        account_deposit(toId,amt); 
    }
}

void delBank(){
     switch(bank_b.TXN_METHOD_b){
		case SGL: 
			break;
		case PHASE_2: 
			
			break; 
		case STM: 
			
			break; 
		case HTM_SGL: 
			
			break;
		case HTM_OPTIMIST: 
			
			break;
		default: 
			
			break;
    }
}
