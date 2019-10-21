#ifndef SPANNINGTREE_H__
#define SPANNINGTREE_H__

#include <stdlib.h>
#include <limits>
#include <omnetpp.h>

#include "../../msg/outgoing_buf/MessageBuffer.h"
#include "../../msg/outgoing_buf/BufferedMessage.h"
#include "../../connection/Neighbours.h"
#include "../../alg/spanning_tree/QueuedMessage.h"
#include "../../connection/LinkedNode.h"


struct state_edge {
    int outgoingEdge;
    int nodeId;
    int state;
    int weight;
    int numOfChildren;
    int children[500];
    bool edgeTowardsRoot;
};

class SpanningTree
{
    public:
        SpanningTree();
        static const int MESSAGE_TYPE = 2;

        static const int INITIATE = 1;
        static const int TEST = 2;
        static const int ACCEPT = 3;
        static const int REJECT = 4;
        static const int REPORT = 5;
        static const int CHANGEROOT = 6;
        static const int CONNECT = 7;

        static const int EDGESTATE_Q_MST = 100;
        static const int EDGESTATE_IN_MST = 101;
        static const int EDGESTATE_NOT_IN_MST = 102;

        static const int STATE_SLEEPING = 200;
        static const int STATE_FIND = 201;
        static const int STATE_FOUND = 202;
        static const int STATE_FINDING_WEIGHT = 203;

        static const int WEIGHT_REQUEST = 300;
        static const int WEIGHT_REPONSE = 301;

        static const int DOWNTREE_BROADCAST = 400;
        static const int ROOT_QUERY = 420;
        static const int ROOT_QUERY_ACCEPT = 425;
        static const int ROOT_QUERY_REJECT = 430;
        static const int UPTREE_REPLY = 450;

        static const int SEARCH_NEXT_TREE = 500;



        static const int INSPECTION = 666;

        void wake_up(void);
        void get_weight_edges(void);

        void set_num_neighbours(int);
        //void set_neighbours(Neighbours*);
        int get_num_neighbours(void);
        int get_num_sent_messages(void);
        int get_initial_queue_index(void);
        void set_node_id(int);
        void set_spanning_tree_index(int);

        int get_state(void);
        int get_level(void);
        std::string get_state_edge(void);

        int get_length_vector_cor(void);
        int* get_vector_cor(void);

        bool is_node_root(void);
        bool full_broadcast_finished(void);
        bool get_linked_nodes_updated(void);

        void handle_message(BasicMessage*, int, omnetpp::simtime_t);

        void check_waiting_replys(void);

        void start_spanning_tree_search(void);

        void pulse_link(void);

        int get_message_count(void);
        BufferedMessage * get_message(void);

        void check_queued_messages(void);
        bool check_connect_queue_conditions(int, int);
        bool check_report_queue_conditions(int, int);
        bool check_test_queue_conditions(int);

        void check_root(void);
        void broadcast(void);

        int update_linked_nodes(LinkedNode *);

        void search_for_next_tree(int);

        std::string state_edges_to_string(void);

    private:
        MessageBuffer msgBuf;
        Neighbours *connectedNeighbours;

        int fragmentLevel; // LN
        int fragmentName;  // FN

        int stateNode; // SN
        int edgeTowardsRoot; //in-branch

        int currentMinEdge;
        int testEdge;
        int bestEdge;
        int bestWeight;
        int findCount; // number of expected messages

        QueuedMessage *initialQueue[500];
        QueuedMessage *connectQueue[500];
        QueuedMessage *reportQueue[500];
        QueuedMessage *testQueue[500];

        int initialQueueIndex;
        int connectQueueIndex;
        int reportQueueIndex;
        int testQueueIndex;

        int testCounter;

        int handle_message_counter;
        int handle_spanning_tree_counter;
        int handle_queue;
        int maxQueue;
        int handle_connect_msg;
        int msgSentDownStream;

        state_edge stateEdges[2000];
        int sent_requests;
        bool edgesWeightUpdated;
        bool noInitialQueueMessages;

        int spanningTreeIndex;
        int spanningTreeNodeId;
        int nodeId;
        int parentNodeId;

        int rootNodeId;
        bool isRoot;
        bool fullBroadcast;

        bool sendNextTree;

        int currentDepth;
        int maxDepth;
        bool linkedNodesUpdated;

        int numConnectedNodes;
        int listChildrenNodes[500];
        int indexChildrenNodes;

        int vectorList[1000];
        int lengthVectorList;

        double msgDelay;
        omnetpp::simtime_t previousSimTime;

        std::string reply_broadcast;

        void update_initial_queue(BasicMessage *, int);
        void handle_initial_queue(void);

        void handle_weight_request(int, int, int);
        void handle_weight_response(int, int, int);

        void handle_spanning_tree_message(BasicMessage *, int);

        void handle_connect(int, int);
        void handle_initiate(int, int, int, int);
        void handle_test(int, int, int);
        void handle_accept(int);
        void handle_reject(int);
        void handle_report(int, int);

        void handle_root_query(int, int);
        void handle_root_accept(void);

        void handle_downtree_broadcast(int, int);
        void handle_uptree_reply(int, int);

        void test(void);
        void report(void);
        void change_root(void);
        void set_edge_towards_root(int);

        void start_building_tree(void);

        bool broadcast_down_stream();
        bool broadcast_up_stream(int);

        void send_spanning_tree_request(void);
        void remove_child(int);
        int get_rand_out_gate(void);
        void fill_state_edges(state_edge *);
        int find_minimum_weight_edge(void);
        int find_edge_in_stateEdge(int);

        int number_of_children_check(void);
        void aggregate_children_list(void);

        // void update_neighbours(void);

        void send_inspection(int, int);
        void update_message_buf(BasicMessage *, int);
        static BasicMessage * inspection(int);

        static BasicMessage * weight_request(int, int, int);
        static BasicMessage * weight_response(int, int, int, int);

        static BasicMessage * connect(int, int);
        static BasicMessage * initiate(int, int, int, int);
        static BasicMessage * test(int, int, int);
        static BasicMessage * accept(int);
        static BasicMessage * reject(int);

        static BasicMessage * report(int, int);
        static BasicMessage * change_root_msg(int);

        static BasicMessage * broadcast_down_tree(int, int, int, int, int*);
        static BasicMessage * broadcast_up_tree(int, int, int*, int);

        static BasicMessage * root_query(int, int);
        static BasicMessage * root_query_accept(int);
        static BasicMessage * root_query_reject(int);

        static BasicMessage * search_next_tree(int,int);

};

#endif
