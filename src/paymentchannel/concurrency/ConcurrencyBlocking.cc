#include "../../paymentchannel/concurrency/ConcurrencyBlocking.h"

ConcurrencyBlocking::ConcurrencyBlocking(void) {
    currentlyBlocking = false;
    idTransactionBlocking = -1;
    nodeIdBlocking = -1;
    currentEdgeDirection = -1;
}

void ConcurrencyBlocking::update_concurrency(bool block, int transId, int nodeId, int outgoingEdge, int amount) {
    currentlyBlocking = block;
    idTransactionBlocking = transId;
    nodeIdBlocking = nodeId;
    currentEdgeDirection = outgoingEdge;
    amount = amount;
}

bool ConcurrencyBlocking::is_blocked(void) {
    return currentlyBlocking;
}

void ConcurrencyBlocking::release(void) {
    currentlyBlocking = false;
    idTransactionBlocking = -1;
    nodeIdBlocking = -1;
}

int ConcurrencyBlocking::get_transaction_id(void) {
    return idTransactionBlocking;
}

int ConcurrencyBlocking::get_node_id(void) {
    return nodeIdBlocking;
}

int ConcurrencyBlocking::get_outgoing_edge(void) {
    return currentEdgeDirection;
}

int ConcurrencyBlocking::get_amount(void) {
    return amount;
}
void ConcurrencyBlocking::release_concurrency(void) {
    currentlyBlocking = false;
    idTransactionBlocking = -1;
    nodeIdBlocking = -1;
    currentEdgeDirection = -1;
}
