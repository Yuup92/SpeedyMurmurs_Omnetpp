#ifndef DISTRICT_H_
#define DISTRICT_H_


#include "./LinkedNode.h"
#include "./Neighbours.h"
#include "./capacity/LinkCapacity.h"
#include "../alg/spanning_tree/SpanningTree.h"
#include "../msg/outgoing_buf/MessageBuffer.h"
#include "../msg/outgoing_buf/BufferedMessage.h"


class District {

    public:
        static const int NUM_OF_TREES = 10;
        static const int MAXCONNECTIONS = 100;


        District();

        void set_num_of_connections(int);

        void set_node_id(int);
        int get_node_id(void);

        LinkCapacity * get_all_link_capacities(void);
        Neighbours * get_neighbourhood(int);

        void set_num_of_neighbourhoods(int);

        void update_linked_nodes_from_spanningtree(SpanningTree *&);
        void update_linked_nodes_from_file(void);
        void neighbours_coordinates_inquiry(void);

        // Message handling
        void handle_message(BasicMessage *, int);
        int get_message_count(void);
        BufferedMessage * get_message(void);

    private:

        int nodeId;
        int numOfNeighbourhoods;
        int numberOfConnections;

        MessageBuffer msgBuf;

        LinkCapacity linkCapacities[MAXCONNECTIONS];
        Neighbours neighbourhoods[NUM_OF_TREES];

};

#endif
