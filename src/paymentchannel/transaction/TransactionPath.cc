#include "./TransactionPath.h"

TransactionPath::TransactionPath(){

}

void TransactionPath::sending_path(double a, LinkedNode *rN, int endN, int n, int transId) {
    amount = a;
    linkTowardsReceiver = rN;
    endNode = endN;
    neighbourhood = n;
    transactionId = transId;
    pathId = rand();
    executionRole = TransactionPath::SENDER;
    state = TransactionPath::QUERY_STATE;
}

void TransactionPath::forwarding_path(double a, LinkedNode *rN, LinkedNode *sN, int endN, int n, int tId, int pId) {
    amount = a;
    linkTowardsSender = sN;
    linkTowardsReceiver = rN;
    transactionId = tId;
    pathId = pId;
    endNode = endN;
    neighbourhood = n;
    executionRole = TransactionPath::FORWARDER;
    state = TransactionPath::JOINING_PATH;
}

void TransactionPath::set_forwarding_path(TransactionPath *path) {
    amount = path->get_amount();
    linkTowardsSender = path->get_link_towards_sender();
    linkTowardsReceiver = path->get_link_towards_receiver();
    transactionId = path->get_transaction_id();
    pathId = path->get_path_trans_id();
    endNode = path->get_end_node_id();
    neighbourhood = path->get_neighbourhood();
    executionRole = path->get_execution_role();
    state = path->get_state();
}

void TransactionPath::receiving_path(double a, LinkedNode *sN, int n, int tId, int pId) {
    amount = a;
    linkTowardsSender = sN;
    transactionId = tId;
    pathId = pId;
    neighbourhood = n;
    executionRole = TransactionPath::RECEIVER;
    state = TransactionPath::JOINED_PATH;
    // Make state
}

void TransactionPath::set_receiving_path(TransactionPath *path) {
    amount = path->get_amount();
    linkTowardsSender = path->get_link_towards_sender();
    transactionId = path->get_transaction_id();
    pathId = path->get_path_trans_id();
    neighbourhood = path->get_neighbourhood();
    executionRole = path->get_execution_role();
    state = path->get_state();
    // Make state
}

void TransactionPath::set_amount(double a) {
    amount = a;
}

double TransactionPath::get_amount(void) {
    return amount;
}

int TransactionPath::get_end_node_id(void) {
    return endNode;
}

void TransactionPath::set_end_node_id(int id)
{
    endNode = id;
}

double TransactionPath::get_path_trans_amount(void) {
    return amount;
}

void TransactionPath::set_path_trans_amount(double a) {
    amount = a;
}

int TransactionPath::get_transaction_id(void) {
    return transactionId;
}

void TransactionPath::set_transaction_id(int transId) {
    transactionId = transId;
}

int TransactionPath::get_path_trans_id(void) {
    return pathId;
}

void TransactionPath::set_path_trans_id(int pId) {
    pathId = pId;
}

int TransactionPath::get_neighbourhood(void) {
    return neighbourhood;
}

int TransactionPath::get_path_outgate_towards_receiver(void) {
    return linkTowardsReceiver->get_connecting_edge();
}

int TransactionPath::get_path_outgate_towards_sender(void) {
    return linkTowardsSender->get_connecting_edge();
}

int TransactionPath::get_execution_role(void) {
    return executionRole;
}

void TransactionPath::set_execution_role(int role) {

    if((role == TransactionPath::SENDER) or
       (role == TransactionPath::FORWARDER) or
       (role == TransactionPath::RECEIVER)) {
        executionRole = role;
    }
}

int TransactionPath::get_state(void) {
    return state;
}

void TransactionPath::set_state(int s) {
    state = s;
}

int TransactionPath::get_num_of_total_paths(void) {
    return numOfPathsForTransaction;
}

void TransactionPath::set_num_of_total_paths(int numOfPaths) {
    numOfPathsForTransaction = numOfPaths;
}

void TransactionPath::set_sending_link(LinkedNode *sN) {
    linkTowardsSender = sN;
}

LinkedNode * TransactionPath::get_link_towards_sender(void) {
    return linkTowardsSender;
}

void TransactionPath::set_receiving_link(LinkedNode *rN) {
    linkTowardsReceiver = rN;
}

LinkedNode * TransactionPath::get_link_towards_receiver(void) {
    return linkTowardsReceiver;
}
