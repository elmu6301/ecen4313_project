/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/30/2020
Lab 1: 

*Note modified from test.c provide
    
*/

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
#include "../bank/bank.hpp"

pthread_t *threads;
pthread_mutex_t * locks; 


size_t NUM_THREADS;
pthread_barrier_t p_bar;

struct timespec tstart;
struct timespec tend;



/*
	Allocates all required data. 
*/
void global_init()
{
	threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t));
	//inialize each lock 
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

	printf("\nThread[%zu] ready!",tid); 
	//Process each transaction
	for(int i = 0; i < array.size(); i++){
		//Call the appropriate function based off of the action field
		if(array[i].action.compare("deposit")== 0){
			printf("\nDepositing %.2f in account[%d]", array[i].amt, array[i].toID); 
			// deposit(array[i].toID, array[i].amt); 
		}else if(array[i].action.compare("withdraw")==0){
			printf("\nDepositing %.2f in account[%d]", array[i].amt, array[i].toID);
			// withdraw(array[i].toID, array[i].amt);  
		}else if(array[i].action.compare("transfer")==0){
			printf("\nTransfering %.2f from account[%d] to account[%d]", array[i].amt, array[i].fromID,array[i].toID); 
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
int bank_tester(int num_threads, Bank &bank, std::vector <TXN_t> &data)
{

	NUM_THREADS = num_threads;
	if (NUM_THREADS > 150) 
	{
		printf("ERROR; too many threads\n");
		exit(-1);
	}
	NUM_THREADS = 1; 
	//Global init
	global_init();

	//Split array into NUM_THREADS parts
	std::vector<std::vector<TXN_t>> split_arrays = split_vector_array(data, NUM_THREADS);
	for(int i = 0; i < NUM_THREADS; i++){
		printf("\n-------Array[%d]----------------------------------",i); 
		printTXNS(split_arrays[i]); 
	}

	
	struct bank_thread_args args[NUM_THREADS];
	
	//Setting up the master thread args data
	args[0].tid = 1; //i;
	args[0].array = split_arrays[0];


	// // launch threads
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



	global_cleanup();

	unsigned long long elapsed_ns;
	elapsed_ns = (tend.tv_sec - tstart.tv_sec) * 1000000000 + (tend.tv_nsec - tstart.tv_nsec);
	printf("\nElapsed (ns): %llu\n", elapsed_ns);
	double elapsed_s = ((double)elapsed_ns) / 1000000000.0;
	printf("Elapsed (s): %lf\n", elapsed_s);
	return 0; 
}