#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./TransactionPath.h"
#include "./TransactionMsg.h"

#include "../../msg/outgoing_buf/BufferedMessage.h"
#include "../../msg/outgoing_buf/MessageBuffer.h"

#include "../../connection/Neighbours.h"
#include "../../connection/District.h"
#include "../../connection/LinkedNode.h"
#include "../../utilities/RandHelper.h"

#include "../../latency/Latency.h"

class Transaction {

    public:

        static const int MAXPATHS = 10;
        static const int MAXCOMPLEMENTPATHS = 20;

        // RECEIVER

        Transaction();
        std::string start_transaction(District*, MessageBuffer*, double, int, int);
        std::string forward_transaction(MessageBuffer*, int, TransactionPath*);
        std::string receiving_transaction(MessageBuffer*, int, TransactionPath*);
        // void start_transaction(double, int, LinkedNode**, int, int*);

        std::string update_sender(MessageBuffer*, int);
        std::string update_forwarder(MessageBuffer*, int);
        std::string update_receiver(MessageBuffer*, int);


        int get_trans_id(void);
        int get_execution_role(void);
        TransactionPath * get_trans_path(int);

    private:

        TransactionPath transactionPath[MAXPATHS];
        District *d;

        int executionRole;
        int state;

        int numOfAddedPaths;
        double Amount;
        double totalFees;
        int transId;
        int nodeRole;
        int endNode;
        int numOfPaths;

        // Sender Variable
        int receivedAcceptedPaths;

        // Receiver Variables
        int receivedPendingPaths;
        int receivedPendRequests;

        int setNeighbourhood[MAXPATHS];
        int complementNeighbourhood[MAXCOMPLEMENTPATHS];
        int lengthComplementNArray;

        Latency latency;

        bool check_path_id(int);
        void create_message_towards_sender(MessageBuffer*, TransactionPath*, int);
        void create_message_towards_receiver(MessageBuffer*, TransactionPath*, int);

        int get_transaction_path_index(int);

        struct transaction_connection{
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

        };

};

#endif
