#ifndef PAYMENTCHANNEL_H_
#define PAYMENTCHANNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <math.h>

#include "../paymentchannel/concurrency/ConcurrencyBlocking.h"
#include "../connection/District.h"

#include "../msg/outgoing_buf/MessageBuffer.h"
#include "../msg/outgoing_buf/BufferedMessage.h"
#include "../connection/Neighbours.h"
#include "../connection/LinkedNode.h"
#include "../latency/Latency.h"
#include "./transaction/Transactions.h"
#include "./transaction/TransactionPath.h"
#include "./transaction/TransactionMsg.h"
#include "../stat/Statistics.h"

class PaymentChannel {

    public:
        PaymentChannel();

        Transactions transactions;

        // PARAM
        static const int CHANNELS_USED = 10;

        // Message Types
        static const int MESSAGE_TYPE = 3;

        static const int CONCURRENCY_BLOCKING = 1;
        static const int CONCURRENCY_NON_BLOCKING = 2;

        static const int MULTIPATH_QUERY = 50;

        static const int TRANSACTION_QUERY = 100;
        static const int TRANSACTOIN_QUERY_ACCEPT = 101;
        static const int TRANSACTION_REJECT = 110;

        static const int TRANSACTION_PUSH = 200;
        static const int TRANSACTION_PUSH_ACCEPT = 201;
        static const int TRANSACTION_PUSH_REJECT = 210;

        static const int CLOSE_TRANSACTION = 300;
        static const int ACCEPT_TRANSACTION_CLOSE = 305;

        static const int CAPACITY_ERROR = 600;
        static const int TIMEOUT = 605;
        static const int ERROR = 610;

        // States
        static const int STATE_WAITING = 100;

        static const int STATE_SENDING_NODE = 200;
        static const int STATE_FORWARDING_NODE = 210;
        static const int STATE_RECEIVING_NODE = 220;

        static const int MAXPATHS = 5;

        void set_node_id(int);
        void set_concurrency_type(int);
        void set_stats(Statistics*);

        void set_district(District *);
        District *get_district(void);

        int get_current_transactions_amount(void);

        int get_current_transaction_size(void);
        int get_current_transaction_index(void);

        int get_num_completed_transactions(void);
        int get_num_forwarded_transactions(void);
        int get_num_of_total_transactions(void);
        int get_num_forwarded_completed_transactions(void);

        int get_failed_transactions(void);
        int get_capacity_failure(void);

        double get_network_delay(void);
        double get_os_delay(void);
        double get_crypto_delay(void);

        void check_for_dead_transactions(void);

        // Message handling
        std::string handle_message(BasicMessage *, int);
        int get_message_count(void);
        BufferedMessage * get_message(void);

        std::string send(int, double);
        std::string multi_path_send(int, double, int);
        std::string forward_send(int, int, int, double, int, int);

        std::string to_string(void);
        std::string capacities_to_string(void);
        std::string delay_to_string(void);

    private:

        District *district;

        TransactionMsg transMsg;

        MessageBuffer msgBuf;

        int nodeId;
        int concurrencyType;
        int currentTransactionId;
        int pendingTransactionAmount;

        int currentNeighbour;
        int currentNeighbourList[10];

        int numberOfCompletedTransactions;
        int numberOfForwardedTransactions;
        int numberOfForwardedTransactionsCompleted;
        int numberOfTotalTransactions;

        int numberOfCapacityErrors;
        int numberOfReceivedCapacityErrors;

        int numberOfTimeoutErrors;
        int numberOfReceivedTimeoutErrors;

        int numberOfErrors;
        int numberOfReceivedErrors;

        int currentState;

        double msgDelay;
        Latency latency;

        int transactionConnectionIndex;

        Statistics *stats;

        void handle_query_message(int, int, int, int, double, int);
        void handle_query_accept_message(TransactionPath*);

        void handle_transaction_push(int, int);
        void handle_transaction_push_reply(int);

        void handle_close_transaction(int, int, int);
        void handle_accept_close_transaction(int);

        void handle_close_link(int, int);
        void handle_close_link_reply(int);

        void handle_capacity_error(int);
        void handle_timeout_error(int);
        void handle_general_error(int);

        void update_message_buf(BasicMessage *, int);
};

#endif

