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
#include <vector>
#include <immintrin.h>

//Developer includes

using namespace std;
#include "bank.hpp"
#include "ticket_lock.hpp"



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
    account_locks = new TicketLock[NUM_ACCOUNTS]; 
 }

Bank::~Bank(){
    delete accounts; 
    delete account_locks; 
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
            sg_lock.lock();
            this->accounts[id].bal += amt; 
            this->total +=amt;    
            sg_lock.unlock(); 
			break;
		case PHASE_2: 
            account_locks[id].lock();
            this->accounts[id].bal += amt; 
            this->total +=amt;
            account_locks[id].unlock(); 
			break; 
		case STM: 
			__transaction_atomic{
            this->accounts[id].bal += amt; 
            this->total +=amt;
            }
			break; 
		case HTM_SGL: 
			

			break;
		case OPTIMIST: 
			
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
            sg_lock.lock(); 
            if(this->accounts[id].bal >= amt){
                this->accounts[id].bal -= amt; 
                this->total -=amt;
            }
            sg_lock.unlock(); 
			break;
		case PHASE_2: 
			account_locks[id].lock(); 
            if(this->accounts[id].bal >= amt){
                this->accounts[id].bal -= amt; 
                this->total -=amt;
            }
            account_locks[id].unlock(); 
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
		case OPTIMIST: 
			
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
            sg_lock.lock(); 
            if(account_withdraw(fromId,amt)==1){
                account_deposit(toId,amt); 
            } 
            sg_lock.unlock(); 
			break;
		case PHASE_2: 
            account_locks[fromId].lock(); 
            account_locks[toId].lock(); 
            if(account_withdraw(fromId,amt)==1){
                account_deposit(toId,amt); 
            } 
            account_locks[fromId].unlock(); 
            account_locks[toId].unlock(); 
			break; 
		case STM: 
			__transaction_atomic{
                if(account_withdraw(fromId,amt)==1){
                    account_deposit(toId,amt); 
                } 
            };
			break; 
		case HTM_SGL: 
			//Allow for NUM_RETRIES
            for(int i = 0; !suc && i < NUM_RETRIES; i++){
                if(_xbegin() == _XBEGIN_STARTED){
                    //Check to see if the lock is held
                    if(sg_lock.lockHeld()){
                        _xabort(1); 
                    }

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
                sg_lock.lock(); 
                    if(account_withdraw(fromId,amt)==1){
                        account_deposit(toId,amt); 
                    } 
                sg_lock.unlock(); 
            }

			break;
		case OPTIMIST: 
			
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
    sg_lock.lock();            
    printf("\n-------------- Bank -------------"); 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        printf("\n\tAccount[%d]: $%.2f", accounts[i].id, accounts[i].bal); 
    }
    printf("\n\n\tBank Total: $%.2f", total); 
    printf("\n"); 
    sg_lock.unlock();      
}













