#include "./PaymentChannel.h"

PaymentChannel::PaymentChannel(){
    nodeId = -1;

    msgDelay = 0;
    currentTransactionId = 0;
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

void PaymentChannel::set_stats(Statistics *s) {
    stats = s;
    transactions.set_stats(s);
}

void PaymentChannel::set_district(District *d) {
    district = d;
}

District * PaymentChannel::get_district(void){
    return district;
}

int PaymentChannel::get_current_transactions_amount(void) {
    return transactions.get_current_trans();
}

int PaymentChannel::get_current_transaction_size(void) {
    return transactions.get_trans_map_size();
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

void PaymentChannel::check_for_dead_transactions(void) {
    bool rm = transactions.remove_dead_transactions();
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

    std::string res = "";
    res = transactions.add_send_transaction(district, &msgBuf, amount, endNode, numberOfPaths);

    transactionConnectionIndex++;
    numberOfTotalTransactions++;
    return res;
}

std::string PaymentChannel::handle_message(BasicMessage *msg, int outgoingEdge) {
    std::string res = "";

    // First time transaction coming in
    if(msg->getSubType() == TransactionMsg::INI_PATH_QUERY) {
        handle_query_message(outgoingEdge, msg->getTransactionId(), msg->getPathTransactionId(),
                             msg->getEndNodeId(), msg->getAmount(),
                             msg->getNeighbourhoodIndex());

    } else {
        // Checking if transaction already has been mapped
        if(transactions.check_for_existing_transaction(msg->getTransactionId())) {
            Transaction *trans = transactions.get_transaction_from_map(msg->getTransactionId());
            if(trans->get_state() == Transaction::TRANSACTION_ALIVE) {
                if((msg->getSubType() >> 6) == (TransactionMsg::ERROR >> 6)) {
                   res += trans->report_error(&msgBuf, msg, msg->getPathTransactionId(), district);
               } else if(trans->get_role() == TransactionPath::SENDER) {
                   res += trans->update_sender(&msgBuf, msg->getPathTransactionId(), district);
               } else if(trans->get_role() == TransactionPath::FORWARDER) {
                   res += trans->update_forwarder(&msgBuf, msg->getPathTransactionId());
               } else if(trans->get_role() == TransactionPath::RECEIVER) {
                   res += trans->update_receiver(&msgBuf, msg->getPathTransactionId());
               }
            } else {
                res += "Transaction is dead";
            }

        } else {
            // Do something? Do nothing?
            res += "Could not find transaction in map";
            return res;
        }
    }
    bool rm = transactions.remove_dead_transactions();

    return res;
}

void PaymentChannel::handle_query_message(int outgoingEdge, int transId, int pathId, int nId, double amount, int neighbourhood) {
    // Message has found receiver
    if(nodeId == nId) {
        LinkedNode *sN = district->get_neighbourhood(neighbourhood)->get_downstream_linked_node(outgoingEdge);

        TransactionPath transPath;
        transPath.receiving_path(amount, sN, neighbourhood, transId, pathId);
        transactions.new_receiving_transaction(&msgBuf, transId, &transPath);

        transactionConnectionIndex++;
        numberOfTotalTransactions++;

    // Message needs to be forwarded
    } else {
        LinkedNode *receivingNode = district->get_neighbourhood(neighbourhood)->get_upstream_linked_node(nId, amount);
        LinkedNode *sendingNode = district->get_neighbourhood(neighbourhood)->get_downstream_linked_node(outgoingEdge);

        LinkCapacity *c = receivingNode->get_link_capacity();
        if(c->check_capacity(amount)) {

            TransactionPath transPath;
            transPath.forwarding_path(amount, receivingNode, sendingNode, nId, neighbourhood, transId, pathId);

            if(transactions.check_for_existing_transaction(transId)) {
                transactions.add_path_to_transaction_id(&msgBuf, transId, &transPath);
            } else {
                transactions.new_forwarding_transaction(&msgBuf, transId, &transPath);
            }

            transactionConnectionIndex++;
            numberOfTotalTransactions++;
            numberOfForwardedTransactions++;

            return;
        } else {
            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::capacity_error(transId, pathId);
            BufferedMessage * bufMsg = new BufferedMessage(m, outgoingEdge, msgDelay);
            msgBuf.addMessage(bufMsg);
            return;
        }
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


