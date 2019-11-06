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

class InputReader {

    public:

        InputReader(void);

        void set_node_id(int);

        int read_transaction_file(TransactionEvent*, double&);

    private:

        int nodeId;

        std::string fileName;
        std::ifstream loadFile;





};


#endif
