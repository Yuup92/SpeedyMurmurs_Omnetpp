#include "./SpanningTree.h"

SpanningTree::SpanningTree(){
    sent_requests = 0;
    edgesWeightUpdated = false;
    noInitialQueueMessages = true;
    spanningTreeNodeId = rand();
    stateNode = SpanningTree::STATE_SLEEPING;

    // TODO check if needs to be initialized with best-weight right out of box
    fragmentName = spanningTreeNodeId;

    currentMinEdge = -1;
    testEdge = -1;
    bestEdge = -1;
    bestWeight = std::numeric_limits<int>::max();
    findCount = 0;
    edgeTowardsRoot = -1;

    initialQueueIndex = -1;
    connectQueueIndex = 0;
    reportQueueIndex = 0;
    testQueueIndex = 0;

    testCounter = 0;
    handle_message_counter = 0;
    handle_spanning_tree_counter = 0;
    handle_queue = 0;
    maxQueue = 0;
    handle_connect_msg = 0;

    isRoot = false;
    fullBroadcast = false;
    linkedNodesUpdated = false;

    msgSentDownStream = 0;
    msgDelay = 0.0;

    reply_broadcast = "";

    previousSimTime = 0;

    currentDepth = -1;
    maxDepth = 0;

    indexChildrenNodes = 0;
    numConnectedNodes = 0;
}

void SpanningTree::set_num_neighbours(int amountOfOutGates) {
    numConnectedNodes = amountOfOutGates;
    fill_state_edges(&stateEdges[0]);
}

int SpanningTree::get_num_neighbours(void){
    return numConnectedNodes;
}

int SpanningTree::get_num_sent_messages(void){
    return sent_requests;
}

int SpanningTree::get_initial_queue_index(void) {
    return initialQueueIndex;
}

int SpanningTree::get_level(void) {
    return fragmentLevel;
}

void SpanningTree::fill_state_edges(state_edge *state_edges) {
    for(int i = 0; i < numConnectedNodes; i++) {
        (state_edges + i)->outgoingEdge = i;
        (state_edges + i)->state = SpanningTree::EDGESTATE_Q_MST;
        (state_edges + i)->weight = std::numeric_limits<int>::max();
        (state_edges + i)->numOfChildren = 0;
        (state_edges + 1)->edgeTowardsRoot = false;
    }
}

void SpanningTree::set_node_id(int id) {
    nodeId = id;
}

void SpanningTree::set_spanning_tree_index(int index) {
    spanningTreeIndex = index;
}

bool SpanningTree::is_node_root(void) {
    return isRoot;
}

bool SpanningTree::full_broadcast_finished(void) {
    return fullBroadcast;
}

bool SpanningTree::get_linked_nodes_updated(void) {
    return linkedNodesUpdated;
}

int SpanningTree::get_state(void) {
    return stateNode;
}


// TODO rename to_string()
std::string SpanningTree::get_state_edge(void) {

    std::string res = "";

    if(stateNode == SpanningTree::STATE_FIND) {
        res = res + "Node in State: Find \n";
    } else if(stateNode == SpanningTree::STATE_FOUND) {
        res = res + "Node in State: Found \n";
    } else {
        res = res + "Node in State: Other? \n";
    }

    char room[256];
    for(int i = 0; i < numConnectedNodes; i++) {
        sprintf( room, "\n %d outoging_edge: %d \n weight: %d \n numOfChildren: %d \n", i, stateEdges[i].outgoingEdge, stateEdges[i].weight, stateEdges[i].numOfChildren);
        res = res + room;
        if(stateEdges[i].state == SpanningTree::EDGESTATE_Q_MST) {
            res = res + " state: ? MST";
        } else if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
            res = res + " state: In MST";
        } else if(stateEdges[i].state == SpanningTree::EDGESTATE_NOT_IN_MST){
            res = res + " state: not in mst";
        } else {
            char buf[40];
            sprintf(buf, " state: %d", stateEdges[i].state);
            res = res + buf;
        }

        res += "\n children: \n";
        for(int j = 0; j < stateEdges[i].numOfChildren; j++) {
            res += std::to_string(stateEdges[i].children[j]) + " , ";
        }

        res = res + " \n";

    }
    char bug[40];
    sprintf(bug, "Towards root: %d \n", edgeTowardsRoot);

    //char queue[300];
    //sprintf(queue, " queue connect: %d \n queue test: %d \n queue report: %d \n testCounter: %d \n fragmentName: %d \n initialQueueIndex: %d \n handleMessage: %d \n handleSpanningTree: %d \n handleQueue: %d \n maxQueue: %d \n handleConnect: %d \n", connectQueueIndex, testQueueIndex, reportQueueIndex, testCounter, fragmentName, initialQueueIndex,handle_message_counter, handle_spanning_tree_counter, handle_queue, maxQueue, handle_connect_msg);
    //res = res +  bug + " \n" + queue;

    if(isRoot) {
        res = res + " I am the root \n";
        if(fullBroadcast) {
            std::string list = " children nodes: \n";
            aggregate_children_list();
            for(int i = 0; i < indexChildrenNodes; i++) {
                list += std::to_string(listChildrenNodes[i]);
                list += " , ";
            }
            list += "\n";
            res = res + list;
            res += "Max depth: " + std::to_string(maxDepth) + " \n";
        }



    }

    return res;

}

int SpanningTree::get_length_vector_cor(void) {
    return lengthVectorList;
}

int* SpanningTree::get_vector_cor(void) {
    return &vectorList[0];
}

void SpanningTree::wake_up(void) {
    get_weight_edges();
    stateNode = SpanningTree::STATE_FINDING_WEIGHT;
}

void SpanningTree::start_building_tree(void) {

    currentMinEdge = find_minimum_weight_edge();
    bestWeight = stateEdges[currentMinEdge].weight;
    fragmentName = bestWeight;
    stateEdges[currentMinEdge].state = SpanningTree::EDGESTATE_IN_MST;
    fragmentLevel = 0;
    stateNode = SpanningTree::STATE_FOUND;
    findCount = 0;

    // fix??
    // edgeTowardsRoot = stateEdges[currentMinEdge].outgoingEdge;

    update_message_buf(SpanningTree::connect(fragmentLevel, spanningTreeIndex), stateEdges[currentMinEdge].outgoingEdge);
}

void SpanningTree::get_weight_edges(void) {
    for(int i = 0; i < numConnectedNodes; i++) {
        update_message_buf(SpanningTree::weight_request(spanningTreeNodeId, i, spanningTreeIndex), stateEdges[i].outgoingEdge);
        sent_requests++;
    }
}

int SpanningTree::get_message_count(void)
{
    return msgBuf.getMessageCount();
}

BufferedMessage * SpanningTree::get_message(void)
{
    return msgBuf.getMessage();
}


void SpanningTree::handle_message(BasicMessage* msg, int outgoingEdge, omnetpp::simtime_t prevSimTime)
{
    handle_message_counter++;
    if(previousSimTime != prevSimTime) {
        msgDelay = 0;
    } else {
        previousSimTime = prevSimTime;
    }


    if(stateNode == SpanningTree::STATE_SLEEPING) {
        wake_up();
    }

    if(edgesWeightUpdated and noInitialQueueMessages) {
        handle_spanning_tree_message(msg, outgoingEdge);
    } else {
        if(msg->getSubType() == SpanningTree::WEIGHT_REQUEST) {
            handle_weight_request(outgoingEdge, msg->getSpanningTreeId(), msg->getSpanningTreeIndexList());
        } else if(msg->getSubType() == SpanningTree::WEIGHT_REPONSE) {
            handle_weight_response(msg->getWeightEdgeSpanningTree(), msg->getSpanningTreeIndexList(), msg->getNodeId());
        } else {
            if(not edgesWeightUpdated){
                update_initial_queue(msg, outgoingEdge);
            } else if(not noInitialQueueMessages) {
                handle_initial_queue();
            } else {
                noInitialQueueMessages = true;
            }
        }
    }

    check_queued_messages();
}

void SpanningTree::update_initial_queue(BasicMessage *msg, int outgoingEdge) {
    QueuedMessage* initialQueueElement = new QueuedMessage();
    BasicMessage *msg_p = msg->dup();
    initialQueueElement->set_initial_queue(msg_p, outgoingEdge);
    initialQueueIndex++;
    maxQueue++;
    initialQueue[initialQueueIndex] = initialQueueElement;
    noInitialQueueMessages = false;
}

void SpanningTree::handle_initial_queue(void) {
    if(not noInitialQueueMessages) {
        while(initialQueueIndex > -1) {
            if(initialQueueIndex > -1){
                handle_queue++;
                BasicMessage *msg = initialQueue[initialQueueIndex]->get_message();
                int outgoingEdge = initialQueue[initialQueueIndex]->get_edge();
                handle_spanning_tree_message(msg, outgoingEdge);
                delete(initialQueue[initialQueueIndex]);
                initialQueueIndex--;
            }
        }
        noInitialQueueMessages = true;
    }
}

void SpanningTree::handle_spanning_tree_message(BasicMessage *msg, int outgoingEdge) {
    handle_spanning_tree_counter++;
    if(msg->getSubType() == SpanningTree::CONNECT) {
       handle_connect(outgoingEdge,
                      msg->getLevelNode());
    } else if(msg->getSubType() == SpanningTree::INITIATE) {
       handle_initiate(outgoingEdge,
                       msg->getLevelNode(),
                       msg->getFragmentName(),
                       msg->getNodeState());
    } else if(msg->getSubType() == SpanningTree::TEST) {
       handle_test(outgoingEdge,
                   msg->getLevelNode(),
                   msg->getFragmentName());
    } else if(msg->getSubType() == SpanningTree::ACCEPT) {
       handle_accept(outgoingEdge);
    } else if(msg->getSubType() == SpanningTree::REJECT) {
       handle_reject(outgoingEdge);
    } else if(msg->getSubType() == SpanningTree::REPORT) {
       handle_report(outgoingEdge,
                     msg->getBestWeight());
    } else if(msg->getSubType() == SpanningTree::CHANGEROOT) {
       change_root();
    } else if(msg->getSubType() == SpanningTree::DOWNTREE_BROADCAST) {
            lengthVectorList = msg->getLengthVectorCordinates();
        for(int i = 0 ; i < lengthVectorList; i++) {
            vectorList[i] = msg->getVectorCordinates(i);
        }

       handle_downtree_broadcast(msg->getSrcNodeId(),
                                 msg->getDepth());

    } else if(msg->getSubType() == SpanningTree::UPTREE_REPLY) {

       maxDepth =  msg->getDepth();

       if(not fullBroadcast) {
           stateEdges[outgoingEdge].numOfChildren = msg->getIndexChildrenIds();
           for(int i = 0; i < msg->getIndexChildrenIds(); i++) {
               stateEdges[outgoingEdge].children[i] = msg->getListChildrenIds(i);
           }
       }


       handle_uptree_reply(outgoingEdge, maxDepth);

    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY) {
       handle_root_query(outgoingEdge,
                         msg->getSpanningTreeId());

    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY_ACCEPT) {
        handle_root_accept();
    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY_REJECT) {
        isRoot = false;
    } else if(msg->getSubType() == SpanningTree::WEIGHT_REQUEST) {
        handle_weight_request(outgoingEdge,
                              msg->getSpanningTreeId(),
                              msg->getSpanningTreeIndexList());

    } else if(msg->getSubType() == SpanningTree::WEIGHT_REPONSE) {
        handle_weight_response(msg->getWeightEdgeSpanningTree(),
                               msg->getSpanningTreeIndexList(),
                               msg->getNodeId());

    }
}

void SpanningTree::handle_weight_request(int outgoingEdge, int spanningTreeId, int index) {
    int weight;
    if(spanningTreeNodeId >= spanningTreeId) {
        weight = spanningTreeNodeId - spanningTreeId;
    } else {
        weight = spanningTreeId - spanningTreeNodeId;
    }
    update_message_buf(SpanningTree::weight_response(weight, index, nodeId, spanningTreeIndex), outgoingEdge);
}

void SpanningTree::handle_weight_response(int weight, int index, int nodeId) {
    stateEdges[index].weight = weight;
    stateEdges[index].nodeId = nodeId;
    sent_requests--;

    if(sent_requests == 0) {
        edgesWeightUpdated = true;
        start_building_tree();
        if(initialQueueIndex > -1){
            msgDelay = 0;
            handle_initial_queue();
            return;
        }
    }
}

void SpanningTree::handle_connect(int outgoingEdge, int level) {
    handle_connect_msg++;

    int pos = outgoingEdge;

    if(level < fragmentLevel) {
        stateEdges[pos].state = SpanningTree::EDGESTATE_IN_MST;
        update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, stateNode, spanningTreeIndex), outgoingEdge);
        if (stateNode == SpanningTree::STATE_FIND) {
            findCount++;
        }
    } else {
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST) {
            QueuedMessage* connectElement = new QueuedMessage();
            connectElement->set_connect_message(SpanningTree::CONNECT, outgoingEdge, level);
            connectQueue[connectQueueIndex] = connectElement;
            connectQueueIndex++;
        } else {
            fragmentName = stateEdges[pos].weight;
            fragmentLevel = fragmentLevel + 1;
            update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, SpanningTree::STATE_FIND, spanningTreeIndex), outgoingEdge);
        }
    }
}

void SpanningTree::handle_initiate(int outgoingEdge, int level, int fragName, int nodState) {
    fragmentLevel = level;
    fragmentName = fragName;
    stateNode = nodState;

    set_edge_towards_root(outgoingEdge);

    bestEdge = -1;
    bestWeight = std::numeric_limits<int>::max();

    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].outgoingEdge != outgoingEdge) {
            if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
                update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, stateNode, spanningTreeIndex), stateEdges[i].outgoingEdge);
                if(nodState == SpanningTree::STATE_FIND) {
                    findCount++;
                }
            }
        }
    }
    if(nodState == SpanningTree::STATE_FIND) {
        test();
    }

}

void SpanningTree::handle_test(int outgoingEdge, int level, int fragName) {

    if(level > fragmentLevel) {
        QueuedMessage* testElement = new QueuedMessage();
        testElement->set_test_message(SpanningTree::TEST, outgoingEdge, level, fragName);
        testQueue[testQueueIndex] = testElement;
        testQueueIndex++;
    } else {
        if(fragName != fragmentName) {
            update_message_buf(SpanningTree::accept(spanningTreeIndex), outgoingEdge);
        } else {
            int pos = outgoingEdge;
            if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST) {
                stateEdges[pos].state = SpanningTree::EDGESTATE_NOT_IN_MST;
            }

            if(testEdge != pos) {
                update_message_buf(SpanningTree::reject(spanningTreeIndex), outgoingEdge);
            } else {
                test();
            }
        }
    }
}

void SpanningTree::handle_accept(int outgoingEdge) {
    testEdge = -1;
    //int pos = find_edge_in_stateEdge(outgoingEdge);
    int pos = outgoingEdge;
    if(stateEdges[pos].weight < bestWeight) {
        bestEdge = pos;
        bestWeight = stateEdges[pos].weight;
    }
    report();
}

void SpanningTree::handle_reject(int outgoingEdge) {
    //int pos = find_edge_in_stateEdge(outgoingEdge);
    int pos = outgoingEdge;
    if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST and edgeTowardsRoot != outgoingEdge) {
        stateEdges[pos].state = SpanningTree::EDGESTATE_NOT_IN_MST;
        testEdge = -1;

    }
    test();
}

void SpanningTree::handle_report(int outgoingEdge, int bestW) {
    if(outgoingEdge != edgeTowardsRoot) {
        findCount--;
        if(bestW < bestWeight) {
            bestWeight = bestW;
            bestEdge = outgoingEdge;
        }
        report();
    } else {
        if(stateNode == SpanningTree::STATE_FIND) {
            QueuedMessage* reportElement = new QueuedMessage();
            reportElement->set_report_message(SpanningTree::REPORT, outgoingEdge, bestW);
            reportQueue[reportQueueIndex] = reportElement;
            reportQueueIndex++;


        } else {
            if(bestW > bestWeight) {
                change_root();
            } else {
                if(bestW == std::numeric_limits<int>::max()) {
                    check_root();
                }
            }
        }
    }
}

void SpanningTree::handle_downtree_broadcast(int pNodeId, int parentDepth) {
    currentDepth = parentDepth + 1;
    parentNodeId = pNodeId;

    if(not broadcast_down_stream()) {
        broadcast_up_stream(currentDepth);
        // update_neighbours();
    }
}

void SpanningTree::handle_uptree_reply(int outgoingEdge, int maxDepth) {
    msgSentDownStream--;

    bool broadcast_up = true;
    if(msgSentDownStream == 0) {
        broadcast_up = broadcast_up_stream(maxDepth);
        fullBroadcast = true;
    }

    if(not broadcast_up) {
        aggregate_children_list();
        // broadcast();
    }
}

void SpanningTree::handle_root_query(int outgoingEdge, int node_id) {
    if(outgoingEdge == stateEdges[edgeTowardsRoot].outgoingEdge and spanningTreeNodeId > node_id) {
        update_message_buf(SpanningTree::root_query_accept(spanningTreeIndex), outgoingEdge);
    } else {
        update_message_buf(SpanningTree::root_query_reject(spanningTreeIndex), outgoingEdge);
    }
}

void SpanningTree::handle_root_accept(void) {
    isRoot = true;
    currentDepth = 0;
    broadcast();
}

void SpanningTree::test(void) {
    testCounter++;
    int bestW = std::numeric_limits<int>::max();
    bool noEdgeInQMST = true;
    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].state == SpanningTree::EDGESTATE_Q_MST and
                stateEdges[i].outgoingEdge != edgeTowardsRoot) {
            noEdgeInQMST = false;
            if(bestW > stateEdges[i].weight){
                testEdge = stateEdges[i].outgoingEdge;
                bestW = stateEdges[i].weight;
            }
        }
    }

    if (noEdgeInQMST) {
        testEdge = -1;
    }

    if(testEdge > -1) {
        update_message_buf(SpanningTree::test(fragmentLevel, fragmentName, spanningTreeIndex), testEdge);
    } else {
        testEdge = -1;
        report();
    }
}

void SpanningTree::report(void) {
    if(findCount == 0 and testEdge == -1) {
        stateNode = SpanningTree::STATE_FOUND;
        update_message_buf(SpanningTree::report(bestWeight, spanningTreeIndex), edgeTowardsRoot);
    }
}

void SpanningTree::change_root(void) {
    // fix?
    if(stateEdges[bestEdge].state == SpanningTree::EDGESTATE_NOT_IN_MST) {
        bestEdge = -1;
    }

    if(bestEdge > -1) {
        // int pos = find_edge_in_stateEdge(bestEdge);
        int pos = bestEdge;
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_IN_MST){
            update_message_buf(SpanningTree::change_root_msg(spanningTreeIndex), bestEdge);
        } else {
            update_message_buf(SpanningTree::connect(fragmentLevel, spanningTreeIndex), bestEdge);
            stateEdges[pos].state = SpanningTree::EDGESTATE_IN_MST;
        }
    }

}

int SpanningTree::find_minimum_weight_edge(void) {
    int min_weight = stateEdges[0].weight;
    int pos = 0;
    for(int i = 1; i < numConnectedNodes; i++) {
        if(min_weight > stateEdges[i].weight) {
            min_weight = stateEdges[i].weight;
            pos = i;
        }
    }
    return pos;
}

int SpanningTree::find_edge_in_stateEdge(int incomingEdge) {
    int pos = -1;
    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].outgoingEdge == incomingEdge) {
            return i;
        }
    }
    return pos;
}

void SpanningTree::check_root(void) {
    update_message_buf(SpanningTree::root_query(spanningTreeNodeId, spanningTreeIndex), edgeTowardsRoot);
}

void SpanningTree::set_edge_towards_root(int outgoingEdge) {
    edgeTowardsRoot = outgoingEdge;

    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].outgoingEdge == edgeTowardsRoot) {
            stateEdges[i].edgeTowardsRoot = true;
        } else if(stateEdges[i].edgeTowardsRoot and
                  stateEdges[i].outgoingEdge != edgeTowardsRoot) {
            stateEdges[i].edgeTowardsRoot = false;
        }
    }
}


void SpanningTree::broadcast(void) {
    if(msgSentDownStream == 0 and isRoot == true) {
        broadcast_down_stream();
    }
}

bool SpanningTree::broadcast_down_stream() {
    bool msgSent = false;

    if(isRoot) {
        lengthVectorList = 0;

        for(int i = 0; i < numConnectedNodes; i++) {
                    if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
                        vectorList[lengthVectorList] = i + 1;


                        update_message_buf(SpanningTree::broadcast_down_tree(nodeId, currentDepth, spanningTreeIndex, (lengthVectorList + 1), &vectorList[0]),
                                            stateEdges[i].outgoingEdge);
                        msgSent = true;
                        msgSentDownStream++;


                    }
                }
    } else {
        int vector = 1;

        for(int i = 0; i < numConnectedNodes; i++) {
            if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                    stateEdges[i].outgoingEdge != edgeTowardsRoot) {
                vectorList[lengthVectorList] = vector;
                vector++;

                update_message_buf(SpanningTree::broadcast_down_tree(nodeId, currentDepth, spanningTreeIndex, (lengthVectorList + 1), &vectorList[0]),
                                    stateEdges[i].outgoingEdge);
                msgSent = true;
                msgSentDownStream++;
            }
        }
    }

    return msgSent;
}

int SpanningTree::update_linked_nodes(LinkedNode *linkedNodes) {
    int k = 0;
    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
            (linkedNodes + k)->set_connected_node_id(stateEdges[i].nodeId);
            (linkedNodes + k)->set_connecting_edge(stateEdges[i].outgoingEdge);
            (linkedNodes + k)->set_edge_towards_root(stateEdges[i].edgeTowardsRoot);
            (linkedNodes + k)->set_number_of_children(stateEdges[i].numOfChildren);
            (linkedNodes + k)->set_children(stateEdges[i].children);
            k++;
        }
    }

    linkedNodesUpdated = true;
    return k;
}

std::string SpanningTree::state_edges_to_string(void) {

    std::string res = "";


    for(int i = 0; i < numConnectedNodes; i++) {
        char buff[300];
        sprintf(buff,"edgeState: %d \n outgoingEdge: %d \n edgeTowardsRoot: %d \n numOfChildren: %d \n",stateEdges[i].state, stateEdges[i].outgoingEdge, stateEdges[i].edgeTowardsRoot, stateEdges[i].numOfChildren );
        res = res + buff;
    }

    return res;

}

void SpanningTree::check_queued_messages(void) {

    if(initialQueueIndex > -1 and edgesWeightUpdated){
        handle_initial_queue();
    }

    if(connectQueueIndex >= 0) {
        for(int i = 0; i < connectQueueIndex; i++) {
            int edge = connectQueue[i]->get_edge();
            int lvl = connectQueue[i]->get_level();

            if(check_connect_queue_conditions(edge, lvl)) {
                handle_connect(edge, lvl);

                //Remove from list and shift list
                for(int j = i; j < connectQueueIndex; j++) {
                    connectQueue[j] = connectQueue[j + 1];
                }
                connectQueueIndex--;
                if(connectQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }

    if(reportQueueIndex >= 0) {
        for(int i = 0; i < reportQueueIndex; i++) {
            int edge = reportQueue[i]->get_edge();
            int weight = reportQueue[i]->get_weight();

            if(check_report_queue_conditions(edge, weight)) {
                handle_report(edge, weight);

                //Remove from list and shift list
                for(int j = i; j < reportQueueIndex; j++) {
                    reportQueue[j] = reportQueue[j + 1];
                }
                reportQueueIndex--;
                if(reportQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }

    if(testQueueIndex >= 0) {
        for(int i = 0; i < testQueueIndex; i++) {
            int lvl = testQueue[i]->get_level();

            if(check_test_queue_conditions(lvl)) {
                handle_test(testQueue[i]->get_edge(), lvl, testQueue[i]->get_fragment_name());

                //Remove from list and shift list
                for(int j = i; j < testQueueIndex; j++) {
                    testQueue[j] = testQueue[j + 1];
                }
                testQueueIndex--;
                if(testQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }
}

// Returns true if message can be removed from queue
bool SpanningTree::check_connect_queue_conditions(int edge, int lvl) {
    if(lvl < fragmentLevel) {
        return true;
    } else {
        //int pos = find_edge_in_stateEdge(edge);
        int pos = edge;
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST){
            return false;
        } else {
            return true;
        }
    }
}

// Returns true if message can be removed from queue
bool SpanningTree::check_report_queue_conditions(int edge, int weight) {
    if (edge != edgeTowardsRoot){
        return true;
    } else {
        if(stateNode == SpanningTree::STATE_FIND) {
            return false;
        } else {
            return true;
        }
    }
}

// Return true if message can be removed from queue
bool SpanningTree::check_test_queue_conditions(int level) {
    if(level > fragmentLevel) {
        return false;
    } else {
        return true;
    }
}

bool SpanningTree::broadcast_up_stream(int maxDepth) {
    bool msgSent = false;
    if(not isRoot){
        aggregate_children_list();
        for(int i = 0; i < numConnectedNodes; i++) {
                    if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                            (stateEdges[i].outgoingEdge == edgeTowardsRoot)) {
                        update_message_buf(SpanningTree::broadcast_up_tree(maxDepth,
                                                                            indexChildrenNodes,
                                                                            listChildrenNodes,
                                                                            spanningTreeIndex),
                                            stateEdges[i].outgoingEdge);
                        msgSent = true;
                    }
                }
    }

    return msgSent;
}

int SpanningTree::number_of_children_check(void) {
    int numChildren = 0;

    if(isRoot) return 0;

    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                stateEdges[i].outgoingEdge != edgeTowardsRoot) {
            numChildren++;
        }
    }

    return numChildren;
}

void SpanningTree::aggregate_children_list(void) {

    if(not fullBroadcast) {
        listChildrenNodes[indexChildrenNodes] = nodeId;
        indexChildrenNodes++;

        for(int i = 0; i < numConnectedNodes; i++) {
            if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                    (stateEdges[i].outgoingEdge != edgeTowardsRoot or isRoot)) {
                for(int j = 0; j < stateEdges[i].numOfChildren; j++) {
                    listChildrenNodes[j + indexChildrenNodes] = stateEdges[i].children[j];
                }
                indexChildrenNodes += stateEdges[i].numOfChildren;
            }
        }
    }
}



void SpanningTree::send_inspection(int num, int inspect) {
    for(int i = 0; i < num; i++) {
        update_message_buf(SpanningTree::inspection(inspect), i);
    }
}

// TODO add msgDelay to the bufferedmessage process
//  or add it later to the game somehow the way it works now
//  is a little shitty
void SpanningTree::update_message_buf(BasicMessage *msg, int outgoingEdge) {
    msgDelay = msgDelay + 0.05;
    BufferedMessage * bufMsg = new BufferedMessage(msg, outgoingEdge, msgDelay);
    msgBuf.addMessage(bufMsg);
}

BasicMessage * SpanningTree::inspection(int inspect) {
    char msgname[80];
    sprintf(msgname, "Inspection %d", inspect);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::INSPECTION);
    msg->setInspection(inspect);

    return msg;
}

BasicMessage * SpanningTree::weight_request(int spanningTreeId, int indexList, int index) {
    char msgname[40];
    sprintf(msgname, "weight request");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::WEIGHT_REQUEST);
    msg->setSpanningTreeId(spanningTreeId);
    msg->setSpanningTreeIndexList(indexList);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::weight_response(int weight, int indexListRequest, int nodeId, int index) {
    char msgname[80];
    sprintf(msgname, "weight response: %d", weight);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::WEIGHT_REPONSE);
    msg->setNodeId(nodeId);
    msg->setWeightEdgeSpanningTree(weight);
    msg->setSpanningTreeIndexList(indexListRequest);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::connect(int level, int index) {
    char msgname[40];
    sprintf(msgname, "Connect message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::CONNECT);
    msg->setLevelNode(level);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::initiate(int level, int fragmentName, int stateNode, int index) {
    char msgname[80];
    sprintf(msgname, "Initiate message, lvl: %d, fragN: %d", level, fragmentName);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::INITIATE);
    msg->setLevelNode(level); // LN
    msg->setFragmentName(fragmentName); //FN
    msg->setNodeState(stateNode); //SN

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::test(int level, int fragmentName, int index) {
    char msgname[40];
    sprintf(msgname, "Test message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::TEST);
    msg->setLevelNode(level); // LN
    msg->setFragmentName(fragmentName); //FN

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::accept(int index) {
    char msgname[40];
    sprintf(msgname, "Accept message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ACCEPT);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::reject(int index) {
    char msgname[40];
    sprintf(msgname, "Reject message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::REJECT);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::report(int bestW, int index) {
    char msgname[40];
    sprintf(msgname, "Report message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::REPORT);
    msg->setBestWeight(bestW);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::change_root_msg(int index) {
    char msgname[40];
    sprintf(msgname, "Change Root message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::CHANGEROOT);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::broadcast_down_tree(int nodeId, int depth, int index, int lengthVectorCordinates, int *vectorCordinateList) {
    char msgname[40];
    sprintf(msgname, "BroadCast message tree: %d", index);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::DOWNTREE_BROADCAST);
    msg->setSrcNodeId(nodeId);
    msg->setDepth(depth);
    msg->setSpanningTreeIndex(index);
    msg->setLengthVectorCordinates(lengthVectorCordinates);
    for(int i = 0; i < lengthVectorCordinates; i++ ){
        msg->setVectorCordinates(i, *(vectorCordinateList + i));
    }

    return msg;
}

BasicMessage * SpanningTree::broadcast_up_tree(int maxDepth, int indexChildList, int *childList, int index) {


    std::string msgName = "";
    for(int i = 0; i < indexChildList; i++) {
        msgName += std::to_string(childList[i]) + " , ";
    }

    char msgname[200];
    sprintf(msgname, "BroadCast Reply message %s", msgName.c_str());
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::UPTREE_REPLY);
    for(int i = 0; i < indexChildList; i++) {
        msg->setListChildrenIds(i, childList[i]);
    }
    msg->setDepth(maxDepth);
    msg->setIndexChildrenIds(indexChildList);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::root_query(int spanningTreeId, int index) {
    char msgname[40];
    sprintf(msgname, "root query");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY);
    msg->setSpanningTreeId(spanningTreeId);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::root_query_accept(int index) {
    char msgname[40];
    sprintf(msgname, "root accept");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY_ACCEPT);

    msg->setSpanningTreeIndex(index);

    return msg;
}

BasicMessage * SpanningTree::root_query_reject(int index) {
    char msgname[40];
    sprintf(msgname, "root reject");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY_REJECT);

    msg->setSpanningTreeIndex(index);

    return msg;
}

