/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/30/2020
Lab 1: 

    
*/

//Library includes
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <cmath>

//Developer includes
#include "array_splitter.hpp"

/*
	Splits a vector array into num_parts. 
*/
std::vector<std::vector<int>> split_vector_array(std::vector<int>array,int num_parts){ 
	
	//Create and allocate a vector of vectors
	std::vector<std::vector<int>> arrays; 
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
	std::vector<int>::iterator start = std::next(array.begin(), array_index * size); 
	 
	std::vector<int>::iterator array_end = array.end(); 
	int rem = array.size() - array_index * size;
	arrays[array_index].resize(rem); 
	//Copy
	std::copy(start, array_end, arrays[array_index].begin()); 
	arrays[array_index].resize(rem); 
	return arrays; 
}

