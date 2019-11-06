#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <omnetpp.h>
using namespace std;

#include "./TransactionPath.h"
#include "./TransactionMsg.h"

#include "../../msg/outgoing_buf/BufferedMessage.h"
#include "../../msg/outgoing_buf/MessageBuffer.h"

#include "../../connection/Neighbours.h"
#include "../../connection/District.h"
#include "../../connection/LinkedNode.h"
#include "../../utilities/RandHelper.h"

#include "../../latency/Latency.h"

#include "../../stat/Statistics.h"

class Transaction {

    public:

        static const int MAXPATHS = 10;
        static const int MAXCOMPLEMENTPATHS = 20;

        static const int TRANSACTION_ALIVE = 200;
        static const int TRANSACTION_DEAD = 500;
        static const int TRANSACTION_ASLEEP = 150;

        static const int TRANSACTION_ALIVE_TIME = 50;//s

        // RECEIVER

        Transaction();
        void set_stats(Statistics *);
        void reset(void);
        void set_amount(double);
        double get_amount(void);
        int get_state(void);
        int get_role(void);

        std::string start_transaction(District*, MessageBuffer*, int, int, int);
        std::string transaction_new_neighbourhood(District*, MessageBuffer*, int, int, TransactionPath*, int);
        std::string report_error(MessageBuffer*, BasicMessage*, int, District*);

        std::string update_sender(MessageBuffer*, int, District*);
        std::string update_forwarder(MessageBuffer*, int);
        std::string update_receiver(MessageBuffer*, int);

        std::string new_foward_transaction(MessageBuffer*, int, TransactionPath*);
        std::string add_new_forward_path(MessageBuffer*, TransactionPath*);

        std::string new_receiving_transaction(MessageBuffer*, int, TransactionPath*);
        std::string add_new_receiving_path(MessageBuffer*, int, TransactionPath*);

        int get_trans_id(void);
        int get_death_counter(void);
        int increment_death_counter(void);

        void check_alive_time(void);

    private:

        unordered_map<int, TransactionPath*> transPathMap;
        District *district;

        int state;
        int role;
        int deathCounter;


        int numOfAddedPaths;
        double amount;
        double totalFees;
        int transId;
        int nodeRole;
        int endNode;
        int numOfPaths;

        int linkAttempts;

        // Statistical Variables:
        int completedTransactions;

        // Sender Variable
        int receivedAcceptedPaths;
        int sentClosedPaths;
        int receivedClosedPaths;


        // Receiver Variables
        int receivedPendingPaths;
        int receivedPendRequests;
        int receivedClosedTransactions;

        int neighbourhoodSet[MAXPATHS];
        int neighbourhoodComplement[MAXCOMPLEMENTPATHS];
        int lengthComplementNArray;

        Latency latency;
        simtime_t transactionAliveTime;

        Statistics *stats;

        void create_message_towards_sender(MessageBuffer*, TransactionPath*, int);
        void create_message_towards_receiver(MessageBuffer*, TransactionPath*, int);

        std::string sender_pend_capacities(MessageBuffer*);
        std::string sender_push_transaction(MessageBuffer*, TransactionPath*);
        std::string sender_close_transaction(TransactionPath*);

        std::string close_path(MessageBuffer*, TransactionPath*);
        std::string capacity_error(MessageBuffer*, TransactionPath*, District*);
        std::string forwader_capacity_error(MessageBuffer*, TransactionPath*);

        std::string timeout_error(MessageBuffer*, TransactionPath*);
        std::string handle_error(MessageBuffer*, TransactionPath*);

        std::string kill_transaction(MessageBuffer*, TransactionPath*);

        int get_transaction_path_index(int);

        void remove_transaction_path(TransactionPath*);

        bool check_for_existing_path_id(int);
        TransactionPath* get_trans_path_from_map(int);

};

#endif
