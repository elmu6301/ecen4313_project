/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/30/2020
Lab 1: 

*Note modified from test.c provide
    
*/
/*************************************************
	FILE INCLUDES
**************************************************/
//Library includes
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <cmath>
#include <vector>
#include <string>

//Developer includes
#include "bank_tester.hpp"
#include "../account/bank_account.hpp"
#include "../bank/bank.hpp"


/*************************************************
	GLOBAL VARIABLES
**************************************************/
pthread_t *threads;
pthread_mutex_t sgl_lock; 
pthread_mutex_t * p2_locks; 

//For bank
int TXN_METHOD; 
int NUM_ACCOUNTS; 
float total; 
Account * bank; 
// 

size_t NUM_THREADS;
pthread_barrier_t p_bar;

struct timespec tstart;
struct timespec tend;


/*************************************************
	FUNCTION DECLARATIONS
**************************************************/
//Helper functions
std::vector<std::vector<TXN_t>> split_vector_array(std::vector<TXN_t>array,int num_parts); 
void printTXNS(std::vector <TXN_t> &txnData); 
void printBank(); 
//Bank functions
int deposit(int accountID, float amount); 
void withdraw(int accountID, float amount); 
void transfer(int fromAccountID, int toAccountID, float amount); 
/*************************************************
	GLOBAL INIT AND CLEANING FUNCTIONS
**************************************************/
/*
	Allocates all required data. 
*/
void global_init(int txn_method, std::vector <float> &startingBalances)
{
	//Setup accounts
	NUM_ACCOUNTS = startingBalances.size(); 
	TXN_METHOD = txn_method; 
	bank = new Account[NUM_ACCOUNTS]; 
	for(int i = 0; i < NUM_ACCOUNTS; i++){
        // accounts[i].id = i;
		bank[i] = Account(i,startingBalances[i]); 
		total += startingBalances[i]; 
    }
	printBank(); 
	//pthread 
	threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t));
	pthread_barrier_init(&p_bar, NULL, NUM_THREADS);

	switch(TXN_METHOD){
		case SGL: 
			sgl_lock = PTHREAD_MUTEX_INITIALIZER;
			printf("Running bank with SGL"); 
			break;
		case PHASE_2: 
			printf("Running bank with Two Phase Locking"); 
			p2_locks = new pthread_mutex_t[NUM_ACCOUNTS]; 
			for(int i = 0; i < NUM_ACCOUNTS; i++){
				p2_locks[i] = PTHREAD_MUTEX_INITIALIZER; 
			}
			break; 
		case STM: 
			printf("Running bank with STM"); 
			break; 
		case HTM_SGL: 
			printf("Running bank with HTM and SGL"); 
			break;
		case HTM_OPTIMIST: 
			printf("Running bank with HTM and Optimist"); 
			break;
		default: 
			printf("Running bank with DEFAULT"); 
			break;
        }
}

/*
	Frees all allocated data. 
*/
void global_cleanup()
{
	free(threads);
	delete bank; 
	pthread_barrier_destroy(&p_bar);
	switch(TXN_METHOD){
		case SGL: 
			printf("\nCleaning bank with SGL"); 
			break;
		case PHASE_2: 
			printf("\nCleaning bank with Two Phase Locking"); 
			delete p2_locks;
			break; 
		case STM: 
			printf("\nCleaning bank with STM"); 
			break; 
		case HTM_SGL: 
			printf("\nCleaning bank with HTM and SGL"); 
			break;
		case HTM_OPTIMIST: 
			printf("\nCleaning bank with HTM and Optimist"); 
			break;
		default: 
			printf("\nCleaning bank with DEFAULT"); 
			break;
    }

}

/*************************************************
	HELPER FUNCTIONS
**************************************************/
/*
	Splits a vector array into num_parts. 
*/
std::vector<std::vector<TXN_t>> split_vector_array(std::vector<TXN_t>array,int num_parts){ 
	
	//Create and allocate a vector of vectors
	std::vector<std::vector<TXN_t>> arrays; 
	arrays.resize(num_parts); 

	//Calculate the best size for each sub array
	float f_size = array.size()/(float)num_parts; 
	int size = array.size()/num_parts; 
	if(f_size - size >= 0.5 && (size+1)*(num_parts-1) < array.size()){//round up
		size++; 
	}
	//Split array into n-1 size chuncks
	int index = 0; 
	int array_index; 
	//Add size elements into the array for each array_index
	for(array_index = 0; array_index < num_parts-1; array_index++){
		//Generate endpoints
		auto start = std::next(array.begin(), array_index * size); 
		auto end = std::next(array.begin(), array_index*size + size); 
		//Allocate mem
		arrays[array_index].resize(size); 
		//Copy		
		std::copy(start, end, arrays[array_index].begin()); 

	}
	//Add the last array
	std::vector<TXN_t>::iterator start = std::next(array.begin(), array_index * size); 
	 
	std::vector<TXN_t>::iterator array_end = array.end(); 
	int rem = array.size() - array_index * size;
	arrays[array_index].resize(rem); 
	//Copy
	std::copy(start, array_end, arrays[array_index].begin()); 
	arrays[array_index].resize(rem); 
	return arrays; 
}

void printTXNS(std::vector <TXN_t> &txnData){
	for(int i = 0; i < txnData.size();  i++){
        printf("\nTXN[%d]: action = %s, toID = %d, fromID = %d, amt = %.2f", i, txnData[i].action.c_str(), txnData[i].toID, txnData[i].fromID, txnData[i].amt);  
    }
}

void printBank(){
	printf("\n-------------- Bank -------------"); 
    for(int i = 0; i < NUM_ACCOUNTS; i++){
        printf("\n\tAccount[%d]: $%.2f", bank[i].getId(),bank[i].getBalance()); 
    }
    printf("\n\n\tBank Total: $%.2f", total); 
    printf("\n"); 
}

/*************************************************
	BANK FUNCTIONS
**************************************************/
int deposit(int accountID, float amount){
	// printf("\nAccount id = %d amt = $%.2f", accountID, amount); 
	switch(TXN_METHOD){
		case SGL: 
			printf("Depositing $%.2f to account[%d] using SGL\n",amount, accountID); 
			pthread_mutex_lock(&sgl_lock);
			
			if(accountID < NUM_ACCOUNTS){
				bank[accountID].deposit(amount); 
				total += amount; 
			}
			printf(" : Account[%d] = $%.2f\n",accountID, bank[accountID].getBalance()); 
			pthread_mutex_unlock(&sgl_lock);
			
			break;
		case PHASE_2: 
			printf("Depositing $%.2f to account[%d] using Two Phase Locking\n",amount, accountID); 
			//Phase 1: aquire all needed locks
			pthread_mutex_lock(&p2_locks[accountID]);

			//Make changes
			bank[accountID].deposit(amount); 
			total += amount; 

			//Phase 2: release all needed locks
			pthread_mutex_unlock(&p2_locks[accountID]);

			break; 
		case STM: 
			printf("Depositing $%.2f to account[%d] using SMT\n",amount, accountID); 
			break; 
		case HTM_SGL: 
			printf("Depositing $%.2f to account[%d] using HMT with SGL\n",amount, accountID); 
			break; 
		case HTM_OPTIMIST: 
			printf("Depositing $%.2f to account[%d] using HMT with Optimistic\n",amount, accountID); 
			break; 
	}
	return 1; 
}

void __attribute__((transaction_safe)) test(int &val){
	 val+=1; 
}

void withdraw(int accountID, float amount){
        //Transfer money 
	int temp1 = 0; 

	switch(TXN_METHOD){
		case SGL: 
			printf("Withdrawing $%.2f from account[%d] using SGL",amount, accountID); 
			pthread_mutex_lock(&sgl_lock);
			if(accountID < NUM_ACCOUNTS){
				bank[accountID].widthdraw(amount); 
				total -= amount; 
				// bank[accountID].printAccount();
				printf(" : Account[%d] = $%.2f\n",accountID, bank[accountID].getBalance()); 
			}
			pthread_mutex_unlock(&sgl_lock);
			break;
		case PHASE_2: 
			printf("Withdrawing $%.2f from account[%d] using Two Phase Locking\n",amount, accountID); 
			//Phase 1: aquire all needed locks
			pthread_mutex_lock(&p2_locks[accountID]);

			//Make changes
			bank[accountID].widthdraw(amount); 
			total -= amount; 

			//Phase 2: release all needed locks
			pthread_mutex_unlock(&p2_locks[accountID]);
			break; 
		case STM: 
			printf("Withdrawing $%.2f from account[%d] using SMT\n",amount, accountID); 
			
			__transaction_atomic{
				test(temp1);  
			}
			break; 
		case HTM_SGL: 
			printf("Withdrawing $%.2f from account[%d] using HMT with SGL\n",amount, accountID); 
			break; 
		case HTM_OPTIMIST: 
			printf("Withdrawing $%.2f from account[%d] using HMT with Optimistic\n",amount, accountID); 
			break;
	}
	printf("temp = %d",temp1); 
}



void transfer(int fromAccountID, int toAccountID, float amount){
        //Transfer money 
	switch(TXN_METHOD){
		case SGL: 
			printf("Transfering $%.2f from account[%d] to account[%d] using SGL\n",amount, fromAccountID, toAccountID); 
			pthread_mutex_lock(&sgl_lock);
			bank[fromAccountID].widthdraw(amount);  
			bank[toAccountID].deposit(amount); 
			pthread_mutex_unlock(&sgl_lock);
			break;
		case PHASE_2: 
			printf("Transfering $%.2f from account[%d] to account[%d] using Two Phase Locking\n",amount, fromAccountID, toAccountID); 
			//Phase 1: aquire all needed locks
			pthread_mutex_lock(&p2_locks[fromAccountID]);
			pthread_mutex_lock(&p2_locks[toAccountID]);
			//Make changes
			bank[fromAccountID].widthdraw(amount);  
			bank[toAccountID].deposit(amount); 

			//Phase 2: release all needed locks
			pthread_mutex_unlock(&p2_locks[toAccountID]);
			pthread_mutex_unlock(&p2_locks[fromAccountID]);
			
			
			break; 
		case STM: 
			printf("Transfering $%.2f from account[%d] to account[%d] using SMT\n",amount, fromAccountID, toAccountID); 
			break; 
		case HTM_SGL: 
			printf("Transfering $%.2f from account[%d] to account[%d] using HMT with SGL\n",amount, fromAccountID, toAccountID); 
			break; 
		case HTM_OPTIMIST: 
			printf("Transfering $%.2f from account[%d] to account[%d] using HMT with Optimistic\n",amount, fromAccountID, toAccountID); 
			break; 
	}
}


/*************************************************
	THREAD FUNCTIONS
**************************************************/
/*
	Thread version of bucketsort. Runs bucketsort with the assigned data. 
*/
void *bank_thread(void *args)
{
	size_t tid = ((struct bank_thread_args *)args)->tid; //*((size_t*)args);
	std::vector<TXN_t> array = ((struct bank_thread_args *)args)->array;

	pthread_barrier_wait(&p_bar);
	if (tid == 1)
	{
		clock_gettime(CLOCK_MONOTONIC, &tstart);
	}
	pthread_barrier_wait(&p_bar);

	
	//Process each transaction
	for(int i = 0; i < array.size(); i++){
		printf("\nThread[%zu]: ",tid); 
		//Call the appropriate function based off of the action field
		if(array[i].action.compare("deposit")== 0){
			// printf("\nDepositing %.2f in account[%d]", array[i].amt, array[i].toID); 
			deposit(array[i].toID, array[i].amt); 
		}
		else if(array[i].action.compare("withdraw")==0){
			// printf("\nDepositing %.2f in account[%d]", array[i].amt, array[i].toID);
			withdraw(array[i].toID, array[i].amt);  
		}else if(array[i].action.compare("transfer")==0){
			// printf("\nTransfering %.2f from account[%d] to account[%d]", array[i].amt, array[i].fromID,array[i].toID); 
			transfer(array[i].fromID, array[i].toID, array[i].amt); 
		}
	}

	pthread_barrier_wait(&p_bar);
	if (tid == 1)
	{
		clock_gettime(CLOCK_MONOTONIC, &tend);
	}

	return 0;
}

/*
	Runs the parallelized bucketsort. Creates and joins all threads and returns
	the data to main. 
*/
int bank_tester(int num_threads, int txn_method, std::vector <float> &startingBalances, std::vector <TXN_t> &data)
{

	NUM_THREADS = num_threads;
	if (NUM_THREADS > 150) 
	{
		printf("ERROR; too many threads\n");
		exit(-1);
	}
	// NUM_THREADS = 1; 

	//Global init
	global_init(txn_method,startingBalances);

	//Split array into NUM_THREADS parts
	std::vector<std::vector<TXN_t>> split_arrays = split_vector_array(data, NUM_THREADS);

	
	struct bank_thread_args args[NUM_THREADS];
	
	//Setting up the master thread args data
	args[0].tid = 1; //i;
	args[0].array = split_arrays[0];


	// launch threads
	int ret;
	size_t i;

	for (i = 1; i < NUM_THREADS; i++)
	{
		args[i].tid = i + 1;
		args[i].array = split_arrays[i];
		// printf("creating thread %zu\n",args[i].tid);
		ret = pthread_create(&threads[i], NULL, &bank_thread, &args[i]);
		if (ret)
		{
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}

	bank_thread(&args); // master also calls mergesort_thread

	// join threads
	for (size_t i = 1; i < NUM_THREADS; i++)
	{
		ret = pthread_join(threads[i], NULL);
		if (ret)
		{
			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);
		}
		// printf("joined thread %zu\n",i+1);
	}


	printBank();

	global_cleanup();
	
	unsigned long long elapsed_ns;
	elapsed_ns = (tend.tv_sec - tstart.tv_sec) * 1000000000 + (tend.tv_nsec - tstart.tv_nsec);
	printf("\nElapsed (ns): %llu\n", elapsed_ns);
	double elapsed_s = ((double)elapsed_ns) / 1000000000.0;
	printf("Elapsed (s): %lf\n", elapsed_s);
	return 0; 
}