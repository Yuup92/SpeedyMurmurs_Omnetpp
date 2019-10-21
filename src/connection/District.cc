#include "./District.h"

District::District() {
    numberOfConnections = 0;
}

void District::set_num_of_connections(int numConn) {
    numberOfConnections = numConn;
    for(int i = 0; i < numberOfConnections; i++) {
        linkCapacities[i].set_outgate(i);
        linkCapacities[i].set_current_capacity(100);
    }
}

void District::set_node_id(int id) {
    nodeId = id;
}

int District::get_node_id(void) {
    return nodeId;
}


int District::getNumSentCoordinateMsg(void) {
    return sentCoordinateMsg;
}

LinkCapacity * District::get_all_link_capacities(void) {
    return linkCapacities;
}

Neighbours * District::get_neighbourhood(int i) {
    return &neighbourhoods[i];
}

void District::set_num_of_neighbourhoods(int num) {
    numOfNeighbourhoods = num;
}

void District::update_linked_nodes_from_spanningtree(SpanningTree *&spanningTrees) {

    for(int i = 0; i < District::NUM_OF_TREES; i++) {
        int numLinkedNeighbours = spanningTrees[i].update_linked_nodes(neighbourhoods[i].get_linked_nodes());
        neighbourhoods[i].set_neighbourhood_index(i);
        neighbourhoods[i].set_node_id(nodeId);
        neighbourhoods[i].set_num_linked_nodes(numLinkedNeighbours);
        neighbourhoods[i].add_capacities_to_linked_nodes(linkCapacities, numberOfConnections);
        neighbourhoods[i].set_vector_coordinates(spanningTrees[i].get_length_vector_cor(), spanningTrees[i].get_vector_cor());
    }
}

void District::update_linked_nodes_from_file(void) {
    for(int i = 0; i < District::NUM_OF_TREES; i++) {
        neighbourhoods[i].add_capacities_to_linked_nodes(linkCapacities, numberOfConnections);
    }
}

int District::neighbours_coordinates_inquiry(void) {
    sentCoordinateMsg = 0;

    for(int i = 0; i < numOfNeighbourhoods; i++) {
        sentCoordinateMsg += neighbourhoods[i].send_coordinates_direct_neighbours(&msgBuf);
    }

    return sentCoordinateMsg;
}

void District::handle_message(BasicMessage *msg, int outgoingEdge) {
    int neighbourhood = msg->getNeighbourhoodIndex();
    neighbourhoods[neighbourhood].handle_message(msg, outgoingEdge);
}

int District::get_message_count(void) {
    return msgBuf.getMessageCount();
}

BufferedMessage * District::get_message(void) {
    return msgBuf.getMessage();
}
