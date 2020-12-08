/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/30/2020
Final Project
    
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
#include "ticket_lock.hpp"

/*************************************************
	FUNCTIONS
**************************************************/

/*************************************************
	CLASS FUNCTIONS
**************************************************/
/*
    Constructor
*/
TicketLock::TicketLock(){
    next_num.store(0); 
    now_serving.store(0); 
    lock_held.store(false); 
}

/*
    Acquires the lock and spins if it is unable to. 
*/
void TicketLock::lock(){
    int my_num = atomic_fetch_add_explicit(&next_num, 1, memory_order_seq_cst); 
    while(atomic_load_explicit(&now_serving,memory_order_seq_cst) !=my_num); 
    lock_held.store(true); 
}
/*
    Tries to acquire the lock and returns if it acquire the lock. 
*/
bool TicketLock::tryLock(){
    int my_num = atomic_fetch_add_explicit(&next_num, 1, memory_order_seq_cst); 
    int curr_num = atomic_load_explicit(&now_serving,memory_order_seq_cst); 
    if(curr_num ==my_num){
        lock_held.store(true); 
        return true; 
    }
    return false;
}
/*
    Releases the lock. 
*/
void TicketLock::unlock(){
    lock_held.store(false);
    atomic_fetch_add_explicit(&now_serving, 1, memory_order_seq_cst); 
}

/*
    Checks to see if the lock is currently being held. 
*/
bool TicketLock::lockHeld(){
    return lock_held.load(); 
}