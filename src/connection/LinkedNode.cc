#include "./LinkedNode.h"

LinkedNode::LinkedNode(){
    numOfChildren = 0;

    sentConnectedCoordinates = false;
    receivedConnectedCoordinates = false;
}

void LinkedNode::update_node(int outgoingEdge, int s, int w, int numChild, int *childList, bool _edgeTowardsRoot) {
    connectingEdge = outgoingEdge;
    state = s;
    weight = w;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = *(childList + i);
    }
    edgeTowardsRoot = _edgeTowardsRoot;

}

void LinkedNode::update_node(int outgoingEdge, int s, int w, int numChild, int *childList, bool _edgeTowardsRoot, LinkCapacity *capacity) {
    connectingEdge = outgoingEdge;
    state = s;
    weight = w;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = *(childList + i);
    }
    linkCapacity = capacity;
    edgeTowardsRoot = _edgeTowardsRoot;
}

void LinkedNode::update_node(int outgoingEdge, int numChild, int *childList, bool _edgeTowardsRoot, int capacity) {
    connectingEdge = outgoingEdge;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = *(childList + i);
    }
    //
    linkCapacity->set_current_capacity(capacity);
    edgeTowardsRoot = _edgeTowardsRoot;
}

void LinkedNode::update_node(int outgoingEdge, int outgoingEdgeId, int numChild, int *childList, bool _edgeTowardsRoot) {
    connectingEdge = outgoingEdge;
    connectedNodeId = outgoingEdgeId;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = childList[i];
    }
    edgeTowardsRoot = _edgeTowardsRoot;
}

void LinkedNode::set_connected_node_id(int nodeId) {
    connectedNodeId = nodeId;
}

int LinkedNode::get_connected_node_id(void) {
    return connectedNodeId;
}

void LinkedNode::set_connecting_edge(int outgoingEdge) {
    connectingEdge = outgoingEdge;
}

int LinkedNode::get_connecting_edge(void) {
    return connectingEdge;
}

void LinkedNode::set_state(int s) {
    state = s;
}

int LinkedNode::get_state(void) {
    return state;
}

void LinkedNode::set_weight(int w) {
    weight = w;
}

int LinkedNode::get_weight(void) {
    return weight;
}

void LinkedNode::set_number_of_children(int numChild) {
    numOfChildren = numChild;
}

int LinkedNode::get_number_of_children(void) {
    return numOfChildren;
}

void LinkedNode::set_children(int *childList) {
    for(int i = 0; i < numOfChildren; i++) {
        children[i] = *(childList + i);
    }
}

int * LinkedNode::get_children(void) {
    return &(children[0]);
}

int LinkedNode::get_child(int i) {
    return children[i];
}

void LinkedNode::set_capacity(LinkCapacity *capacity) {
    linkCapacity = capacity;
}

double LinkedNode::get_capacity(void) {
    return linkCapacity->get_current_capacity();
}

double LinkedNode::get_virtual_capacity(void) {
    return linkCapacity->get_current_virtual_capacity();
}

void LinkedNode::set_sent_coordinate_bool(bool r) {
    sentConnectedCoordinates = r;
}

int LinkedNode::process_transaction(int transactionAmount) {
    if((capacity - transactionAmount) > 0){
        capacity = capacity - transactionAmount;
        return capacity;
    } else {
        return -1;
    }
}

bool LinkedNode::get_edge_towards_root(void) {
    return edgeTowardsRoot;
}

void LinkedNode::set_edge_towards_root(bool edge) {
    edgeTowardsRoot = edge;
}

bool LinkedNode::pend_increase_transaction(double amount, int transId) {
    return linkCapacity->add_pending_transaction_increase(amount, transId);
}

bool LinkedNode::pend_decrease_transaction(double amount, int transId) {
    return linkCapacity->add_pending_transaction_decrease(amount, transId);
}

bool LinkedNode::update_capacity(int transId) {
    bool updated_capacity =  linkCapacity->complete_transaction(transId);
    return updated_capacity;
}

bool LinkedNode::update_capacity_increase(double amount) {
    return linkCapacity->update_increase(amount);
}

bool LinkedNode::update_capacity_decrease(double amount) {
    return linkCapacity->update_decrease(amount);
}

void LinkedNode::add_coordinates(BasicMessage *msg) {
    lengthConnectedCoordinates = msg->getLengthVectorCordinates();

    for(int i = 0; i < lengthConnectedCoordinates; i++) {
        connectedVectorCoordinates[i] = msg->getVectorCordinates(i);
    }
    receivedConnectedCoordinates = true;

}

std::string LinkedNode::to_string(void) {

    std::string res = "";

    char buff[300];

    sprintf(buff, "OUTGOINGEDGE:%d\n"
                  "LINKEDNODECONNECTEDTO:%d\n"
                  "NUMBEROFCHILDREN:%d\n"
                  "CAPACITY:%.2f\n"
                  "CAPACITYID:%d\n", connectingEdge, connectedNodeId, numOfChildren, linkCapacity->get_current_capacity(), linkCapacity->get_connected_node_id());
    res = res + buff;

    char buf[300];
    if(numOfChildren < 1) {
        res += "NOCHILDREN";
    } else {
        res += "CHILDREN:";
        for(int i = 0; i < numOfChildren; i++) {
            res += std::to_string(children[i]);
            res += ";";
        }
    }
    res += "\n";

    if(edgeTowardsRoot) {
        res += "EDGEROOT\n";
    } else {
        res += "EDGENOTROOT\n";
    }

    return res;

}
