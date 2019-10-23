#ifndef TRANSACTIONS_H_
#define TRANSACTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./TransactionPath.h"
#include "./Transaction.h"

#include "../../msg/outgoing_buf/MessageBuffer.h"

#include "../../connection/District.h"

class Transactions {

    public:

        static const int MAX_TRANSACTIONS = 1000;

        Transactions();
        int get_current_trans();

        std::string add_send_transaction(District*, MessageBuffer*, double, int, int);
        std::string add_forward_transaction(MessageBuffer*, int, TransactionPath*);
        std::string add_receiving_transaction(MessageBuffer*, int, TransactionPath*);

        bool check_for_trans_id(int);
        Transaction * get_transaction(int);
        TransactionPath * get_path(int, int);
        // int add_send_transaction(double, int, LinkedNode**, int, int*);

        bool remove_dead_transactions(void);

    private:

        int currentNumberOfTransactions;
        Transaction transactions[MAX_TRANSACTIONS];

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

        bool check_transId_exists(int, int&);


        bool remove_transaction(int);
};

#endif
