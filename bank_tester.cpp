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
#include <assert.h>

//Developer includes
#include "bank_tester.hpp"
#include "bank.hpp"


/*************************************************
	GLOBAL VARIABLES
**************************************************/
Bank * bank; 

pthread_t *threads;
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
void global_init()
{
	threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t));
	pthread_barrier_init(&p_bar, NULL, NUM_THREADS);
}

/*
	Frees all allocated data. 
*/
void global_cleanup()
{
	free(threads);
	pthread_barrier_destroy(&p_bar);
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

void analyzeThrougput(struct bank_thread_args *args, int num_threads){
	
	float avg_throughput_ns = 0; 
	float avg_throughput_s = 0; 
	float std_throughput_ns = 0; 
	float std_throughput_s = 0; 
	
	//Calculate agreagate throughputs for both ns and s
	for(int i = 0; i < num_threads; i++){
		avg_throughput_ns += args[i].throughput_ns; 
		avg_throughput_s += args[i].throughput_s; 
	}
	//Compute mean throughputs for both ns and s
	avg_throughput_ns /=(float)num_threads; 
	avg_throughput_s /=(float)num_threads; 

	//Calculate the standard deviation for both ns and s
	for(int i = 0; i < num_threads; i++){
		std_throughput_ns += pow(args[i].throughput_ns - avg_throughput_ns, 2); 
		std_throughput_s += pow(args[i].throughput_s - avg_throughput_s, 2); 
	}
	std_throughput_ns = sqrt(std_throughput_ns/(float)num_threads); 
	std_throughput_s = sqrt(std_throughput_s/(float)num_threads); 

	//Print out results 
	printf("\n-------------THROUGHPUT ANALYSIS-------------"); 
	printf("\nThroughput (txns/ns):"); 
	printf("\n\tAverage (txns/ns): %f", avg_throughput_ns); 
	printf("\n\tStandard Devation (txns/ns): %f", std_throughput_ns); 
	printf("\n\nThroughput (txns/s):"); 
	printf("\n\tAverage (txns/s): %f", avg_throughput_s); 
	printf("\n\tStandard Devation (txns/s): %f", std_throughput_s); 
	printf("\n---------------------------------------------\n"); 
	
}
/*************************************************
	THREAD FUNCTIONS
**************************************************/
/*
	Processes the given transactions in parallel with other threads.
*/
void *bank_thread(void *args)
{
	//Read in all useful arg information 
	size_t tid = ((struct bank_thread_args *)args)->tid; //*((size_t*)args);
	std::vector<TXN_t> array = ((struct bank_thread_args *)args)->array;

	//Create local thread variables
	thread_local struct timespec myStart, myEnd; 
	thread_local int num_txns = array.size(); 
	
	pthread_barrier_wait(&p_bar);
	//Compute local start time
	clock_gettime(CLOCK_MONOTONIC, &myStart);

	//Process each transaction
	for(int i = 0; i < num_txns; i++){
		//Call the appropriate function based off of the action field
		if(array[i].action.compare("deposit")== 0){
			// printf("\nThd[%zu] D: %.2f in account[%d]",tid, array[i].amt, array[i].toID); 
			(*bank).deposit(array[i].toID, array[i].amt); 
		}
		else if(array[i].action.compare("withdraw")==0){
			// printf("\nThd[%zu] W: %.2f in account[%d]", tid,array[i].amt, array[i].toID);
			(*bank).withdraw(array[i].fromID, array[i].amt);  
		}else if(array[i].action.compare("transfer")==0){
			// printf("\nThd[%zu] T: %.2f from account[%d] to account[%d]",tid, array[i].amt, array[i].fromID,array[i].toID); 
			(*bank).transfer(array[i].fromID, array[i].toID, array[i].amt); 
		}
	}

	//Compute local end time
	clock_gettime(CLOCK_MONOTONIC, &myEnd);
	pthread_barrier_wait(&p_bar);

	//Compute local runtime in nanoseconds and seconds
	((struct bank_thread_args *)args)->elapsed_ns = (myEnd.tv_sec - myStart.tv_sec) * 1000000000 + (myEnd.tv_nsec - myStart.tv_nsec);
	((struct bank_thread_args *)args)->elapsed_s =  (((struct bank_thread_args *)args)->elapsed_ns) / 1000000000.0;
	
	//Compute local throughput in txns/nanoseconds and txns/seconds
	((struct bank_thread_args *)args)->throughput_ns = ((float)num_txns)/(((struct bank_thread_args *)args)->elapsed_ns); 
	((struct bank_thread_args *)args)->throughput_s = ((float)num_txns)/(((struct bank_thread_args *)args)->elapsed_s); 
	
	//Print thread information
	// printf("\nThread[%ld] Elapsed (ns): %llu Throuput (txns/ns): %f", tid, ((struct bank_thread_args *)args)->elapsed_ns,((struct bank_thread_args *)args)->throughput_ns ); 
	// printf("\nThread[%ld] Elapsed (s): %lf\n Throuput (txns/s): %f", tid,((struct bank_thread_args *)args)->elapsed_s, ((struct bank_thread_args *)args)->throughput_s);


	return 0;
}

/*
	Runs the parallelized bucketsort. Creates and joins all threads and returns
	the data to main. 
*/
int bank_tester(int num_threads, Bank &myBank, std::vector <TXN_t> &data)
{
	bank = &myBank; 

	NUM_THREADS = num_threads;
	if (NUM_THREADS > 150) 
	{
		printf("ERROR; too many threads\n");
		exit(-1);
	}

	/* INITIALIZE THREADS & BARRIERS */
	global_init();

	(*bank).printBank();

	/* SETUP ARGUMENTS FOR THREADS */
	std::vector<std::vector<TXN_t>> split_arrays = split_vector_array(data, NUM_THREADS); //divide transactions
	struct bank_thread_args args[NUM_THREADS];
	
	//Setting up the master thread args data
	args[0].tid = 1; //i;
	args[0].array = split_arrays[0];
	args[0].elapsed_ns = 0; 
	args[0].elapsed_s = 0; 
	args[0].throughput_ns = 0; 
	args[0].throughput_s = 0; 

	/* LAUNCH THREADS */
	int ret;
	size_t i;
	for (i = 1; i < NUM_THREADS; i++)
	{
		args[i].tid = i + 1;
		args[i].array = split_arrays[i];
		args[i].elapsed_ns = 0; 
		args[i].elapsed_s = 0; 
		args[i].throughput_ns = 0; 
		args[i].throughput_s = 0; 
		// printf("creating thread %zu\n",args[i].tid);
		ret = pthread_create(&threads[i], NULL, &bank_thread, &args[i]);
		if (ret)
		{
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}

	//run bank_thread on the main/master thread
	bank_thread(&args); 

	/* JOIN THREADS */
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
	/* ERROR CHECKING */
	float total = (*bank).computeTotal(); 
	assert(total == (*bank).getTotal()); //Verify that the total is accurate based off of the account balances
	
	analyzeThrougput(args, NUM_THREADS); 
	
	(*bank).printBank(); 
	
	
	/* CLEAN UP */
	global_cleanup();

	return 0; 
}