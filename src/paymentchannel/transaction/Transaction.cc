#include "./Transaction.h"

Transaction::Transaction(){
    numOfAddedPaths = 0;
    lengthComplementNArray = 0;

    receivedAcceptedPaths = 0;

    receivedPendingPaths = 0;
    receivedPendRequests = 0;
}

std::string Transaction::start_transaction(District *dis, MessageBuffer *msg, double amount, int endNode, int pathAmount) {
    //Get list of maximum paths

    std::string res = "";

    //TODO make maxNumOfTrees not a number
    // make a config file or something
    int maxNumOfTrees = 10;
    numOfPaths = pathAmount;
    transId = rand();
    double amount_per_path = amount / numOfPaths;

    RandHelper::rand_set_and_complement_list(setNeighbourhood, complementNeighbourhood, maxNumOfTrees, numOfPaths);
    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    bool pathsToBeChecked = true;
    int attempts = 0;
    numOfAddedPaths = 0;
    receivedAcceptedPaths = 0;


    while((numOfAddedPaths < numOfPaths) and pathsToBeChecked) {

       int randNeighbourhood = setNeighbourhood[numOfAddedPaths];

       LinkedNode *n = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
       if(amount_per_path < n->get_capacity()) {
           transactionPath[numOfAddedPaths].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
           numOfAddedPaths++;
       } else {
            attempts++;
            if(lengthComplementNArray > 0) {
                setNeighbourhood[numOfAddedPaths] = complementNeighbourhood[0];
                for(int i = 0; i < (lengthComplementNArray - 1); i++) {
                        complementNeighbourhood[i] = complementNeighbourhood[i+1];
                }
            }
            if(attempts > (maxNumOfTrees - numOfPaths)) {
                pathsToBeChecked = false;
            }
       }

        // Debugging
        attempts++;
        if(attempts > (maxNumOfTrees - numOfPaths)) {
            pathsToBeChecked = false;
        }
    }


    if(attempts > (maxNumOfTrees - numOfPaths)) {
        return "false";
    // TODO
    // Consider where else messages can be added
    } else {
        for(int i = 0; i < numOfPaths; i++) {
            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[i]);
            BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[i].get_path_outgate_towards_receiver(), msgDelay);
            msg->addMessage(bufMsg);
            res += "Added a message";
        }
        executionRole = TransactionPath::SENDER;
        state = TransactionPath::QUERY_STATE;

    }
    return res;
}

std::string Transaction::forward_transaction(MessageBuffer *msg, int tId, TransactionPath *transPath) {
        std::string res = "";

        transId = tId;
        transactionPath[numOfAddedPaths].set_forwarding_path(transPath);

        double msgDelay = latency.calculate_delay_ms(true);
        BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[numOfAddedPaths]);
        BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[numOfAddedPaths].get_path_outgate_towards_receiver(), msgDelay);
        msg->addMessage(bufMsg);
        res += "Added a message";

        numOfAddedPaths++;

        executionRole = TransactionPath::FORWARDER;
        state = TransactionPath::JOINING_PATH;
        return res;
}

std::string Transaction::receiving_transaction(MessageBuffer *msg, int tId, TransactionPath *transPath) {
    std::string res = "";
    transId = tId;
    transactionPath[numOfAddedPaths].set_receiving_path(transPath);

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::path_request_accept(&transactionPath[numOfAddedPaths]);
    BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[numOfAddedPaths].get_path_outgate_towards_sender(), msgDelay);
    msg->addMessage(bufMsg);
    res += "Added a message";

    numOfAddedPaths++;

    executionRole = TransactionPath::RECEIVER;

    return res;
}

std::string Transaction::update_sender(MessageBuffer *msg, int pathId) {
    std::string res = "";

    int path = get_transaction_path_index(pathId);

    if(path < 0) {
        res += "Error error";
        return res;
    }

    if(transactionPath[path].get_state() == TransactionPath::QUERY_STATE) {
        receivedAcceptedPaths++;
        if(receivedAcceptedPaths == numOfAddedPaths) {
            for(int i = 0; i < numOfPaths; i++) {
                transactionPath[i].set_num_of_total_paths(numOfPaths);
                transactionPath[i].set_state(TransactionPath::PEND_TRANSACTION);

                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::pend_transaction_request(&transactionPath[i]);
                BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[i].get_path_outgate_towards_receiver(), msgDelay);
                msg->addMessage(bufMsg);
                res += "Added a message";
            }
        }

    } else if(transactionPath[path].get_state() == TransactionPath::PEND_TRANSACTION) {

        transactionPath[path].set_state(TransactionPath::PUSH_TRANSACTION);
        double msgDelay = latency.calculate_delay_ms(true);
        BasicMessage *m = TransactionMsg::finish_transaction(&transactionPath[path]);
        BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[path].get_path_outgate_towards_receiver(), msgDelay);
        msg->addMessage(bufMsg);
        res += "Added a message";

    } else if(state == TransactionPath::PUSH_TRANSACTION) {

        // Del transaction if all this have come in



    }
    return res;
}

std::string Transaction::update_forwarder(MessageBuffer *msg, int pathId) {
    std::string res = "";

    int path = get_transaction_path_index(pathId);

    if(path > -1) {
        if(transactionPath[path].get_state() == TransactionPath::JOINING_PATH) {

            create_message_towards_sender(msg, &transactionPath[path], TransactionPath::JOINED_PATH);

        } else if(transactionPath[path].get_state() == TransactionPath::JOINED_PATH) {

            create_message_towards_receiver(msg, &transactionPath[path], TransactionPath::PEND_TRANSACTION);

        } else if(transactionPath[path].get_state() == TransactionPath::PEND_TRANSACTION) {
            // Check and hold the capacity
            // if
            create_message_towards_sender(msg, &transactionPath[path], TransactionPath::PENDING_ACCEPTED);
            // else
            // close all paths due to capacity issues
        } else if(transactionPath[path].get_state() == TransactionPath::PENDING_ACCEPTED) {

            create_message_towards_receiver(msg, &transactionPath[path], TransactionPath::FINISH_TRANSACTION);

        } else if(transactionPath[path].get_state() == TransactionPath::FINISH_TRANSACTION) {

            create_message_towards_sender(msg, &transactionPath[path], TransactionPath::CLOSE_TRANSACTION);
            // TODO
            // Del transaction

        }
    }


    return res;
}

std::string Transaction::update_receiver(MessageBuffer *msg, int pathId) {
    std::string res = "";

    int path = -1;
    for(int i = 0; i < numOfAddedPaths; i++) {
        if(pathId == transactionPath[i].get_path_trans_id()) {
            path = i;
        }
    }

    if(path > -1) {

        if(transactionPath[path].get_state() == TransactionPath::JOINED_PATH) {
            // Need to make sure that I received enough Joined_Path
            receivedPendRequests++;
            if(receivedPendRequests >= numOfAddedPaths) {
                for(int i = 0; i < numOfAddedPaths; i++) {
                    transactionPath[i].set_state(TransactionPath::PENDING_ACCEPTED);
                    create_message_towards_sender(msg, &transactionPath[i], TransactionPath::PENDING_ACCEPTED);
                }

            }
        } else if(transactionPath[path].get_state() == TransactionPath::PENDING_ACCEPTED) {
            transactionPath[path].set_state(TransactionPath::CLOSE_TRANSACTION);
            create_message_towards_sender(msg, &transactionPath[path], TransactionPath::CLOSE_TRANSACTION);
        }

    }

    return res;
}


int Transaction::get_trans_id(void) {
    return transId;
}

int Transaction::get_execution_role(void) {
    return executionRole;
}

TransactionPath * Transaction::get_trans_path(int pathId) {
    TransactionPath *path;

    for(int i = 0; i < numOfAddedPaths; i++) {
        path = &transactionPath[i];
        if(path->get_path_trans_id() == pathId) {
            return path;
        }
    }
    return path;
}

bool Transaction::check_path_id(int pathId) {
    for(int i = 0; i < numOfAddedPaths; i++) {
        if(transactionPath[i].get_path_trans_id() == pathId) {
            return true;
        }
    }
    return false;
}

void Transaction::create_message_towards_sender(MessageBuffer *msg, TransactionPath *trans, int new_state) {
    BasicMessage *m;
    double msgDelay = latency.calculate_delay_ms(true);

    if(new_state == TransactionPath::JOINED_PATH) {
        m = TransactionMsg::path_request_accept(trans);
    } else if(new_state == TransactionPath::PENDING_ACCEPTED) {
        m = TransactionMsg::accept_pending_request(trans);
    } else if(new_state == TransactionPath::PENDING_ACCEPTED) {
        m = TransactionMsg::accept_pending_request(trans);
    } else if(new_state == TransactionPath::CLOSE_TRANSACTION) {
        m = TransactionMsg::close_transaction(trans);
    }

    BufferedMessage * bufMsg = new BufferedMessage(m, trans->get_path_outgate_towards_sender(), msgDelay);
    msg->addMessage(bufMsg);
    trans->set_state(new_state);
}

void Transaction::create_message_towards_receiver(MessageBuffer *msg, TransactionPath *trans, int new_state) {
    BasicMessage *m;
    double msgDelay = latency.calculate_delay_ms(true);

    if(new_state == TransactionPath::PEND_TRANSACTION) {
        m = TransactionMsg::pend_transaction_request(trans);
    } else if(new_state == TransactionPath::FINISH_TRANSACTION) {
        m = TransactionMsg::finish_transaction(trans);
    }

    BufferedMessage * bufMsg = new BufferedMessage(m, trans->get_path_outgate_towards_receiver(), msgDelay);
    msg->addMessage(bufMsg);
    trans->set_state(new_state);
}

int Transaction::get_transaction_path_index(int pathId) {
    int path = -1;
    for(int i = 0; i < numOfAddedPaths; i++) {
        if(pathId == transactionPath[i].get_path_trans_id()) {
            path = i;
        }
    }
    return path;
}


