#ifndef TTAS_LOCK_HPP
#define TTAS_LOCK_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 10/17/2020
Lab 2: 
    
*/
#include <stdio.h>
#include <atomic>

class TtasLock{
    private: 
    std::atomic<bool> flag;
     
    public: 
    TtasLock(); 
    void lock(); 
    bool isLockHeld(); 
    void unlock();  

}; 
 void test_ttas_lock(); 

#endif