#ifndef LINKEDNODE_H_
#define LINKEDNODE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <cstdlib>

#include "../connection/capacity/LinkCapacity.h"
#include "../msg/outgoing_buf/MessageBuffer.h"
#include "../msg/outgoing_buf/BufferedMessage.h"

class LinkedNode {
    public:
        LinkedNode();

        bool sentConnectedCoordinates;
        bool receivedConnectedCoordinates;

        void update_node(int, int, int, int, int *, bool);
        void update_node(int, int, int, int, int *, bool, LinkCapacity *);
        void update_node(int, int, int *, bool, int);
        void update_node(int, int, int, int *, bool);

        void set_connected_node_id(int);
        int get_connected_node_id(void);

        void set_connecting_edge(int);
        int get_connecting_edge(void);

        void set_state(int);
        int get_state(void);

        void set_weight(int);
        int get_weight(void);

        void set_number_of_children(int);
        int get_number_of_children(void);

        void set_children(int*);
        int * get_children(void);

        int get_child(int);

        void set_capacity(LinkCapacity*);
        double get_capacity(void);

        double get_virtual_capacity(void);

        void set_sent_coordinate_bool(bool);

        int process_transaction(int);

        bool get_edge_towards_root(void);
        void set_edge_towards_root(bool);

        bool pend_increase_transaction(double, int);
        bool pend_decrease_transaction(double, int);

        bool update_capacity(int);
        bool update_capacity_increase(double);
        bool update_capacity_decrease(double);

        void add_coordinates(BasicMessage*);

        std::string to_string(void);

    private:
        int connectedNodeId;
        int connectingEdge;
        int state;
        int weight;
        int numOfChildren;
        int children[1000];
        int capacity;
        bool edgeTowardsRoot;


        int lengthConnectedCoordinates;
        int connectedVectorCoordinates[1000];

        LinkCapacity *linkCapacity;
};

#endif

