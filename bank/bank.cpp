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
#include <immintrin.h>

using namespace std;

//Developer includes
#include "bank.hpp"

#define NUM_RETRIES 1

// /*************************************************
// 	CONSTRUCTOR and DESTRUCTOR
// **************************************************/
Bank::Bank(int txn_method, std::vector <float> &startingBalances){
    TXN_METHOD = txn_method; 
    NUM_ACCOUNTS = startingBalances.size(); 
    total = 0; 


    //Create accounts and assign id's and balances
    accounts = new Account_t[NUM_ACCOUNTS]; 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        accounts[i].id = i; 
        accounts[i].bal = startingBalances[i]; 
        total += startingBalances[i]; 
    }
     
    sg_lock = PTHREAD_MUTEX_INITIALIZER; 
    switch(this->TXN_METHOD){
		case SGL: 
			// sg_lock = PTHREAD_MUTEX_INITIALIZER; 
			break;
		case PHASE_2: 
			account_locks = new pthread_mutex_t[NUM_ACCOUNTS]; 
			for(int i = 0; i < NUM_ACCOUNTS; i++){
				account_locks[i] = PTHREAD_MUTEX_INITIALIZER; 
			}
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

// /*************************************************
// 	BANK FUNCTIONS
// **************************************************/

void __attribute__((transaction_safe)) Bank::account_deposit(int id, float amt){
    this->accounts[id].bal += amt; 
    this->total +=amt;
}

int __attribute__((transaction_safe)) Bank::account_withdraw(int id, float amt){
    if(this->accounts[id].bal < amt){
        return -1;
    }
    this->accounts[id].bal -= amt; 
    this->total -=amt; 
    return 1; 
}

void Bank::deposit(int id, float amt){
    if(amt < 0 || id < 0 || id >= NUM_ACCOUNTS){
            return; 
    }

    switch(TXN_METHOD){
		case SGL: 
            pthread_mutex_lock(&sg_lock);
            this->accounts[id].bal += amt; 
            this->total +=amt;    
            pthread_mutex_unlock(&sg_lock);
			break;
		case PHASE_2: 
			pthread_mutex_lock(&account_locks[id]);
            this->accounts[id].bal += amt; 
            this->total +=amt;
            pthread_mutex_unlock(&account_locks[id]);
			break; 
		case STM: 
			__transaction_atomic{
            this->accounts[id].bal += amt; 
            this->total +=amt;
            }
			break; 
		case HTM_SGL: 
			

			break;
		case HTM_OPTIMIST: 
			
			break;
		default: 
			
			break;
    }
    
}

void Bank::withdraw(int id, float amt){
    if(amt < 0 || id < 0 || id >= NUM_ACCOUNTS){
            return; 
    }
    switch(TXN_METHOD){
		case SGL: 
            pthread_mutex_lock(&sg_lock);
            if(this->accounts[id].bal >= amt){
                this->accounts[id].bal -= amt; 
                this->total -=amt;
            }
                
            pthread_mutex_unlock(&sg_lock);
			break;
		case PHASE_2: 
			pthread_mutex_lock(&account_locks[id]);
            if(this->accounts[id].bal >= amt){
                this->accounts[id].bal -= amt; 
                this->total -=amt;
            }
            pthread_mutex_unlock(&account_locks[id]);
			break; 
		case STM: 
			__transaction_atomic{
                if(this->accounts[id].bal >= amt){
                    this->accounts[id].bal -= amt; 
                    this->total -=amt;
                }
            }
			break; 
		case HTM_SGL: 
			
			break;
		case HTM_OPTIMIST: 
			
			break;
		default: 
			
			break;
    }
    
}


void Bank::transfer(int fromId, int toId, float amt){
    //Check for invalid amounts and account ids
    if(amt < 0 || toId < 0 || toId >= NUM_ACCOUNTS || fromId < 0 || fromId >= NUM_ACCOUNTS){
        return; 
    }
    int suc = 0; 
    switch(TXN_METHOD){
		case SGL: 
            pthread_mutex_lock(&sg_lock);
            if(account_withdraw(fromId,amt)==1){
                account_deposit(toId,amt); 
            } 
                
            pthread_mutex_unlock(&sg_lock);
			break;
		case PHASE_2: 
			pthread_mutex_lock(&account_locks[fromId]);
            pthread_mutex_lock(&account_locks[toId]);
            if(account_withdraw(fromId,amt)==1){
                account_deposit(toId,amt); 
            } 
            pthread_mutex_unlock(&account_locks[fromId]);
            pthread_mutex_unlock(&account_locks[toId]);
			break; 
		case STM: 
			__transaction_atomic{
                if(account_withdraw(fromId,amt)==1){
                    account_deposit(toId,amt); 
                } 
            }
			break; 
		case HTM_SGL: 
			//Allow for NUM_RETRIES
            for(int i = 0; !suc && i < NUM_RETRIES; i++){
                if(_xbegin() == _XBEGIN_STARTED){
                    if(account_withdraw(fromId,amt)==1){
                        account_deposit(toId,amt); 
                    } 
                    _xend(); 
                    suc = 1;  
                }
            }
            //Check to see if the transaction still failed
            if(!suc){
                //Execute SGL as fall back
                printf("\nTXN failed after %d retries",NUM_RETRIES); 
                pthread_mutex_lock(&sg_lock);
                if(account_withdraw(fromId,amt)==1){
                    account_deposit(toId,amt); 
                } 
                pthread_mutex_unlock(&sg_lock);

            }

			break;
		case HTM_OPTIMIST: 
			
			break;
		default: 
			
			break;
    }
    
}


float Bank::computeTotal(){
    float comp_total = 0; 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        comp_total += accounts[i].bal; 
    }
    return comp_total; 
}

void Bank::printBank(){

    pthread_mutex_lock(&sg_lock);
           
    printf("\n-------------- Bank -------------"); 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        printf("\n\tAccount[%d]: $%.2f", accounts[i].id, accounts[i].bal); 
    }
    printf("\n\n\tBank Total: $%.2f", total); 
    printf("\n"); 
           
    pthread_mutex_unlock(&sg_lock);
}













