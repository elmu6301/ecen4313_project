/*
ECEN 4313: Concurrent Programming
Author: Elena Murray
Date: X/XX/2020
Lab XXXX: 
*/

//Library includes
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <getopt.h>
#include <vector>
#include <string>
#include <exception> 

using namespace std; 
//Developer includes
#include "bank.hpp"
#include "bank_tester.hpp"

//Global Variables
char my_name[] = "Elena Murray"; 


void printUsage(){
    printf("bank [--init initfile.txt] [--tnx tnxfile.txt] [--o outfile.txt] [-t NUM_THREADS] [--alg=<sgl,2p1,stm,htm,opt>]\n");
}


int readInitData(string initFile, std::vector <float> &data){
    ifstream fileIn;  
    fileIn.open(initFile); 
    if(!fileIn){
        cout<<"Unable to open the file: "<<initFile<<endl; 
        return -1; 
    }

    //Read in the input
    string line; 
    float el; 
    while(getline(fileIn, line)){
        try{
            el = stof(line); 
            data.push_back(el); 
        }catch(exception & e){
            cout<<"Error: invalid data detected."<<endl; 
            fileIn.close(); 
            return -4; 
        }
    }
    fileIn.close(); 
    return 1; 
}

int readTxnData(string txnFile, std::vector <TXN_t> &data){
    ifstream fileIn;  
    fileIn.open(txnFile); 
    if(!fileIn){
        cout<<"Unable to open the file: "<<txnFile<<endl; 
        return -1; 
    }

    //Read in the input
    string line, temp; 

    TXN_t txn; 
    int i = 0; 
    while(getline(fileIn, line)){
        
        //Get action
        txn.action = line.substr(0,line.find(",")); 
        if(txn.action.compare("stop")==0){
            fileIn.close(); 
            return 1; 
        }
    
        // Get to account id
        line = line.substr(line.find(",")+1);
        temp = line.substr(0,line.find(",")); 
        try{
            txn.toID = stoi(temp); 
        }catch(exception & e){
            cout<<"Error: invalid to account id detected:"<<temp<<endl; 
            fileIn.close(); 
            return -4; 
        }
        //Get from account id
        line = line.substr(line.find(",")+1);
        temp = line.substr(0,line.find(",")); 

        try{
            txn.fromID = stoi(temp); 
        }catch(exception & e){
            cout<<"Error: invalid from account id detected:"<<temp<<endl; 
            fileIn.close(); 
            return -4; 
        }
        //Get amount 
        line = line.substr(line.find(",")+1);
        try{
            txn.amt = stof(line); 
        }catch(exception & e){
            cout<<"Error: invalid amount detected."<<endl; 
            fileIn.close(); 
            return -4; 
        }
        // cout<<"TXN["<<i<<"]: action = "<<txn.action<<" toID = "<<txn.toID<<" fromID = "<<txn.fromID<<" amt = "<<txn.amt<<endl; 
         data.push_back(txn); 
    }
    fileIn.close(); 
    return 1; 
}

int printLedger(string outFile, Bank &bank){
    // Output sorted data to output file
    ofstream fileOut; 
    fileOut.open(outFile); 
     if(!fileOut){
        cout<<"Unable to open the file: "<<outFile<<endl; 
        return -4; 
    }

    int num_accts = bank.getNumAccounts(); 
    for(int i = 0; i < num_accts; i++){
        fileOut<<"Account["<<i<<"]: $"<<fixed<<std::setprecision(2)<<bank.getAccountBalance(i)<<endl; 
    }
    fileOut<<"Total: $"<<fixed<<std::setprecision(2)<<bank.getTotal()<<endl; 
    fileOut.close();    
    return 1; 
}

//main function
int main(int argc, char* argv[]){ 

    //variable for parsing the command line
    string initFile; //stores the name of the file to initialize the bank system from
    string txnFile; //stores the name of the file to execute transactions from
    string outFile; //stores the name of the file to output the data 
    string threads; 
    string alg; 


    char opt; //stores the option value
    int num_threads = 1; //stores the number of threads to create, set to default of 1 thread (i.e. master thread)
    
    static struct option longopt[] = {
        {"init", required_argument, NULL, 'i'},// init file
        {"txn", required_argument, NULL, 'x'}, // transaction file
        {"t", required_argument, NULL, 't'}, // threads
        {"alg", required_argument, NULL, 'a'} // algorithm
    }; 
    char * optstr = "i:x:t:a:o:"; 
    //Parse the rest of the command line
    while((opt = getopt_long(argc, argv, optstr, longopt, NULL))!=-1){
        // cout<<opt<<endl; 
        switch(opt){
            case 'i': 
                initFile = optarg; 
                break; 
            case 'x':
                txnFile = optarg; 
                break;  
            case 't':
                threads = optarg; 
                break; 
            case 'a':
                alg = optarg; 
                break; 
            case 'o':
                outFile = optarg; 
                break; 
        }
    }
    //Check options to make sure that they are valid
    if(initFile.rfind(".txt")==string::npos){
        cout<<"The initialization file: "<<initFile<<" is not a .txt file."<<endl; 
        printUsage(); 
        return -1;
    }

   
    if(txnFile.rfind(".txt")==string::npos){
        cout<<"The transaction file: "<<txnFile<<" is not a .txt file."<<endl; 
        printUsage(); 
        return -1;
    } 

    int txnAlg; 
    if(alg.compare("sgl")== 0){
        txnAlg = SGL; 
    }else if(alg.compare("2pl")==0){
        txnAlg = PHASE_2; 
    }else if(alg.compare("stm")==0){
        txnAlg = STM; 
    }else if(alg.compare("htm")==0){
        txnAlg = HTM_SGL; 
    }else if(alg.compare("opt")==0){
        txnAlg = OPTIMIST; 
    }else{
        cout<<"An invalid algorithm was entered."<<endl; 
        printUsage();
        return -3; 
    }
    //Convert threads to an integer
    if(threads.empty()){
        cout<<"No threads were entered. Running the program with only the master thread (NUM_THREAD = 1). "<<endl; 
    }else{
        try{
            num_threads = stoi(threads); 
        }catch( exception &e){
            //Print message
            cout<<"An invalid thread count was entered. Try using an integer value."<<endl; 
            printUsage(); 
            return -4; 
        }
        //Not enough threads
        if(num_threads<1){
            cout<<"An invalid thread count was entered. A master thread is required.\nThe number of threads to run must be greater than 1."<<endl; 
            printUsage(); 
            return -4; 
        }
    }


    //Variable declaration: 
    vector <float> initData; 
    vector <TXN_t> txnData; 
    int res; 

    //read in data from the initialization file and store it to initData
    res = readInitData(initFile, initData); 
    if(res !=1){
        return -5; 
    }
    int num_accounts = initData.size(); 

    cout<<"Creating Bank with "<<num_accounts<<" accounts and initialized by '"<<initFile<<"'.txt"<<endl; 

    //create bank and initalize it with initData
    Bank myBank = Bank(txnAlg, initData);

    //Read in transactions
    res = readTxnData(txnFile, txnData);
    if(res !=1){
        return -5; 
    }
    int data_size = txnData.size(); 
    //Check to make sure that the size of data is larger than the number of threads
    if(num_threads>data_size){
        cout<<"An invalid thread count was entered. The number of threads cannot exceed the amount of data.\nSetting the number of threads to match the amount of data."<<endl; 
        num_threads = data_size; 
    }
    bank_tester(num_threads,myBank, txnData); 
    
    //Perform error checking
    printLedger(outFile, myBank); 
}


