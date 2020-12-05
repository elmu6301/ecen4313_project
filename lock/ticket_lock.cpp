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
#include "ticket_lock.hpp"

/*************************************************
	FUNCTIONS
**************************************************/
void test_ticket_lock(){
    printf("\nticket_lock\n"); 
}

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
    Aquires the lock
*/
void TicketLock::lock(){
    int my_num = atomic_fetch_add_explicit(&next_num, 1, memory_order_seq_cst); 
    while(atomic_load_explicit(&now_serving,memory_order_seq_cst) !=my_num); 
    lock_held.store(true); 
}


/*
    Releases the lock
*/
void TicketLock::unlock(){
    lock_held.store(false);
    atomic_fetch_add_explicit(&now_serving, 1, memory_order_seq_cst); 
}

bool TicketLock::lockHeld(){
    return lock_held.load(); 
}