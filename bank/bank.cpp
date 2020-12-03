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
#include "../account/bank_account.hpp"

/*************************************************
	FUNCTIONS
**************************************************/


/*************************************************
	CLASS FUNCTIONS
**************************************************/

/*
    Constructor
*/

Bank::Bank(int num_acnts){
    //Update imporant fields
    this->NUM_ACCOUNTS = num_acnts; 
    this->total = 0; 
    this->transfer_method = SGL; 
    //Set up accounts 
    this->accounts = new Account[num_acnts]; 
    for(int i = 1; i < NUM_ACCOUNTS; i++){
        accounts[i].id = i;
    }
    

}

Bank::Bank(int num_acnts, int transfer_method){
    //Update imporant fields
    this->NUM_ACCOUNTS = num_acnts; 
    this->total = 0; 
    this->transfer_method = transfer_method; 
    //Set up accounts 
    this->accounts = new Account[num_acnts]; 
    for(int i = 1; i < NUM_ACCOUNTS; i++){
        accounts[i].id = i;
    }
}

void Bank::initAccounts(std::vector <float> &startingBalances){
    int size = startingBalances.size();
    if(size > this->NUM_ACCOUNTS){
        size = this->NUM_ACCOUNTS; 
    }
    for(int i = 0; i < size; i++){
        //Update balance of account i
        accounts[i].updateBalance(startingBalances[i]); 
        //Update running total 
        this->total += startingBalances[i]; 
    }
}


// int Bank::deposit(int accountID, float amount){
//     //Account must exist within the system
//     if(accountID>= NUM_ACCOUNTS || amount < 0){
//         return -1; 
//     }else{
//         // //Transfer money 
//         switch(this->transfer_method){
            // case SGL: 
            //     printf("\nDepositing $%.2f account[%d] using SGL",amount, accountID); 
            // break;
            // case PHASE_2: 
            //     printf("\nDepositing $%.2f account[%d] using Two Phase Locking",amount, accountID); 
            //     break; 
            // case STM: 
            //     printf("\nDepositing $%.2f account[%d] using SMT",amount, accountID); 
            //     break; 
            // case HTM_SGL: 
            //     printf("\nDepositing $%.2f account[%d] using HMT with SGL",amount, accountID); 
            //     break; 
            // case HTM_OPTIMIST: 
            //     printf("\nDepositing $%.2f account[%d] using HMT with Optimistic",amount, accountID); 
            //     break; 
//         }
//         return 1; 
//     }   
// }

// int Bank::withdraw(int accountID, float amount){
//     //Account must exist within the system
//     if(accountID>= NUM_ACCOUNTS){
//         return -1; 
//     }else{
//         //Transfer money 
//         switch(this->transfer_method){
//             case SGL: 
//                 printf("\nWithdrawing $%.2f account[%d] using SGL",amount, accountID); 
//             break;
//             case PHASE_2: 
//                 printf("\nWithdrawing $%.2f account[%d] using Two Phase Locking",amount, accountID); 
//                 break; 
//             case STM: 
//                 printf("\nWithdrawing $%.2f account[%d] using SMT",amount, accountID); 
//                 break; 
//             case HTM_SGL: 
//                 printf("\nWithdrawing $%.2f account[%d] using HMT with SGL",amount, accountID); 
//                 break; 
//             case HTM_OPTIMIST: 
//                 printf("\nWithdrawing $%.2f account[%d] using HMT with Optimistic",amount, accountID); 
//                 break;
//         }
//         return 1; 
//     }   
// }


// int Bank::transfer(int fromAccountID, int toAccountID, float amount){
//     if(fromAccountID >= NUM_ACCOUNTS || toAccountID >= NUM_ACCOUNTS || amount < 0){
//         return -1; 
//     }else{
         
//         switch(this->transfer_method){
//             case SGL: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using SGL",amount, fromAccountID, toAccountID); 
//             break;
//             case PHASE_2: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using Two Phase Locking",amount, fromAccountID, toAccountID); 
//                 break; 
//             case STM: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using SMT",amount, fromAccountID, toAccountID); 
//                 break; 
//             case HTM_SGL: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using HMT with SGL",amount, fromAccountID, toAccountID); 
//                 break; 
//             case HTM_OPTIMIST: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using HMT with Optimistic",amount, fromAccountID, toAccountID); 
//                 break; 
//             default: 
//                 printf("\nTransfering $%.2f from account[%d] to account[%d] using Single Process",amount, fromAccountID, toAccountID); 
//                 // //Widthraw amount from the fromAccount
//                 // accounts[fromAccount].widthdraw(amount); 
//                 // //Add amount from the fromAccount
//                 // accounts[fromAccount].addFunds(amount);
//                 return 1;  
//                 break; 
//         }
//         return 1; 
//     }   
// }



void Bank::printBank(){
    printf("\n-------------- Bank -------------"); 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        printf("\n\tAccount[%d]: $%.2f", accounts[i].getId(), accounts[i].getBalance()); 
    }
    printf("\n\n\tBank Total: $%.2f", this->total); 
    printf("\n"); 
}