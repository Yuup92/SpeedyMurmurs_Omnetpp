#include "./LinkCapacity.h"

LinkCapacity::LinkCapacity() {
    capacity = 0;
    virtualCapacity = capacity;
    numOfPayments = 0;
    numOfPaymentsPending = 0;
}

LinkCapacity::LinkCapacity(int cap) {
    capacity = cap;
    virtualCapacity = cap;
    numOfPayments = 0;
    numOfPaymentsPending = 0;
}

void LinkCapacity::set_outgate(int outg) {
    outgate = outg;
}

int LinkCapacity::get_outgate(void) {
    return outgate;
}

void LinkCapacity::set_connected_node_id(int nodeId) {
    connectedNodeId = nodeId;
}

int LinkCapacity::get_connected_node_id(void) {
    return connectedNodeId;
}

void LinkCapacity::set_current_capacity(double cap) {
    capacity = cap;
    virtualCapacity = cap;
    return;
}

double LinkCapacity::get_current_capacity(void) {
    return capacity;
}

bool LinkCapacity::check_capacity(double amount) {
    int newVirtualCapacity = virtualCapacity - amount;
    if(newVirtualCapacity > 0) {
        return true;
    } else {
        return false;
    }
}


double LinkCapacity::get_current_virtual_capacity(void) {
    return virtualCapacity;
}

bool LinkCapacity::pend_transaction_upstream(double amount) {
    int newVirtualCapacity = virtualCapacity - amount;

    if(newVirtualCapacity > 0) {
        virtualCapacity = newVirtualCapacity;
        numOfPaymentsPending++;
        return true;
    }
    return false;
}

void LinkCapacity::complete_transaction_downstream(double amount) {
    capacity = capacity + amount;
    virtualCapacity = virtualCapacity + amount;
    numOfPayments++;
}

bool LinkCapacity::complete_transaction_upstream(double amount) {
    int newCapacity = capacity - amount;
    if(newCapacity > 0) {
        capacity = newCapacity;
        numOfPaymentsPending--;
        numOfPayments++;
        return true;
    } else {
        virtualCapacity = virtualCapacity + amount;
        //numOfPaymentsPending--;
        return false;
    }
}

void LinkCapacity::cancel_pend(double amount) {
    virtualCapacity = virtualCapacity + amount;
    numOfPaymentsPending--;
    if(numOfPaymentsPending == 0) {
        virtualCapacity = capacity;
    }
}

void LinkCapacity::add_capacity(double amount) {
    capacity = capacity + amount;
    numOfPayments--;
}

void LinkCapacity::remove_capacity(double amount) {
    capacity = capacity - amount;
    numOfPayments--;
}

void LinkCapacity::remove_pending_transaction_index(int index) {
    if(numOfPaymentsPending == 1) {

        numOfPaymentsPending = 0;
        return;
    } else if(index == numOfPaymentsPending) {
        numOfPaymentsPending--;
    } else {
        for(int i = index; i < numOfPaymentsPending; i++) {
            if( (i+1) < numOfPaymentsPending) {
                pendingPayments[i] = pendingPayments[i + 1];
            }

        }
        numOfPaymentsPending--;
    }
}

void LinkCapacity::remove_pending_transaction_transaction_id(int transId) {

}

bool LinkCapacity::update_increase(double amount) {
    double cap = capacity + amount;

    if(cap > -1) {
        capacity = cap;
        return true;
    } else {
        return false;
    }
}

bool LinkCapacity::update_decrease(double amount) {
    double cap = capacity - amount;

    if(cap > -1) {
        capacity = cap;
        return true;
    } else {
        return false;
    }
}


std::string LinkCapacity::to_file(void) {
    std::string res = std::to_string(connectedNodeId) + ",";
    res += std::to_string(capacity) + ",";
    res += std::to_string(virtualCapacity) + ",";

    res += std::to_string(numOfPayments) + ",";
    res += std::to_string(numOfPaymentsPending) + ",";


    return res;
}
