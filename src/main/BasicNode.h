#ifndef BASICNODE_H_
#define BASICNODE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <cstdlib>

#include "./TransactionEvent.h"

#include "../paymentchannel/PaymentChannel.h"
#include "../msg/basicmsg_m.h"
#include "../sync/NodeClock.h"

#include "../connection/District.h"
#include "../connection/Neighbours.h"

#include "../msg/outgoing_buf/BufferedMessage.h"

#include "../alg/leader_election/LeaderElection.h"
#include "../alg/spanning_tree/SpanningTree.h"

#include "../output/FileWriter.h"
#include "../output/SaveState.h"
#include "../output/InputReader.h"

#include "../stat/Statistics.h"

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    public:
        static const int NUM_OF_TREES = 10;
        static const int NUM_OF_NODES = 23;
        static const int STATIC_CAPACITY = 100;
        static const bool RUN_SPANNINGTREE = true;
        static const bool SAVE_STATE = false;
        static const bool SAVE_RESULTS = false;

        // States of basic node
        static const int SPANNING_TREE_STATE = 100;
        static const int COORDINATE_SHARING_STATE = 200;
        static const int SIMULATION_STATE = 300;
        static const int FINISHED_STATE = 666;

        BasicNode();
        virtual ~BasicNode();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();
        virtual void finish() override;

        virtual void start_message_timer();

        virtual void handleMessage(cMessage *msg) override;

        // Starts the search for a leader
        virtual void broadcastLeaderRequest();

    private:
        int state;

        NodeClock clock;
        int nodeId;
        double endTransactionTime;

        int spanningTreeSearchIndex;
        int coordinateMsgsSent;
        int coordinateMsgsReceived;

        bool firstSend;
        bool nodeStateLoaded;
        bool runningSimulation;
        bool runningSpanningTree;

        // NUM of trees must be equal to the number of SpanningTree and Neighbours
        SpanningTree spanningTrees[NUM_OF_TREES];
        District district;

        LeaderElection leader_election;
        PaymentChannel paymentChannel;

        FileWriter fileWriter;
        SaveState saveState;
        InputReader inputReader;

        bool simulationSetup;
        int transEventIndex;
        int numOfTransEvents;
        double periodicUpdate;
        double startTimeTransactions;
        double endOfTransactions;
        TransactionEvent transactionEvent[1000];

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *startTransaction;
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual void sendMessagesFromBuffer(void);

        virtual std::string parseNodeID(const char* nodeName);

        void district_initialization(void);
        void IO_initialization(void);
        void payment_channel_initialization(void);
        void initialize_stats(void);
        void leaderInitialization(void);

        void internal_message_handling(void);
        void internal_spanning_tree_handling(void);
        void external_message_handling(BasicMessage*, cMessage*);
        void process_spanning_tree_msg(BasicMessage*, cMessage*);

        void check_coordinate_handling(void);

        void sendLeaderMessages(void);
        void sendSpanningTreeMessages(void);
        void sendTransactionMessages(void);
        void sendDistrictMessages(void);

        void run_simulation(void);
        void set_timer_transactions(void);

        /**
         * Statistic Collection Variables
         */

        void check_stat_flags(void);


        /**
         * Message Statistics
         */
        Statistics stats;

        long msgSendCount;
        long sptMsgCount; //spanning tree msgs
        long pcMsgCount; //payment channel msgs

        cOutVector msgSendCountVector;
        cOutVector sptMsgCountVector;
        cOutVector pcMsgCountVector;
};

Define_Module(BasicNode);
#endif
