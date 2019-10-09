#include "./Neighbours.h"


Neighbours::Neighbours()
{
    num_of_neighbours = 0;
    num_of_neighbours_rand = 0;
    linkedNodesUpdated = false;
    numLinkedNodes = 0;
}

void Neighbours::handle_message(BasicMessage  *msg, int outgoingEdge) {

    if(msg->getSubType() == Neighbours::SEND_COORDINATES) {
        handle_send_coordinates(msg);
    }
}


void Neighbours::handle_send_coordinates(BasicMessage *msg) {

    for(int i = 0; i < numLinkedNodes; i++) {
        if(msg->getNodeId() == linkedNodes[i].get_connected_node_id()) {
            linkedNodes[i].add_coordinates(msg);
        }
    }

}

void Neighbours::set_neighbourhood_index(int n) {
    neighbourhoodIndex = n;
}

int Neighbours::get_neighbourhood_index(void) {
    return neighbourhoodIndex;
}

void Neighbours::set_node_id(int id) {
    nodeId = id;
}

int Neighbours::get_node_id(void) {
    return nodeId;
}


void Neighbours::send_coordinates_direct_neighbours(MessageBuffer *msgBuf) {

    for(int i = 0; i < numLinkedNodes; i++) {
        int outgate = linkedNodes[i].get_connecting_edge();
        BasicMessage *m = Neighbours::send_coordinates(nodeId, neighbourhoodIndex, lengthNeighbourhoodCor, &neighbourhoodCoordinates[0]);
        BufferedMessage * bufMsg = new BufferedMessage(m, outgate, 1);
        msgBuf->addMessage(bufMsg);
    }

}

// Maybe add a bool to reset out_gate_list or something
void Neighbours::set_number_of_neighbours(int out_gate_size)
{
    num_of_neighbours = out_gate_size;
    num_of_neighbours_rand = out_gate_size;
}

void Neighbours::update_linked_nodes(int index, int outgoingEdge, int state, int weight, int numChild, int *childList, bool edgeTowardsRoot) {
    linkedNodes[index].update_node(outgoingEdge, state, weight, numChild, childList, edgeTowardsRoot);
}

void Neighbours::update_linked_nodes(int index, int outgoingEdge, int outgoingEdgeId, int numChild, int *childList, bool edgeTowardsRoot) {
    linkedNodes[index].update_node(outgoingEdge, outgoingEdgeId, numChild, childList, edgeTowardsRoot);
}

LinkedNode * Neighbours::get_linked_nodes() {
    return &linkedNodes[0];
}

void Neighbours::set_linked_node_updates(bool updated) {
    linkedNodesUpdated = updated;
}

bool Neighbours::is_linked_nodes_updated(void) {
    return linkedNodesUpdated;
}

void Neighbours::set_num_linked_nodes(int amount) {
    numLinkedNodes = amount;
}

int Neighbours::get_num_linked_nodes(void) {
    return numLinkedNodes;
}

int Neighbours::get_index_linked_nodes(int edge) {

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_connecting_edge() == edge) {
            return i;
        }
    }

}

void Neighbours::set_vector_coordinates(int length, int *vectorList) {
    for(int i = 0; i < length; i ++) {
        neighbourhoodCoordinates[i] = vectorList[i];
    }
    lengthNeighbourhoodCor = length;
}

int* Neighbours::get_vector_coordinates(void) {
    return &neighbourhoodCoordinates[0];
}

int Neighbours::get_length_neighbourhood_coordinates(void) {
    return lengthNeighbourhoodCor;
}

int Neighbours::get_outgoing_edge_transaction(int nodeId, int amount) {
    int indexParent;
    int edgeTowardsTransaction = -1;
    bool edgeFound = false;

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                edgeFound = true;
                if(check_capacity(i, amount)) {
                    edgeTowardsTransaction = linkedNodes[i].get_connecting_edge();
                      return edgeTowardsTransaction;
                  }
            }
        }
    }

    return edgeTowardsTransaction;
}

// TODO
// if amount is not possible do something
// Add check for shortcuts
LinkedNode * Neighbours::get_upstream_linked_node(int nodeId, double amount) {
    int indexParent;
    int edgeTowardsTransaction = 0;
    bool edgeFound = false;

    //TODO
    // Add shortcuts

    if(numLinkedNodes == 1) {
        return &linkedNodes[0];
    }

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                return &linkedNodes[i];
            }
        }
    }

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_edge_towards_root()) {
            return &linkedNodes[i];
        }
    }


    // return node;
}

// TODO build in safety
LinkedNode * Neighbours::get_downstream_linked_node(int outgoingEdge) {

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_connecting_edge() == outgoingEdge) {
            return &linkedNodes[i];
        }
    }

}

void Neighbours::add_capacities_to_linked_nodes(LinkCapacity *linkCapacities, int numberOfNodes) {
    for(int i = 0; i < numLinkedNodes; i++) {
        for(int k = 0; k < numberOfNodes; k++) {
            if(linkCapacities[k].get_outgate() == linkedNodes[i].get_connecting_edge()) {
                linkedNodes[i].set_capacity(&linkCapacities[k]);
                linkCapacities[k].set_connected_node_id(linkedNodes[i].get_connected_node_id());
            }
        }
    }
}

bool Neighbours::check_capacity(int nodeId, double amount) {

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                if(linkedNodes[i].get_capacity() > amount) {
                    return true;
                }
            }
        }
    }

    return false;

}

std::string Neighbours::get_save_state(void) {
    std::string res = "";
    return res;
}

std::string Neighbours::to_string(void) {
    std::string res = "";

    res += "LINKEDNODES:" + std::to_string(numLinkedNodes) + "\n";

    for(int i = 0; i < numLinkedNodes; i++) {
        res += "####\n";
        res += linkedNodes[i].to_string();
    }

    return res;
}

std::string Neighbours::get_all_capacities(void) {

    std::string res = "";

    res += "Number of linked nodes: " + std::to_string(numLinkedNodes) + "\n";

    for(int i = 0; i < numLinkedNodes; i++) {
        res = res + "linked node id: " + std::to_string(linkedNodes[i].get_connected_node_id()) + "\n";
        res = res + "  capacity: " + std::to_string(linkedNodes[i].get_capacity()) + "\n";
        res = res + " to_string: " + linkedNodes[i].to_string();
    }

    return res;

}

BasicMessage * Neighbours::send_coordinates(int curNodeId, int neighbourhoodIndex, int vectorLength, int *vectorList) {
    char msgname[40];
    sprintf(msgname, "Send Coordinates");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Neighbours::MESSAGE_TYPE);
    msg->setSubType(Neighbours::SEND_COORDINATES);

    msg->setSpanningTreeIndex(neighbourhoodIndex);
    msg->setNodeId(curNodeId);

    msg->setLengthVectorCordinates(vectorLength);
    for(int i = 0; i < vectorLength; i++) {
        msg->setVectorCordinates(i, vectorList[i]);
    }

    return msg;
}











