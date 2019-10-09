#include "./PaymentChannel.h"

PaymentChannel::PaymentChannel(){
    nodeId = -1;

    msgDelay = 0;
    currentTransactionId = -1;
    currentState = PaymentChannel::STATE_WAITING;
    transactionConnectionIndex = 0;

    numberOfCompletedTransactions = 0;
    numberOfForwardedTransactions = 0;
    numberOfForwardedTransactionsCompleted = 0;
    numberOfTotalTransactions = 0;

    numberOfCapacityErrors = 0;
    numberOfReceivedCapacityErrors = 0;

    numberOfTimeoutErrors = 0;
    numberOfReceivedTimeoutErrors = 0;

    numberOfErrors = 0;
    numberOfReceivedErrors = 0;

    currentNeighbour = 0;
}

void PaymentChannel::set_node_id(int id) {
    nodeId = id;
}

// TODO add this type of functionality
void PaymentChannel::set_concurrency_type(int concurrency) {
    concurrencyType = concurrency;
}

//void PaymentChannel::set_connected_neighbours(Neighbours *n) {
//    connectedNeighbours = n;
//}

void PaymentChannel::set_district(District *d) {
    district = d;
}

District * PaymentChannel::get_district(void){
    return district;
}

int PaymentChannel::get_current_transaction_id(void) {
    return currentTransactionId;
}

int PaymentChannel::get_current_transaction_index(void) {
    return transactionConnectionIndex;
}

int PaymentChannel::get_num_completed_transactions(void) {
    return numberOfCompletedTransactions;
}

int PaymentChannel::get_num_forwarded_transactions(void) {
    return numberOfForwardedTransactions;
}

int PaymentChannel::get_num_of_total_transactions(void) {
    return numberOfTotalTransactions;
}

int PaymentChannel::get_num_forwarded_completed_transactions(void) {
    return numberOfForwardedTransactionsCompleted;
}

int PaymentChannel::get_failed_transactions(void) {
    return numberOfErrors;
}

int PaymentChannel::get_capacity_failure(void) {
    return numberOfCapacityErrors;
}

double PaymentChannel::get_network_delay(void) {
    return latency.get_network_delay();
}

double PaymentChannel::get_os_delay(void) {
   return latency.get_os_delay();
}

double PaymentChannel::get_crypto_delay(void) {
    return latency.get_crypto_delay();
}

std::string PaymentChannel::send(int endNode, double amount) {

    // Get a random Neighbourhood
    std::string res = "";
    int amountOfPaths = 1;

    res = transactions.add_send_transaction(district, &msgBuf, amount, endNode, amountOfPaths);

    transactionConnectionIndex++;
    numberOfTotalTransactions++;

    return res;
}

std::string PaymentChannel::multi_path_send(int endNode, double amount, int numberOfPaths) {

    int transactionId = rand();

    // TODO make this a double
    int partialAmount = amount/numberOfPaths;

    std::string res = "";
    res = transactions.add_send_transaction(district, &msgBuf, amount, endNode, numberOfPaths);

    numberOfTotalTransactions++;
    return res;
}

std::string PaymentChannel::handle_message(BasicMessage *msg, int outgoingEdge) {

    std::string res = "";

    if(msg->getSubType() == TransactionMsg::INI_PATH_QUERY) {
        handle_query_message(outgoingEdge, msg->getTransactionId(), msg->getPathTransactionId(),
                             msg->getEndNodeId(), msg->getAmount(),
                             msg->getNeighbourhoodIndex());
    } else if(msg->getSubType() == TransactionMsg::CAPACITY_ERROR) {

    } else if(msg->getSubType() == TransactionMsg::TIMEOUT) {

    } else if(msg->getSubType() == TransactionMsg::ERROR) {

    }  else {
        Transaction *trans = transactions.get_transaction(msg->getTransactionId());

        int role = trans->get_execution_role();
        int pathId = msg->getPathTransactionId();

        if(role == TransactionPath::SENDER) {
            trans->update_sender(&msgBuf, pathId);
        } else if(role == TransactionPath::RECEIVER) {
            trans->update_receiver(&msgBuf, pathId);
        } else if(role == TransactionPath::FORWARDER) {
            res = trans->update_forwarder(&msgBuf, pathId);
        } else {
            // TODO
            // ERROR
        }
    }

    return res;
}

void PaymentChannel::handle_query_message(int outgoingEdge, int transId, int pathTransId, int nId, double amount, int neighbourhood) {
    // Message has found receiver
    if(nodeId == nId) {

        LinkedNode *sN = district->get_neighbourhood(neighbourhood)->get_downstream_linked_node(outgoingEdge);

        TransactionPath transPath;
        transPath.receiving_path(amount, sN, neighbourhood, transId, pathTransId);

        transactions.add_receiving_transaction(&msgBuf, transId, &transPath);
        transactionConnectionIndex++;
        numberOfTotalTransactions++;

        // handle next transaction msg

    // Message needs to be forwarded
    } else {
        LinkedNode *sN = district->get_neighbourhood(neighbourhood)->get_downstream_linked_node(outgoingEdge);
        LinkedNode *rN = district->get_neighbourhood(neighbourhood)->get_upstream_linked_node(nId, amount);

        if(amount < rN->get_virtual_capacity()) {
            TransactionPath transPath;
            transPath.forwarding_path(amount, rN, sN, nId, neighbourhood, transId, pathTransId);
            transactions.add_forward_transaction(&msgBuf, transId, &transPath);

            transactionConnectionIndex++;
            numberOfTotalTransactions++;
            numberOfForwardedTransactions++;

            return;
        } else {
            //TODO Cancel this transaction
            //update_message_buf(PaymentChannel::capacity_error(transactionId, pathTransId), senderEdge);
            return;
        }
        // TODO check if path has enough capacity to send futher
        // TODO check if transaction ID doesnt already exist
        //forward_send(transId, pathTransId, nId, amount, outgoingEdge, neighbourhood);
    }
}

int PaymentChannel::get_message_count(void) {
    return msgBuf.getMessageCount();
}

BufferedMessage * PaymentChannel::get_message(void) {
    return msgBuf.getMessage();
}

// TODO add msgDelay to the bufferedmessage process
//  or add it later to the game somehow the way it works now
//  is a little shitty
void PaymentChannel::update_message_buf(BasicMessage *msg, int outgoingEdge) {
    msgDelay = latency.calculate_delay_ms(true);
    // msgDelay = msgDelay + 0.5;
    BufferedMessage * bufMsg = new BufferedMessage(msg, outgoingEdge, msgDelay);
    msgBuf.addMessage(bufMsg);
}

int PaymentChannel::get_trans_conn_index(int transId, int pathTransId) {
    if(transactionConnectionIndex > 0) {
        for(int i = 0; i < transactionConnectionIndex; i++) {
            if(transactionConnections[i].transId == transId and
                    transactionConnections[i].pathTransId == pathTransId) {
                return i;
            }
        }
    }
    return -1;
}

void PaymentChannel::remove_transaction_via_index(int index) {
    if(index < 0) {
        return;
    }

    for(int i = index; i < transactionConnectionIndex; i++) {
        if((i+1) < transactionConnectionIndex) {
            transactionConnections[i] = transactionConnections[i+1];
        }
    }
    transactionConnectionIndex--;
}

void PaymentChannel::remove_transaction(int transId) {
    int i = 0;
    while(i < transactionConnectionIndex) {
        if(transactionConnections[i].transId == transId) {
            remove_transaction_via_index(i);
        } else {
            i++;
        }
    }
}

std::string PaymentChannel::to_string(void) {

    LinkedNode *node = district->get_neighbourhood(currentNeighbour)->get_upstream_linked_node(21, 10);

    return node->to_string(); // connectedNeighbours[0].to_string();
}

std::string PaymentChannel::capacities_to_string(void) {

    // bool checkCapacity = connectedNeighbours[currentNeighbour].check_capacity(endNode, amount);

    return district->get_neighbourhood(0)->get_all_capacities();
}

std::string PaymentChannel::delay_to_string(void) {
    return std::to_string(msgDelay);
}


