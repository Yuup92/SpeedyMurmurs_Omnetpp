#ifndef NEIGHBOURS_H__
#define NEIGHBOURS_H__

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "../msg/outgoing_buf/MessageBuffer.h"
#include "../msg/outgoing_buf/BufferedMessage.h"
#include "./LinkedNode.h"

class Neighbours
{
    public:

        static const int MESSAGE_TYPE = 5;

        static const int REQUEST_COORDINATES = 1;
        static const int SEND_COORDINATES = 2;

        Neighbours();
        void handle_message(BasicMessage*, int);
        void handle_request_coordinates();
        void handle_send_coordinates(BasicMessage*);

        void set_neighbourhood_index(int);
        int get_neighbourhood_index(void);

        void set_node_id(int);
        int get_node_id(void);

        int send_coordinates_direct_neighbours(MessageBuffer*);

        void set_number_of_neighbours(int out_gate_size);

        int get_amount_of_neighbours();
        void remove_neighbour_from_random_gate_list(int);

        void update_linked_nodes(int, int, int, int, int, int *, bool);
        void update_linked_nodes(int, int, int, int, int *, bool);
        LinkedNode * get_linked_nodes(void);
        LinkedNode * get_upstream_linked_node(int, double);
        LinkedNode * get_downstream_linked_node(int);

        void set_linked_node_updates(bool);
        bool is_linked_nodes_updated(void);

        void set_num_linked_nodes(int);
        int get_num_linked_nodes(void);

        int get_index_linked_nodes(int);
        int get_outgoing_edge_transaction(int, int);

        void set_vector_coordinates(int, int*);
        int * get_vector_coordinates(void);
        int get_length_neighbourhood_coordinates(void);


        void add_capacities_to_linked_nodes(LinkCapacity*, int);

        bool check_capacity(int, double);

        std::string get_save_state(void);

        std::string to_string(void);
        std::string get_all_capacities(void);

    private:

        int neighbourhoodIndex;
        int nodeId;

        int num_of_neighbours;
        int num_of_neighbours_rand;

        bool linkedNodesUpdated;
        int numLinkedNodes;
        LinkedNode linkedNodes[1000];
        int lengthNeighbourhoodCor;
        int neighbourhoodCoordinates[2000];

        static BasicMessage * request_coordinates(int, int);
        static BasicMessage * send_coordinates(int, int, int, int*);
};

#endif
