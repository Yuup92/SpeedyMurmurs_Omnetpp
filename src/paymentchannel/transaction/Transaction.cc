#include "./Transaction.h"

Transaction::Transaction(){

    amount = 0;
    totalFees = 0;
    transId = 0;
    numOfPaths = 0;
    numOfAddedPaths = 0;
    lengthComplementNArray = 0;

    receivedAcceptedPaths = 0;

    receivedPendingPaths = 0;
    receivedPendRequests = 0;

    linkAttempts = 0;
    transId = 0;

    state = Transaction::TRANSACTION_ASLEEP;
    receivedClosedTransactions = 0;
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
    numOfPaths = 0;

    linkAttempts = 0;
    for(int i = 0 ; i < numOfAddedPaths; i++) {
        if(transactionPath[i].get_transaction_pended()) {
            LinkCapacity *c = transactionPath[i].get_link_towards_receiver()->get_link_capacity();
            c->cancel_pend(transactionPath[i].get_amount());
            transactionPath[i].set_transaction_pended(false);
        }
        transactionPath[i].reset();
    }

    for(int i = 0; i < 10 ; i++) {
        neighbourhoodSet[i] = 0;
        neighbourhoodComplement[i] = 0;
    }

    numOfAddedPaths = 0;
    receivedClosedTransactions = 0;

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
            if(n->check_capacity(amount_per_path)){
                transactionPath[numOfAddedPaths].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
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

    if(not sendingFailed) {
        for(int i = 0; i < numOfAddedPaths; i++) {
            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[i]);
            BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[i].get_path_outgate_towards_receiver(), msgDelay);
            msgBuf->addMessage(bufMsg);
        }
    } else {
        state = Transaction::TRANSACTION_DEAD;
    }
    return res;
}

std::string Transaction::transaction_new_neighbourhood(District *dis, MessageBuffer *msgBuf, int endNode, int pathAmount, int pathIndex, int tId = 0) {
    std::string res = "";

    int maxNumOfTrees = 10;

    double amount_per_path = pathAmount;

    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    if(transactionPath[pathIndex].get_transaction_pended()){
        LinkCapacity *c = transactionPath[pathIndex].get_link_towards_receiver()->get_link_capacity();
        c->cancel_pend(pathAmount);
        transactionPath[pathIndex].set_transaction_pended(false);
    }

    bool sendingFailed = false;
    numOfAddedPaths--;
    receivedAcceptedPaths--;
    bool foundNewPath = false;
    for(int i = linkAttempts; i < maxNumOfTrees; i++) {
       int randNeighbourhood = neighbourhoodSet[linkAttempts];
       linkAttempts++;
       LinkedNode *n = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
       if(numOfAddedPaths < numOfPaths and foundNewPath == false) {
           if(n->check_capacity(amount_per_path)) {
               foundNewPath = true;
               transactionPath[pathIndex].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
           }
       } else {
           break;
       }
    }

    if(linkAttempts >= maxNumOfTrees) {
       sendingFailed = true;
    }

    numOfAddedPaths++;
    if(not sendingFailed) {
        double msgDelay = latency.calculate_delay_ms(true);
        BasicMessage *m = TransactionMsg::path_initiation_request(&transactionPath[pathIndex]);
        BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[pathIndex].get_path_outgate_towards_receiver(), msgDelay);
        msgBuf->addMessage(bufMsg);
        res += "Sending a new inquiry about a possible path";
    } else {
        kill_transaction(msgBuf, pathIndex);
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
        res += kill_transaction(msgBuf, get_transaction_path_index(pathId));
    } else if(msg->getSubType() == TransactionMsg::CLOSE_PATH) {
        TransactionPath *transPath = get_trans_path(pathId);
        if(transPath->get_transaction_pended()) {
            LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
            c->cancel_pend(transPath->get_amount());
            transPath->set_transaction_pended(false);
            transPath->reset();
        } else {

        }

        if(transPath->get_execution_role() == TransactionPath::FORWARDER) {
            res += close_path(msgBuf, transPath);
        } else {

        }
        remove_transaction_path(transPath);
    }

    return res;
}

std::string Transaction::update_sender(MessageBuffer *msgBuf, int pathId, District *dist) {
    std::string res = "";
    TransactionPath *transPath = &transactionPath[get_transaction_path_index(pathId)];

    if(transPath->get_state() == TransactionPath::QUERY_STATE) {
        receivedAcceptedPaths++;
        res += s_pend_trans_handler(msgBuf);

    // Capacity is locked once PEND_TRANASCTION is received
    } else if(transPath->get_state() == TransactionPath::PEND_TRANSACTION) {
        res += s_push_trans_handler(msgBuf, transPath);
    } else if(transPath->get_state() == TransactionPath::PUSH_TRANSACTION) {
        LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
        if(c->complete_transaction_upstream(transPath->get_amount())) {
            transPath->set_transaction_pended(false);
            res += "Transaction completed";
            transPath->set_done();
            int pathsFinished = 0;
            for(int i = 0; i < numOfAddedPaths; i++) {
                if(transactionPath[i].is_completed()) {
                    pathsFinished++;
                }
            }
            if(pathsFinished == numOfAddedPaths) {
                res += "Transaction has been declared dead" + std::to_string(numOfAddedPaths);
                state = Transaction::TRANSACTION_DEAD;
            }
        } else {
            // TODO remove

            res += transaction_new_neighbourhood(dist, msgBuf, transPath->get_end_node_id(), transPath->get_amount(), get_transaction_path_index(pathId), transPath->get_transaction_id());
        }
    }
    return res;
}

std::string Transaction::update_forwarder(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";
    int path = get_transaction_path_index(pathId);
    TransactionPath *transPath = &transactionPath[path];

    if(path > -1) {
        if(transPath->get_state() == TransactionPath::JOINING_PATH) {
            create_message_towards_sender(msgBuf, &transactionPath[path], TransactionPath::JOINED_PATH);

        } else if(transPath->get_state() == TransactionPath::JOINED_PATH) {
            LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
            if(c->check_capacity(transPath->get_amount())) {
                c->pend_transaction_upstream(transPath->get_amount());
                transPath->set_transaction_pended(true);
                create_message_towards_receiver(msgBuf, transPath, TransactionPath::PEND_TRANSACTION);
            } else {
                //Capacity can not be pended
                close_path(msgBuf, transPath);
                forwader_capacity_error(msgBuf, transPath);
            }


        } else if(transPath->get_state() == TransactionPath::PEND_TRANSACTION) {
            create_message_towards_sender(msgBuf, transPath, TransactionPath::PENDING_ACCEPTED);
        } else if(transPath->get_state() == TransactionPath::PENDING_ACCEPTED) {
            create_message_towards_receiver(msgBuf, transPath, TransactionPath::FINISH_TRANSACTION);
        } else if(transPath->get_state() == TransactionPath::FINISH_TRANSACTION) {
            LinkCapacity *cSender = transPath->get_link_towards_receiver()->get_link_capacity();
            if(cSender->complete_transaction_upstream(transPath->get_amount())) {
                LinkCapacity *cReceiver = transPath->get_link_towards_sender()->get_link_capacity();
                cReceiver->complete_transaction_downstream(transPath->get_amount());
                transactionPath[path].set_transaction_pended(false);
                create_message_towards_sender(msgBuf, transPath, TransactionPath::CLOSE_TRANSACTION);

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
            } else {
                // TODO see if this occurs
                // kill transaction
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
            LinkCapacity *c = transactionPath[path].get_link_towards_sender()->get_link_capacity();
            c->complete_transaction_downstream(transactionPath[path].get_amount());

            transactionPath[path].set_done();
            bool allPathsDone = true;
            for(int i = 0; i <numOfAddedPaths; i++) {
                if(not transactionPath[i].is_completed()) {
                    allPathsDone = false;
                }
            }

            receivedClosedTransactions++;
            if(receivedClosedTransactions >= numOfAddedPaths) {
                for(int i = 0; i < numOfAddedPaths; i++) {
                    create_message_towards_sender(msg, &transactionPath[i], TransactionPath::CLOSE_TRANSACTION);
                }
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
    // TODO
    // Put this function in the caller of this function
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
    bool capacityError = false;
    if(receivedAcceptedPaths == numOfAddedPaths) {
        for(int i = 0; i < numOfAddedPaths; i++) {
            TransactionPath *transPath = &transactionPath[i];
            LinkedNode *n = transPath->get_link_towards_receiver();
            LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
            if(c->pend_transaction_upstream(transPath->get_amount())) {
                transPath->set_transaction_pended(true);
                transPath->set_num_of_total_paths(numOfAddedPaths);
                transPath->set_state(TransactionPath::PEND_TRANSACTION);
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::pend_transaction_request(transPath);
                BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
                res += "Added a message";

            } else {
                capacityError = true;
                res += "Pending failed";
            }
        }

        if(capacityError) {
            kill_transaction(msgBuf, 0);
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

std::string Transaction::close_path(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";
    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::close_path(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    return res;
}

std::string Transaction::capacity_error(MessageBuffer *msgBuf, int pathId, District *dist) {
    std::string res = "";
    int pathIndex = get_transaction_path_index(pathId);
    TransactionPath *p = &transactionPath[pathIndex];
    int s = p->get_execution_role();

    res += "state: " + std::to_string(s);

    if(s == TransactionPath::SENDER) {
        res += transaction_new_neighbourhood(dist, msgBuf, p->get_end_node_id(), p->get_amount(), pathIndex, p->get_transaction_id());
    } else if(s == TransactionPath::FORWARDER or
                    s == TransactionPath::RECEIVER) {
        forwader_capacity_error(msgBuf, p);
    }

    return res;
}

std::string Transaction::forwader_capacity_error(MessageBuffer *msgBuf, TransactionPath *transPath){
    if(transPath->get_transaction_pended()) {
        LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
        c->cancel_pend(transPath->get_amount());
        transPath->set_transaction_pended(false);
    }
    std::string res = "";
    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::capacity_error(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_sender(), msgDelay);
    msgBuf->addMessage(bufMsg);

    //remove_transaction_path(transPath);
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

std::string Transaction::kill_transaction(MessageBuffer *msgBuf, int pathIndex) {
    std::string res = "";
    if(state != Transaction::TRANSACTION_DEAD) {

        if(transactionPath[0].get_execution_role() == TransactionPath::SENDER) {
            for(int i = 0; i < numOfAddedPaths; i++){
                LinkCapacity *c = transactionPath[i].get_link_towards_receiver()->get_link_capacity();
                if(transactionPath[i].is_completed()) {
                    c->remove_capacity(transactionPath[i].get_amount());
                } else if(transactionPath[i].get_transaction_pended()) {
                    c->cancel_pend(transactionPath[i].get_amount());
                    transactionPath[i].set_transaction_pended(false);
                }
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::transaction_fail(&transactionPath[i]);
                BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[i].get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
            }
            state = Transaction::TRANSACTION_DEAD;
        } else if(transactionPath[0].get_execution_role() == TransactionPath::RECEIVER) {
            for(int i = 0; i < numOfAddedPaths; i++) {
                if(transactionPath[i].is_completed()) {
                    LinkCapacity *c = transactionPath[i].get_link_towards_sender()->get_link_capacity();
                    c->remove_capacity(transactionPath[i].get_amount());
                }
            }

            state = Transaction::TRANSACTION_DEAD;
            return res;
        } else if(transactionPath[pathIndex].get_execution_role() == TransactionPath::FORWARDER){
            numOfAddedPaths--;
            if(transactionPath[pathIndex].is_completed()) {
                LinkCapacity *cUpstream = transactionPath[pathIndex].get_link_towards_receiver()->get_link_capacity();
                cUpstream->remove_capacity(transactionPath[pathIndex].get_amount());

                LinkCapacity *cDownstream = transactionPath[pathIndex].get_link_towards_sender()->get_link_capacity();
                cDownstream->add_capacity(transactionPath[pathIndex].get_amount());
            } else if(transactionPath[pathIndex].get_transaction_pended()) {
                LinkCapacity *c = transactionPath[pathIndex].get_link_towards_receiver()->get_link_capacity();
                c->cancel_pend(transactionPath[pathIndex].get_amount());
                transactionPath[pathIndex].set_transaction_pended(false);
            }

            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::transaction_fail(&transactionPath[pathIndex]);
            BufferedMessage * bufMsg = new BufferedMessage(m, transactionPath[pathIndex].get_path_outgate_towards_receiver(), msgDelay);
            msgBuf->addMessage(bufMsg);
            // remove_transaction_path(&transactionPath[pathIndex]);
            if(numOfAddedPaths == 0){
                state = Transaction::TRANSACTION_DEAD;
            }
        }
    }
    return res;
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

void Transaction::remove_transaction_path(TransactionPath *transPath) {
    int index = -1;

    for(int i = 0; i < numOfAddedPaths; i++) {
        if(transactionPath[i].get_path_trans_id() == transPath->get_path_trans_id()) {
            index = i;
        }
    }

    if(numOfAddedPaths == 1) {
        state = TRANSACTION_DEAD;
        transactionPath[0].reset();
        numOfAddedPaths--;
    } else if(index > -1) {
        if(index + 1 == numOfAddedPaths) {
            transactionPath[index].reset();
        } else {
            for(int i = index; i < numOfAddedPaths - 1; i++) {
                std::swap(transactionPath[i], transactionPath[i+1]);
            }
            transactionPath[numOfAddedPaths-1].reset();
        }
        numOfAddedPaths--;
    }
}

