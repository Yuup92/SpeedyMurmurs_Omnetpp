#ifndef INPUTREADER_H_
#define INPUTREADER_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <chrono>
#include <ctime>
#include <vector>
#include <sstream>

#include "../main/TransactionEvent.h"
#include "../connection/capacity/LinkCapacity.h"
class InputReader {

    public:

        InputReader(void);

        void set_node_id(int);

        int read_transaction_file(TransactionEvent*, double&);
        int read_capacity_file(int, int, LinkCapacity*);

    private:

        int nodeId;

        std::string fileName;
        std::string fileNameCapacities;
        std::ifstream loadFile;
        std::ifstream loadFileCap;





};


#endif
