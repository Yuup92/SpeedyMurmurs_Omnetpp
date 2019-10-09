#include "./FileWriter.h"

FileWriter::FileWriter(void) {
    nodeId = 0;
    index = 0;
}

void FileWriter::set_node_id(int id, int numberNeighbours) {
    nodeId = id;
    fileName = "N_" + std::to_string(id) + ".txt";
    amountOfNeighbours = numberNeighbours;
}

void FileWriter::initialize_file(void) {

    // Make a result directory with time
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string t(std::ctime(&end_time));
    std::string dir = "results/result_" + t + "/";
    mkdir(dir.c_str(), 0777);

    // Make the file
    fileName = dir + fileName;
    myFile.open(fileName.c_str(), std::ios_base::app);
    myFile << "time, tCompleted, tStarted, tForwarded, currentTransactions, tFailed, tTimedout, nDelay, cDelay, osDelay, numLinkedNeighbours, ";
    for(int i = 0; i < amountOfNeighbours; i++) {
        myFile << "id, capacity, virtualCap, numOfpayments, numOfPaymentsPending,";
    }
    myFile <<"\n";

    myFile.close();
}

void FileWriter::update_variables(std::string t, int tCompleted, int tStarted, int tForwarded,
                                    int tCompletedForwarded, int curT, int tFailed, int cFailed,
                                    double nDelay, double cDelay, double oDelay, std::string cap) {

    time[index] = t;

    numOfTransactionsCompleted[index] = tCompleted;
    numOfTransactionsStarted[index] = tStarted;
    numOfTransactionsForwarded[index] = tForwarded;
    numOfTransactionsForwardedCompleted[index] = tCompletedForwarded;
    currentNumberOfTransactions[index] = curT;
    numOfTransactionsFailed[index] = tFailed;
    numOfTransactionsFailedCapacity[index] = cFailed;
    networkDelay[index] = nDelay;
    cryptoDelay[index] = cDelay;
    osDelay[index] = oDelay;
    capacities[index] = cap;

    index++;

    if(index < FileWriter::BUFFER) {
        write_to_file();
    }
}

void FileWriter::write_to_file(void) {

    myFile.open(fileName.c_str(), std::ios_base::app);

    for(index = 0; index < FileWriter::BUFFER; index++) {
        std::string res = time[index] + ",";
        res = res + std::to_string(numOfTransactionsCompleted[index]) + ",";
        res = res + std::to_string(numOfTransactionsStarted[index]) + ",";
        res = res + std::to_string(numOfTransactionsForwarded[index]) + ",";
        res = res + std::to_string(currentNumberOfTransactions[index]) + ",";
        res = res + std::to_string(numOfTransactionsFailed[index]) + ",";
        res = res + std::to_string(networkDelay[index]) + ",";
        res = res + std::to_string(cryptoDelay[index]) + ",";
        res = res + std::to_string(osDelay[index]) + ",";
        res = res + std::to_string(amountOfNeighbours) + ",";
        res = res + capacities[index] + "/n";

        myFile << res;
    }

    index = 0;
    myFile.close();


}
