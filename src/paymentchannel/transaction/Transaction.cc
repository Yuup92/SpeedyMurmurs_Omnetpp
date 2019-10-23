#include "./Transaction.h"

Transaction::Transaction(){

    amount = 0;
    totalFees = 0;
    transId = 0;
    endNode = -1;
    numOfPaths = 0;
    numOfAddedPaths = 0;
    lengthComplementNArray = 0;

    receivedAcceptedPaths = 0;

    receivedPendingPaths = 0;
    receivedPendRequests = 0;

    linkAttempts = 0;
    transId = 0;

    state = Transaction::TRANSACTION_ASLEEP;
}

void Transaction::reset(void) {
    numOfAddedPaths = 0;
    lengthComplementNArray = 0;
    receivedAcceptedPaths = 0;
    receivedPendingPaths = 0;
    receivedPendRequests = 0;

    amount = 0;
    totalFees = 0;
    transId = 0;
    endNode = -1;
    numOfPaths = 0;

    linkAttempts = 0;
    for(int i = 0 ; i < numOfAddedPaths; i++) {
        transactionPath[i].reset();
    }

    for(int i = 0; i < 10 ; i++) {
        neighbourhoodSet[i] = 0;
        neighbourhoodComplement[i] = 0;
    }

    numOfAddedPaths = 0;

    state = Transaction::TRANSACTION_ASLEEP;
}


void Transaction::set_amount(double a) {
    amount = a;
}

int Transaction::get_state(void) {
    return state;
}

std::string Transaction::start_transaction(District *dis, MessageBuffer *msgBuf, int endNode, int pathAmount, int tId = 0) {
    //Get list of maximum paths
    std::string res = "";

    state = Transaction::TRANSACTION_ALIVE;

    //TODO make maxNumOfTrees not a number
    // make a config file or something
    int maxNumOfTrees = 10;
    numOfPaths = pathAmount;
    if(tId == 0) {
        transId = rand();
    } else {
        transId = tId;
    }

    double amount_per_path = amount / numOfPaths;

    RandHelper::rand_set_and_complement_list(neighbourhoodSet, neighbourhoodComplement, maxNumOfTrees, maxNumOfTrees);
    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    bool sendingFailed = false;
    int attempts = 0;

    for(int i = linkAttempts; i < maxNumOfTrees; i++) {
        int randNeighbourhood = neighbourhoodSet[linkAttempts];
        linkAttempts++;
        LinkedNode *n = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
        if(numOfAddedPaths < numOfPaths) {
            if(amount_per_path < n->get_capacity()) {
                transactionPath[numOfAddedPaths].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[numOfAddedPaths]);
                BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[numOfAddedPaths].get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
                res += "Added a message";
                numOfAddedPaths++;
            }
        } else {
            break;
        }

    }

    if(linkAttempts >= maxNumOfTrees) {
        sendingFailed = true;
    }

    if(sendingFailed) {
        kill_transaction(msgBuf);
        return "false";
    }
    return res;
}

std::string Transaction::transaction_new_neighbourhood(District *dis, MessageBuffer *msgBuf, int endNode, int pathAmount, int pathIndex, int tId = 0) {
    std::string res = "";

    int maxNumOfTrees = 10;

    double amount_per_path = pathAmount;

    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    bool sendingFailed = false;
    numOfAddedPaths--;
    for(int i = linkAttempts; i < maxNumOfTrees; i++) {
       int randNeighbourhood = neighbourhoodSet[linkAttempts];
       linkAttempts++;
       LinkedNode *n = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
       if(numOfAddedPaths < numOfPaths) {
           if(amount_per_path < n->get_capacity()) {
               transactionPath[pathIndex].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
               double msgDelay = latency.calculate_delay_ms(true);
               BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[pathIndex]);
               BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[pathIndex].get_path_outgate_towards_receiver(), msgDelay);
               msgBuf->addMessage(bufMsg);
               res += "Added a message";
               numOfAddedPaths++;
           }
       } else {
           break;
       }
    }

    if(linkAttempts >= maxNumOfTrees) {
       sendingFailed = true;
    }

    if(sendingFailed) {
        kill_transaction(msgBuf);
        res += "No new path found";
    }
    return res;
}

std::string Transaction::report_error(MessageBuffer *msgBuf, BasicMessage *msg, int pathId, District *dist) {

    std::string res = "report_error in Transaction";

    if(msg->getSubType() == TransactionMsg::CAPACITY_ERROR) {
        res += capacity_error(msgBuf, pathId, dist);
    } else if(msg->getSubType() == TransactionMsg::TIMEOUT) {
        timeout_error(msgBuf, pathId);
    } else if(msg->getSubType() == TransactionMsg::ERROR) {
        handle_error(msgBuf, pathId);
    } else if(msg->getSubType() == TransactionMsg::FAILURE) {
        kill_transaction(msgBuf);
    }

    return res;
}

std::string Transaction::update_sender(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";
    TransactionPath *transPath = &transactionPath[get_transaction_path_index(pathId)];

    if(transPath->get_state() == TransactionPath::QUERY_STATE) {
        receivedAcceptedPaths++;
        s_pend_trans_handler(msgBuf);
    } else if(transPath->get_state() == TransactionPath::PEND_TRANSACTION) {
        s_push_trans_handler(msgBuf, transPath);


    } else if(transPath->get_state() == TransactionPath::PUSH_TRANSACTION) {
        transPath->set_done();
        bool allPathsDone = true;
        for(int i = 0; i <numOfAddedPaths; i++) {
            if(not transactionPath[i].is_completed()) {
                allPathsDone = false;
            }
        }
        if(allPathsDone) {
            state = Transaction::TRANSACTION_DEAD;
        }

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

            transactionPath[path].set_done();
            bool allPathsDone = true;
            for(int i = 0; i <numOfAddedPaths; i++) {
                if(not transactionPath[i].is_completed()) {
                    allPathsDone = false;
                }
            }
            if(allPathsDone) {
                state = Transaction::TRANSACTION_DEAD;
            }


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

            transactionPath[path].set_done();
            bool allPathsDone = true;
            for(int i = 0; i <numOfAddedPaths; i++) {
                if(not transactionPath[i].is_completed()) {
                    allPathsDone = false;
                }
            }
            if(allPathsDone) {
                state = Transaction::TRANSACTION_DEAD;
            }
        }

    }

    return res;
}


std::string Transaction::forward_transaction(MessageBuffer *msg, int tId, TransactionPath *transPath) {
        std::string res = "";
        state = Transaction::TRANSACTION_ALIVE;
        transId = tId;
        transactionPath[numOfAddedPaths].set_forwarding_path(transPath);

        double msgDelay = latency.calculate_delay_ms(true);
        BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[numOfAddedPaths]);
        BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[numOfAddedPaths].get_path_outgate_towards_receiver(), msgDelay);
        msg->addMessage(bufMsg);
        res += "Added a message";

        numOfAddedPaths++;
        return res;
}


std::string Transaction::receiving_transaction(MessageBuffer *msg, int tId, TransactionPath *transPath) {
    std::string res = "";
    state = Transaction::TRANSACTION_ALIVE;
    transId = tId;
    transactionPath[numOfAddedPaths].set_receiving_path(transPath);

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::path_request_accept(&transactionPath[numOfAddedPaths]);
    BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[numOfAddedPaths].get_path_outgate_towards_sender(), msgDelay);
    msg->addMessage(bufMsg);
    res += "Added a message";

    numOfAddedPaths++;

    return res;
}


int Transaction::get_trans_id(void) {
    return transId;
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
        if(state != Transaction::TRANSACTION_DEAD) {
            m = TransactionMsg::close_transaction(trans);
        }

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


std::string Transaction::s_pend_trans_handler(MessageBuffer *msgBuf) {
    std::string res = "";
    if(receivedAcceptedPaths == numOfAddedPaths) {
        for(int i = 0; i < numOfPaths; i++) {
            TransactionPath *transPath = &transactionPath[i];
            transPath->set_num_of_total_paths(numOfPaths);
            transPath->set_state(TransactionPath::PEND_TRANSACTION);

            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::pend_transaction_request(transPath);
            BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
            msgBuf->addMessage(bufMsg);
            res += "Added a message";
        }
    }

    return res;
}

std::string Transaction::s_push_trans_handler(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";
    transPath->set_state(TransactionPath::PUSH_TRANSACTION);
    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::finish_transaction(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";
    return res;
}

std::string Transaction::capacity_error(MessageBuffer *msgBuf, int pathId, District *dist) {
    std::string res = "";
    int pathIndex = get_transaction_path_index(pathId);
    TransactionPath *p = &transactionPath[pathIndex];
    int s = p->get_execution_role();

    res += "state: " + std::to_string(s);

    if(s == TransactionPath::SENDER) {
        res += s_capacity_error(msgBuf, p, pathIndex, dist);
    } else if(s == TransactionPath::FORWARDER or
                    s == TransactionPath::RECEIVER) {
        double msgDelay = latency.calculate_delay_ms(true);
        BasicMessage *m = TransactionMsg::capacity_error(p);
        BufferedMessage * bufMsg = new BufferedMessage(m, p->get_path_outgate_towards_sender(), msgDelay);
        msgBuf->addMessage(bufMsg);
        if(numOfAddedPaths == 1) {
            s = Transaction::TRANSACTION_DEAD;
            res += "Transaction died";
        } else {
            //Remove path
            res += "Path died, find new one";
        }
    }

    return res;
}


std::string Transaction::s_capacity_error(MessageBuffer *msgBuf, TransactionPath *p, int pathIndex, District *dist) {
    std::string res = transaction_new_neighbourhood(dist, msgBuf, p->get_end_node_id(), 1, pathIndex, p->get_transaction_id());
    return res;
}

std::string Transaction::timeout_error(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";
    TransactionPath *transPath = &transactionPath[get_transaction_path_index(pathId)];

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::timeout_error(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";

    state = Transaction::TRANSACTION_DEAD;

    return res;
}

std::string Transaction::handle_error(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";
    TransactionPath *transPath = &transactionPath[get_transaction_path_index(pathId)];

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::timeout_error(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";

    state = Transaction::TRANSACTION_DEAD;

    return res;
}

void Transaction::kill_transaction(MessageBuffer *msgBuf) {
    if(state != Transaction::TRANSACTION_DEAD) {
        state = Transaction::TRANSACTION_DEAD;
        for(int i = 0; i < numOfAddedPaths; i++) {
            if(transactionPath[i].get_execution_role() == TransactionPath::RECEIVER) {
                break;
            }
            double msgDelay = latency.calculate_delay_ms(true) * 4;
            BasicMessage *m = TransactionMsg::transaction_fail(&transactionPath[i]);
            BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[i].get_path_outgate_towards_receiver(), msgDelay);
            msgBuf->addMessage(bufMsg);
        }

    }
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


