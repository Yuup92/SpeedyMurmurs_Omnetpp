#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <chrono>
#include <ctime>

// This class can be updated with:
// Producer–consumer problem

class FileWriter {
    public:
        const int static BUFFER = 1000;

        FileWriter(void);

        void set_node_id(int, int);
        void initialize_file(void);

        void update_variables(std::string, int, int, int, int, int, int, int, double, double, double, std::string);

    private:
        int nodeId;
        int amountOfNeighbours;
        std::string fileName;

        std::ofstream  myFile;

        int index;
        std::string time[10000];
        double networkDelay[10000];
        double cryptoDelay[10000];
        double osDelay[10000];

        int numOfTransactionsCompleted[10000];
        int numOfTransactionsStarted[10000];
        int numOfTransactionsForwarded[10000];
        int numOfTransactionsForwardedCompleted[10000];
        int currentNumberOfTransactions[10000];
        int numOfTransactionsFailed[10000];
        int numOfTransactionsFailedCapacity[10000];
        std::string capacities[10000];
        // LinkCapacities, should add it as a string
        void write_to_file(void);



};


#endif
