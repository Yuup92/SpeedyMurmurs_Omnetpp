#ifndef TRANSACTIONS_H_
#define TRANSACTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>
using namespace std;

#include "./TransactionPath.h"
#include "./Transaction.h"

#include "../../msg/outgoing_buf/MessageBuffer.h"

#include "../../connection/District.h"
#include "../../stat/Statistics.h"

class Transactions {

    public:

        static const int MAX_TRANSACTIONS = 1000;

        Transactions();
        void set_stats(Statistics*);
        int get_current_trans();
        int get_trans_map_size();

        // Send Transaction
        std::string add_send_transaction(District*, MessageBuffer*, double, int, int);

        // Forward Transaction
        std::string add_path_to_transaction_id(MessageBuffer*, int, TransactionPath*);
        std::string new_forwarding_transaction(MessageBuffer*, int, TransactionPath*);

        //Receive Transaction
        std::string new_receiving_transaction(MessageBuffer*, int, TransactionPath*);

        bool check_for_trans_id(int);
        bool check_for_path_id(int);
        Transaction * get_transaction_from_path(int);
        Transaction * get_transaction(int);
        TransactionPath * get_path(int, int);
        // int add_send_transaction(double, int, LinkedNode**, int, int*);

        bool remove_dead_transactions(void);


        // HASH MAP
        bool check_for_existing_transaction(int);
        Transaction* get_transaction_from_map(int);


    private:

        int currentNumberOfTransactions;

        unordered_map<int, Transaction*> transactionMap;

        double amount;
        // Is an Id for the whole transaction
        int transId;
        // Unique for every path
        int pathTransId;
        int pathsChecked;
        int multiPathState;
        int indexLinkedNode;
        int edgeTowardsReceiver;
        int edgeTowardsSender;
        int endNode;
        int state;
        int neighbourhood;

        LinkedNode *linkTowardsReceiver;
        LinkedNode *linkTowardsSender;

        int hopCount;
        double fees;

        int startTime_ms;
        int endHTLC;

        Statistics *stats;

        bool check_transId_exists(int, int&);

        bool remove_transaction(int);
};

#endif
