#ifndef ARRAY_SPLITTER_HPP
#define ARRAY_SPLITTER_HPP
/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: 9/21/2020
Lab 1: 
    
*/

#include <vector>
#include <string>

// struct TXN_t{
//     string action; 
//     int toID; 
//     int fromID; 
//     float amt; 
// }; 

std::vector<std::vector<int>> split_vector_array(std::vector<int>array,int num_parts); 

#endif