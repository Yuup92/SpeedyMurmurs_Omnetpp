#include "./InputReader.h"

InputReader::InputReader() {
    fileName = "Transactions.txt";
}

void InputReader::set_node_id(int nId) {
    nodeId = nId;
}

int InputReader::read_transaction_file(TransactionEvent *transEvent, double &endT) {

    std::string res = "";
    int numTransEvents = 0;


    loadFile.open(fileName.c_str());

    std::string line = "";
    std::vector<std::string> vect;

    double endTime = 0;
    bool skipFirst = true;

    // Read all data
    if(loadFile.is_open()) {
        while(getline(loadFile, line)) {
            if(skipFirst) {
                skipFirst = false;
            } else {
                std::vector<std::string> input;

                std::stringstream ss(line);

                while(ss.good()) {
                    std::string substr;
                    getline(ss, substr, ',');
                    input.push_back(substr);
                }

                if(std::stoi(input[1]) == nodeId) {
                    double time = std::stod(input[0]);
                    double amount = std::stod(input[3]);
                    int dest = std::stoi(input[2]);
                    transEvent[numTransEvents].set_transaction(time, amount, dest);
                    numTransEvents++;
                }

                if(std::stod(input[0]) > endTime) {
                    endTime = std::stod(input[0]);
                }
            }
        }
    }

    endT = endTime;

    return numTransEvents;
}
