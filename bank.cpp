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
#include <pthread.h>
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
    //Create locks 
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
    int suc = 0;
    thread_local float bal; 
    thread_local float newBal;  
    thread_local float tot; 
    thread_local float newTot; 
    switch(this->TXN_METHOD){
		case SGL: 
            sg_lock.lock(); 
            account_deposit(id,amt);  
            sg_lock.unlock(); 
			break;
		case PHASE_2: 
            account_locks[id].lock();
            account_deposit(id,amt); 
            account_locks[id].unlock(); 
			break; 
		case STM: 
			__transaction_atomic{
                account_deposit(id,amt); 
            }
			break; 
		case HTM_SGL: 
			//Allow for NUM_RETRIES
            for(int i = 0; !suc && i < NUM_RETRIES; i++){
                if(_xbegin() == _XBEGIN_STARTED){
                    //Check to see if the lock is held
                    if(sg_lock.lockHeld()){
                        _xabort(1); 
                    }
                    account_deposit(id,amt); 
                    
                    _xend(); 
                    suc = 1;  
                }
            }
            //Check to see if the transaction still failed
            if(!suc){
                //Execute SGL as fall back
                sg_lock.lock(); 
                account_deposit(id,amt); 
                sg_lock.unlock(); 
            }
			break;
		case OPTIMIST: 
    
            while(suc==0)
            {
                //Read set
                bal = accounts[id].bal; 
                tot = this->total; 
                //Write set
                newBal =  bal + amt;   
                newTot = tot + amt;           
                //Validate
                sg_lock.lock();   
                if(bal== accounts[id].bal && tot == this->total ){
                    //write 
                    suc = 1; 
                    accounts[id].bal = newBal; 
                    this->total = newTot; 
                }
                sg_lock.unlock(); 
            }

			break;
    }
    
}

void Bank::withdraw(int id, float amt){
    if(amt < 0 || id < 0 || id >= NUM_ACCOUNTS){
        return; 
    }
    int suc = 0;
    int err = 1; 
    thread_local float bal; 
    thread_local float newBal;  
    thread_local float tot; 
    thread_local float newTot; 
    switch(TXN_METHOD){
		case SGL: 
            sg_lock.lock(); 
            err = account_withdraw(id,amt); 
            sg_lock.unlock(); 
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f",id, amt); 
            }
			break;
		case PHASE_2: 
			account_locks[id].lock(); 
            err = account_withdraw(id,amt); 
            account_locks[id].unlock(); 
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f",id, amt); 
            }
			break; 
		case STM: 
			__transaction_atomic{
                err = account_withdraw(id,amt); 
            }
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f",id, amt); 
            }
			break; 
		case HTM_SGL: 
			//Allow for NUM_RETRIES 
            for(int i = 0; !suc && i < NUM_RETRIES; i++){
                if(_xbegin() == _XBEGIN_STARTED){
                    //Check to see if the lock is held
                    if(sg_lock.lockHeld()){
                        _xabort(1); 
                    }
                    err = account_withdraw(id,amt); 
                    
                    _xend(); 
                    suc = 1;  
                }
            }
            //Check to see if the transaction still failed
            if(!suc){
                //Execute SGL as fall back
                sg_lock.lock(); 
                err =account_withdraw(id,amt);
                sg_lock.unlock(); 
            }
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f",id, amt);  
            }
			break;
		case OPTIMIST: 
			
            while(suc==0)
            {
                //Read set
                bal = accounts[id].bal; 
                tot = this->total; 
                //Write set
                newBal =  bal - amt;   
                newTot = tot - amt;           
                //Validate
                sg_lock.lock();   
                if(bal== accounts[id].bal && tot == this->total ){
                    //write 
                    suc = 1; 
                    accounts[id].bal = newBal; 
                    this->total = newTot; 
                }
                sg_lock.unlock(); 
            }
			break;
    }
    
}


void Bank::transfer(int fromId, int toId, float amt){
    //Check for invalid amounts and account ids
    if(amt < 0 || toId < 0 || toId >= NUM_ACCOUNTS || fromId < 0 || fromId >= NUM_ACCOUNTS || fromId == toId){
        return; 
    }
    int err =0; 
    int suc = 0; 
    thread_local float balFrom; 
    thread_local float balTo;
    thread_local float tot; 
    thread_local float newBalFrom; 
    thread_local float newBalTo;  

    switch(TXN_METHOD){
		case SGL: 
            sg_lock.lock(); 
            if((err=account_withdraw(fromId,amt))==1){
                account_deposit(toId,amt); 
            } 
            sg_lock.unlock(); 
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f", fromId, amt);  
            }

			break;
		case PHASE_2: 
            bool fromHeld, toHeld; 
            while((fromHeld = account_locks[fromId].tryLock()) == false  || (toHeld = account_locks[toId].tryLock()) == false){
                //Only hold 1 lock so release it
                if(fromHeld){
                    account_locks[fromId].unlock(); 
                }else if(toHeld){
                    account_locks[toId].unlock(); 
                }
            }
            if((err=account_withdraw(fromId,amt))==1){
                account_deposit(toId,amt); 
            } 
            account_locks[toId].unlock(); 
            account_locks[fromId].unlock(); 
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f", fromId, amt); 
            }
			break; 
		case STM: 
			__transaction_atomic{
                if((err=account_withdraw(fromId,amt))==1){
                    account_deposit(toId,amt); 
                } 
            };
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f", fromId, amt);   
            }
			break; 
		case HTM_SGL: 
			//Allow for NUM_RETRIES
            for(int i = 0; !suc && i < NUM_RETRIES; i++){
                if(_xbegin() == _XBEGIN_STARTED){
                    //Check to see if the lock is held
                    if(sg_lock.lockHeld()){
                        _xabort(1); 
                    }
                    if((err=account_withdraw(fromId,amt))==1){
                        account_deposit(toId,amt); 
                    } 
                    _xend(); 
                    suc = 1;  
                }
            }
            
            //Check to see if the transaction still failed
            if(!suc){
                //Execute SGL as fall back
                sg_lock.lock(); 
                    if((err=account_withdraw(fromId,amt))==1){
                        account_deposit(toId,amt); 
                    } 
                sg_lock.unlock(); 
            }
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f", fromId, amt);  
            }
			break;
		case OPTIMIST:
            while (suc==0)
            {
                //Read set
                balFrom = accounts[fromId].bal; 
                balTo = accounts[toId].bal; 
                tot = this->total; 
                //Write set
                newBalFrom =  balFrom - amt; 
                newBalTo = balTo + amt; 
            
                //Validate
                sg_lock.lock();  
                if(balFrom == accounts[fromId].bal && balTo == accounts[toId].bal && tot == this->total){
                    //write 
                    suc = 1; 
                    if(accounts[fromId].bal >= amt){
                        accounts[fromId].bal = newBalFrom; 
                        accounts[toId].bal = newBalTo;
                    }else{
                        err = -1; 
                    }
                  
                }
                sg_lock.unlock(); 
            }
            if(err==-1){
                printf("\nOverdraw error on on account[%d] and amt =  $%.2f", fromId, amt);  
            }
          
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

float Bank::getTotal(){
    return total; 
}

 float Bank::getAccountBalance(int id){
    if(0 <= id && id < NUM_ACCOUNTS){
        return accounts[id].bal; 
    }
    return -1;  
 }

 int Bank::getNumAccounts(){
    return NUM_ACCOUNTS; 
 }

void Bank::printBank(){
    sg_lock.lock();            
    printf("\n--------------- Bank --------------"); 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        printf("\n\tAccount[%d]: $%.2f", accounts[i].id, accounts[i].bal); 
    }
    printf("\n\n\tBank Total: $%.2f", total); 
    printf("\n-----------------------------------\n");  
    sg_lock.unlock();      
}













