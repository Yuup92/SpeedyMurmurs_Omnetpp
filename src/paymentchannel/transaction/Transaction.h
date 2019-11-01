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

        static const int TRANSACTION_ALIVE = 200;
        static const int TRANSACTION_DEAD = 500;
        static const int TRANSACTION_ASLEEP = 150;

        // RECEIVER

        Transaction();
        void reset(void);
        void set_amount(double);
        int get_state(void);

        std::string start_transaction(District*, MessageBuffer*, int, int, int);
        std::string transaction_new_neighbourhood(District*, MessageBuffer*, int, int, int, int);
        std::string report_error(MessageBuffer*, BasicMessage*, int, District*);

        std::string update_sender(MessageBuffer*, int, District*);
        std::string update_forwarder(MessageBuffer*, int);
        std::string update_receiver(MessageBuffer*, int);

        std::string forward_transaction(MessageBuffer*, int, TransactionPath*);
        std::string receiving_transaction(MessageBuffer*, int, TransactionPath*);
        // void start_transaction(double, int, LinkedNode**, int, int*);


        int get_trans_id(void);
        TransactionPath * get_trans_path(int);

    private:

        TransactionPath transactionPath[MAXPATHS];
        District *d;

        int state;

        int numOfAddedPaths;
        double amount;
        double totalFees;
        int transId;
        int nodeRole;
        int endNode;
        int numOfPaths;

        int linkAttempts;

        // Sender Variable
        int receivedAcceptedPaths;

        // Receiver Variables
        int receivedPendingPaths;
        int receivedPendRequests;
        int receivedClosedTransactions;

        int neighbourhoodSet[MAXPATHS];
        int neighbourhoodComplement[MAXCOMPLEMENTPATHS];
        int lengthComplementNArray;

        Latency latency;

        bool check_path_id(int);
        void create_message_towards_sender(MessageBuffer*, TransactionPath*, int);
        void create_message_towards_receiver(MessageBuffer*, TransactionPath*, int);

        std::string s_pend_trans_handler(MessageBuffer*);
        std::string s_push_trans_handler(MessageBuffer*, TransactionPath*);

        std::string close_path(MessageBuffer*, TransactionPath*);
        std::string capacity_error(MessageBuffer*, int, District*);
        std::string forwader_capacity_error(MessageBuffer*, TransactionPath*);

        std::string timeout_error(MessageBuffer*, int);
        std::string handle_error(MessageBuffer*, int);

        std::string kill_transaction(MessageBuffer*, int);

        int get_transaction_path_index(int);

        void remove_transaction_path(TransactionPath*);
};

#endif
