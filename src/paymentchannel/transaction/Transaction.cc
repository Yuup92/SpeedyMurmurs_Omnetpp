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
    role = 0;
    receivedClosedTransactions = 0;

    deathCounter = 100;
    transactionAliveTime = 0;

    sentClosedPaths = 0;
    receivedClosedPaths = 0;
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

    for(pair<int, TransactionPath*> element : transPathMap) {
        if(element.second->get_transaction_pended()) {
            LinkCapacity *c = element.second->get_link_towards_receiver()->get_link_capacity();
            c->cancel_pend(element.second->get_amount());
            element.second->set_transaction_pended(false);
        }
        element.second->reset();
    }

    for(int i = 0; i < 10 ; i++) {
        neighbourhoodSet[i] = 0;
        neighbourhoodComplement[i] = 0;
    }

    numOfAddedPaths = 0;
    receivedClosedTransactions = 0;

    state = Transaction::TRANSACTION_ASLEEP;
    transactionAliveTime = 0;
    role = 0;
    deathCounter = 100;

    sentClosedPaths = 0;
    receivedClosedPaths = 0;
}

void Transaction::set_stats(Statistics *s) {
    stats = s;
}

void Transaction::set_amount(double a) {
    amount = a;
}

double Transaction::get_amount(void) {
    return amount;
}

int Transaction::get_state(void) {
    return state;
}

int Transaction::get_role(void) {
    return role;
}

std::string Transaction::start_transaction(District *dis, MessageBuffer *msgBuf, int endNode, int numberOfPaths, int tId = 0) {
    //Get list of maximum paths
    std::string res = "";

    // Setup role and state
    state = Transaction::TRANSACTION_ALIVE;
    role = TransactionPath::SENDER;

    int maxNumOfTrees = MAXPATHS;
    numOfPaths = numberOfPaths;

    transId = rand();
    district = dis;

    float val = amount/numOfPaths;
    float nearest = roundf(val * 100) / 100;
    double amount_per_path = nearest;

    RandHelper::rand_set_and_complement_list(neighbourhoodSet, neighbourhoodComplement, maxNumOfTrees, maxNumOfTrees);
    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    bool sendingFailed = false;

    transactionAliveTime = simTime();

    for(int i = linkAttempts; i < maxNumOfTrees; i++) {
        int randNeighbourhood = neighbourhoodSet[linkAttempts];
        linkAttempts++;
        LinkedNode *linkedNode = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
        if(numOfAddedPaths < numOfPaths) {
            // If capacity is available
            if(linkedNode->check_capacity(amount_per_path)){
                TransactionPath *transPath = new TransactionPath();
                transPath->sending_path(amount_per_path, linkedNode, endNode, randNeighbourhood, transId);
                transPathMap.emplace(transPath->get_path_trans_id(), transPath);
                res += "Added path to map";
                // transactionPath[numOfAddedPaths].sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
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
        for(pair<int, TransactionPath*> element : transPathMap) {
            double msgDelay = latency.calculate_delay_ms(true);
            BasicMessage *m = TransactionMsg::path_initiation_request(element.second);
            BufferedMessage * bufMsg = new BufferedMessage(m, element.second->get_path_outgate_towards_receiver(), msgDelay);
            msgBuf->addMessage(bufMsg);
        }
    } else {
        state = Transaction::TRANSACTION_DEAD;
        stats->transactionsFailed++;
    }
    return res;
}

std::string Transaction::transaction_new_neighbourhood(District *dis, MessageBuffer *msgBuf, int endNode, int pathAmount, TransactionPath *transPath, int tId = 0) {
    std::string res = "";

    int maxNumOfTrees = 10;

    double amount_per_path = pathAmount;

    lengthComplementNArray = maxNumOfTrees - numOfPaths;

    if(transPath->get_transaction_pended()){
        LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
        c->cancel_pend(pathAmount);
        transPath->set_transaction_pended(false);
    }
    transPath->reset();

    // Setup role and state
    state = Transaction::TRANSACTION_ALIVE;
    role = TransactionPath::SENDER;
    bool sendingFailed = false;
    numOfAddedPaths--;
    receivedAcceptedPaths--;
    transactionAliveTime = simTime();

    bool foundNewPath = false;
    for(int i = linkAttempts; i < maxNumOfTrees; i++) {
       int randNeighbourhood = neighbourhoodSet[linkAttempts];
       linkAttempts++;
       LinkedNode *n = dis->get_neighbourhood(randNeighbourhood)->get_upstream_linked_node(endNode, amount_per_path);
       if(numOfAddedPaths < numOfPaths and foundNewPath == false) {
           if(n->check_capacity(amount_per_path)) {
               foundNewPath = true;
               transPath->sending_path(amount_per_path, n, endNode, randNeighbourhood, transId);
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
        BasicMessage *m = TransactionMsg::path_initiation_request(transPath);
        BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
        msgBuf->addMessage(bufMsg);
        res += "Sending a new inquiry about a possible path";
    } else {
        kill_transaction(msgBuf, transPath);
        res += "No new path found";
    }
    return res;
}

std::string Transaction::report_error(MessageBuffer *msgBuf, BasicMessage *msg, int pathId, District *dist) {

    std::string res = "report_error in Transaction";

    if(check_for_existing_path_id(pathId) and state == Transaction::TRANSACTION_ALIVE) {
        TransactionPath *transPath = get_trans_path_from_map(pathId);

            if(msg->getSubType() == TransactionMsg::CAPACITY_ERROR) {
                res += capacity_error(msgBuf, transPath, dist);
            } else if(msg->getSubType() == TransactionMsg::TIMEOUT) {
                timeout_error(msgBuf, transPath);
            } else if(msg->getSubType() == TransactionMsg::ERROR) {
                handle_error(msgBuf, transPath);
            } else if(msg->getSubType() == TransactionMsg::FAILURE) {
                res += kill_transaction(msgBuf, transPath);
            } else if(msg->getSubType() == TransactionMsg::CLOSE_PATH) {
                if(role == TransactionPath::FORWARDER) {
                    res += close_path(msgBuf, transPath);
                }

                transPath->close_path();
                if(transPath->get_transaction_pended()) {
                    LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
                    c->cancel_pend(transPath->get_amount());
                    transPath->set_transaction_pended(false);
                }


                bool allPathsClosed = true;
                for(pair<int, TransactionPath*> element : transPathMap) {
                    if(element.second->is_path_closed() == false) {
                        allPathsClosed = false;
                    }
                }
                if(allPathsClosed){
                    state = Transaction::TRANSACTION_DEAD;
                    res += "\n\n State is set to dead \n\n";
                }
            }
    } else {
        res += "\n\n unable to find pathId... \n\n or transactions is waiting to be removed \n\n";
    }

    return res;
}

std::string Transaction::update_sender(MessageBuffer *msgBuf, int pathId, District *dist) {
    std::string res = "";

    TransactionPath *transPath;
    if(check_for_existing_path_id(pathId)) {
        transPath = get_trans_path_from_map(pathId);
    } else {
        res += "Could not find transaction path";
        return res;
    }

    if(transPath->get_state() == TransactionPath::QUERY_STATE) {
        receivedAcceptedPaths++;
        res += sender_pend_capacities(msgBuf);
    } else if(transPath->get_state() == TransactionPath::PEND_TRANSACTION) {
        res += sender_push_transaction(msgBuf, transPath);
    } else if(transPath->get_state() == TransactionPath::PUSH_TRANSACTION) {
        res += sender_close_transaction(transPath);
    }
    return res;
}

std::string Transaction::update_forwarder(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";

    if(check_for_existing_path_id(pathId)) {
        TransactionPath *transPath = get_trans_path_from_map(pathId);

        if(transPath->get_state() == TransactionPath::JOINING_PATH) {
            create_message_towards_sender(msgBuf, transPath, TransactionPath::JOINED_PATH);
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
                // Search how to remove transPath from map
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
                transPath->set_transaction_pended(false);
                create_message_towards_sender(msgBuf, transPath, TransactionPath::CLOSE_TRANSACTION);
                transPath->set_done();
                bool allPathsDone = true;

                // TODO
                // Do this differently
                // can remove path one by one(?)
                // maybe put paths in dead states(?)
                int pathsFinished = 0;
                for(pair<int, TransactionPath*> element : transPathMap) {
                    TransactionPath *p = element.second;
                    if(p->is_completed()) {
                        pathsFinished++;
                    }
                }
                if(pathsFinished == numOfAddedPaths) {
                    state = Transaction::TRANSACTION_DEAD;
                }
            } else {
                // TODO see if this occurs
                // kill transaction
            }
        }
    } else {
        res += "PATH NOT FOUND!";
    }
    return res;
}

std::string Transaction::update_receiver(MessageBuffer *msgBuf, int pathId) {
    std::string res = "";

    if(check_for_existing_path_id(pathId)) {
        TransactionPath *transPath = get_trans_path_from_map(pathId);

        if(transPath->get_state() == TransactionPath::JOINED_PATH) {
            // Need to make sure that I received enough Joined_Path
            receivedPendRequests++;
            if(receivedPendRequests >= numOfAddedPaths) {
                for(pair<int, TransactionPath*> element : transPathMap) {
                    transPath->set_state(TransactionPath::PENDING_ACCEPTED);
                    create_message_towards_sender(msgBuf, element.second, TransactionPath::PENDING_ACCEPTED);
                }
            }
        } else if(transPath->get_state() == TransactionPath::PENDING_ACCEPTED) {
            transPath->set_state(TransactionPath::CLOSE_TRANSACTION);
            LinkCapacity *c = transPath->get_link_towards_sender()->get_link_capacity();
            c->complete_transaction_downstream(transPath->get_amount());

            transPath->set_done();

            receivedClosedTransactions++;
            if(receivedClosedTransactions >= numOfAddedPaths) {
                for(pair<int, TransactionPath*> element : transPathMap) {
                    create_message_towards_sender(msgBuf, element.second, TransactionPath::CLOSE_TRANSACTION);
                }
                state = Transaction::TRANSACTION_DEAD;
            }

            int pathsFinished = 0;
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *p = element.second;
                if(p->is_completed()) {
                    pathsFinished++;
                }
            }
            if(pathsFinished == numOfAddedPaths) {
                state = Transaction::TRANSACTION_DEAD;
            }


        }

    }

    return res;
}

std::string Transaction::new_foward_transaction(MessageBuffer *msgBuf, int tId, TransactionPath *transPath) {
    std::string res = "";
    state = Transaction::TRANSACTION_ALIVE;
    role = TransactionPath::FORWARDER;
    transactionAliveTime = simTime();
    transId = tId;
    res += add_new_forward_path(msgBuf, transPath);
    return res;
}

std::string Transaction::add_new_forward_path(MessageBuffer *msgBuf, TransactionPath *transPath) {

    std::string res = "";
    TransactionPath *transP = new TransactionPath();
    transP->set_forwarding_path(transPath);
    transPathMap.emplace(transP->get_path_trans_id(), transP);
    res += "Added path to transaction ";

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::path_initiation_request(transP);
    BufferedMessage * bufMsg = new BufferedMessage(m, transP->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);

    numOfAddedPaths++;
    return res;
}

std::string Transaction::new_receiving_transaction(MessageBuffer *msgBuf, int tId, TransactionPath *transPath) {
    std::string res = "";
    state = Transaction::TRANSACTION_ALIVE;
    role = TransactionPath::RECEIVER;
    transactionAliveTime = simTime();
    transId = tId;
    res += add_new_receiving_path(msgBuf, transId, transPath);
    return res;
}

std::string Transaction::add_new_receiving_path(MessageBuffer *msgBuf, int transId, TransactionPath *transPath) {
    std::string res = "";

    TransactionPath *transP = new TransactionPath();
    transP->set_forwarding_path(transPath);
    transPathMap.emplace(transP->get_path_trans_id(), transP);

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::path_request_accept(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_sender(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";

    numOfAddedPaths++;

    return res;
}

int Transaction::get_trans_id(void) {
    return transId;
}

int Transaction::get_death_counter(void) {
    return deathCounter;
}

int Transaction::increment_death_counter(void) {
    deathCounter--;
    return deathCounter;
}

void Transaction::check_alive_time(void) {
    if(state != Transaction::TRANSACTION_DEAD) {
        if(simTime().dbl() - transactionAliveTime.dbl() > TRANSACTION_ALIVE_TIME) {
            if(transPathMap.size() > 0) {
                for(pair<int, TransactionPath*> element : transPathMap) {
                    if(role == TransactionPath::SENDER) {
                        TransactionPath *transPath = element.second;

                         if(transPath->get_transaction_pended()){
                             LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
                             c->cancel_pend(transPath->get_amount());
                             transPath->set_transaction_pended(false);
                         }
                    } else if(role == TransactionPath::FORWARDER) {
                        TransactionPath *transPath = element.second;
                         if(transPath->get_transaction_pended()){
                             LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
                             c->cancel_pend(transPath->get_amount());
                             transPath->set_transaction_pended(false);
                         }
                    }
                }
            }
            state = Transaction::TRANSACTION_DEAD;
            deathCounter = 1;
            if(role == TransactionPath::SENDER) {
                stats->transactionsFailed++;
            }
        }
    }
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

std::string Transaction::sender_pend_capacities(MessageBuffer *msgBuf) {
    std::string res = "";
    bool capacityError = false;
    if(receivedAcceptedPaths == numOfAddedPaths) {
        //Check for all paths if it is possible to pend_transaction
        bool capacityAvailable = true;
        for(pair<int, TransactionPath*> element : transPathMap) {
            TransactionPath *transPath = element.second;
            LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();

            if(c->pend_transaction_upstream(transPath->get_amount()) and transPath->get_transaction_pended() == false) {
                transPath->set_transaction_pended(true);
                transPath->set_num_of_total_paths(numOfAddedPaths);
                transPath->set_state(TransactionPath::PEND_TRANSACTION);
            } else {
                capacityAvailable = false;
                break;
            }
        }

        if(capacityAvailable) {
            // send put msgs
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *transPath = element.second;
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::pend_transaction_request(transPath);
                BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
            }
        } else if(linkAttempts < 10) {
            //search for new msgs
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *transPath = element.second;
                close_path(msgBuf, transPath);
                if(transPath->get_transaction_pended() == false) {
                    transaction_new_neighbourhood(district, msgBuf, endNode, transPath->get_amount(), transPath, transPath->get_transaction_id());
                }
            }
        } else {
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *transPath = element.second;
                if(transPath->get_transaction_pended()) {
                    LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
                    c->cancel_pend(transPath->get_amount());
                    transPath->set_transaction_pended(false);
                }
                close_path(msgBuf, transPath);
            }
            state = Transaction::TRANSACTION_DEAD;
        }
    }

    return res;
}

std::string Transaction::sender_push_transaction(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";
    transPath->set_state(TransactionPath::PUSH_TRANSACTION);
    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::finish_transaction(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";
    sentClosedPaths++;
    return res;
}

std::string Transaction::sender_close_transaction(TransactionPath *transPath) {
    string res = "";
    receivedClosedPaths++;
    if(sentClosedPaths == receivedClosedPaths) {
        for(pair<int, TransactionPath*> element : transPathMap) {
            TransactionPath *p = element.second;
            LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
            if(c->complete_transaction_upstream(transPath->get_amount())) {
                transPath->set_transaction_pended(false);
                transPath->set_done();
            }
        }
        state = Transaction::TRANSACTION_DEAD;
        stats->transactionsCompleted++;
    }
    return res;
}

std::string Transaction::close_path(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";
    transPath->close_path();
    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::close_path(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    return res;
}

std::string Transaction::capacity_error(MessageBuffer *msgBuf, TransactionPath *transPath, District *dist) {
    std::string res = "";

    if(role == TransactionPath::SENDER) {
        res += transaction_new_neighbourhood(dist, msgBuf, transPath->get_end_node_id(), transPath->get_amount(), transPath, transPath->get_transaction_id());
    } else if(role == TransactionPath::FORWARDER or
                    role == TransactionPath::RECEIVER) {
        forwader_capacity_error(msgBuf, transPath);
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

std::string Transaction::timeout_error(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::timeout_error(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";

    state = Transaction::TRANSACTION_DEAD;

    return res;
}

std::string Transaction::handle_error(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";

    double msgDelay = latency.calculate_delay_ms(true);
    BasicMessage *m = TransactionMsg::timeout_error(transPath);
    BufferedMessage * bufMsg = new BufferedMessage(m, transPath->get_path_outgate_towards_receiver(), msgDelay);
    msgBuf->addMessage(bufMsg);
    res += "Added a message";

    state = Transaction::TRANSACTION_DEAD;

    return res;
}

std::string Transaction::kill_transaction(MessageBuffer *msgBuf, TransactionPath *transPath) {
    std::string res = "";
    if(state != Transaction::TRANSACTION_DEAD) {
        if(role == TransactionPath::SENDER) {
            //Transaction Failed
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *tP = element.second;
                LinkCapacity *c = tP->get_link_towards_receiver()->get_link_capacity();
                if(tP->is_completed()) {
                    c->remove_capacity(tP->get_amount());
                } else if(tP->get_transaction_pended()) {
                    c->cancel_pend(tP->get_amount());
                    tP->set_transaction_pended(false);
                }
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::transaction_fail(tP);
                BufferedMessage * bufMsg = new BufferedMessage(m, tP->get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
            }
            state = Transaction::TRANSACTION_DEAD;
            stats->transactionsFailed++;
        } else if(role == TransactionPath::RECEIVER) {
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *tP = element.second;
                if(tP->is_completed()) {
                    LinkCapacity *c = tP->get_link_towards_receiver()->get_link_capacity();
                    c->remove_capacity(tP->get_amount());
                    tP->set_completed(false);
                }
            }
            state = Transaction::TRANSACTION_DEAD;
            return res;
        } else if(role == TransactionPath::FORWARDER){
            numOfAddedPaths--;
            if(transPath->is_completed()) {
                LinkCapacity *cUpstream = transPath->get_link_towards_receiver()->get_link_capacity();
                cUpstream->remove_capacity(transPath->get_amount());

                LinkCapacity *cDownstream = transPath->get_link_towards_sender()->get_link_capacity();
                cDownstream->add_capacity(transPath->get_amount());
                transPath->set_completed(false);
            } else if(transPath->get_transaction_pended()) {
                LinkCapacity *c = transPath->get_link_towards_receiver()->get_link_capacity();
                c->cancel_pend(transPath->get_amount());
                transPath->set_transaction_pended(false);
            }

            // forwad to all nodes
            for(pair<int, TransactionPath*> element : transPathMap) {
                TransactionPath *tP = element.second;
                double msgDelay = latency.calculate_delay_ms(true);
                BasicMessage *m = TransactionMsg::transaction_fail(tP);
                BufferedMessage * bufMsg = new BufferedMessage(m, tP->get_path_outgate_towards_receiver(), msgDelay);
                msgBuf->addMessage(bufMsg);
            }

            state = Transaction::TRANSACTION_DEAD;
        }
    }
    return res;
}

bool Transaction::check_for_existing_path_id(int pathId) {
    unordered_map<int,TransactionPath*>::const_iterator got = transPathMap.find(pathId);
    if(got == transPathMap.end()) {
        return false;
    } else {
        return true;
    }
}

TransactionPath* Transaction::get_trans_path_from_map(int pathId) {
    unordered_map<int,TransactionPath*>::const_iterator got = transPathMap.find(pathId);
    return got->second;
}

