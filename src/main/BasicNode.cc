#include "./BasicNode.h"

BasicNode::BasicNode()
{
    nodeId = 0;
    spanningTreeSearchIndex = 0;

    // Set timer pointer to nullptr
    event = broadcast_tree = nullptr;

    runningSimulation = false;

    firstSend = false;
    nodeStateLoaded = false;
    simulationSetup = false;

    transEventIndex = 0;
    periodicUpdate = 0;
}

BasicNode::~BasicNode()
{
//    cancelAndDelete(event);
    delete broadcast_tree;
}

void BasicNode::initialize()
{
    initialize_parameters();
    start_message_timer();
    initialize_stats();
}

void BasicNode::initialize_parameters()
{
    // Get node ID or generate randomly
    if(true) {
        std::string id_string = this->parseNodeID(getFullName());
        nodeId = std::stoi(id_string);
    } else {
        nodeId = rand();
    }

    district_initialization();
    IO_initialization();
    payment_channel_initialization();

    inputReader.set_node_id(nodeId);
    numOfTransEvents = inputReader.read_transaction_file(&transactionEvent[0], endTransactionTime);



    if(numOfTransEvents == -2) {
        EV << "CANT OPEN TRANSACTIONS.TXT \n";
    } else {
        EV << "node: " << nodeId << " has num of transactions: " << numOfTransEvents << "\n";
    }

//    for(int i = 0; i < numOfTransEvents; i++) {
//        EV << "Sender: " << nodeId << " " << transactionEvent[i].get_string();
//    }

    if(BasicNode::RUN_SPANNINGTREE) {
        state = BasicNode::SPANNING_TREE_STATE;

        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
            spanningTrees[i].set_node_id(nodeId);
            spanningTrees[i].set_spanning_tree_index(i);
            spanningTrees[i].set_num_neighbours(gateSize("out"));
        }
        spanningTrees[spanningTreeSearchIndex].wake_up();
        runningSimulation = false;
        runningSpanningTree = true;
    } else {
        state = BasicNode::SIMULATION_STATE;
        District *dp = &district;
        std::string res = saveState.loadstate(dp);
        runningSimulation = true;
        runningSpanningTree = false;
    }

}

void BasicNode::district_initialization(void) {
    district.set_num_of_connections(gateSize("out"));
    district.set_num_of_neighbourhoods(NUM_OF_TREES);
    district.set_node_id(nodeId);
    coordinateMsgsSent = 0;
    coordinateMsgsReceived = 0;
}

void BasicNode::IO_initialization(void) {
    fileWriter.set_node_id(nodeId, gateSize("out"));
//    if(nodeId == 1) {
//        fileWriter.initialize_file();
//    }

    // Save state
    saveState.set_node_id_and_amount_of_nodes(nodeId, BasicNode::NUM_OF_NODES);

}

void BasicNode::payment_channel_initialization(void) {
    paymentChannel.set_district(&district);
    paymentChannel.set_node_id(nodeId);
    paymentChannel.set_stats(&stats);
}

void BasicNode::initialize_stats(void) {
    msgSendCountVector.setName("msg_Sent");
    sptMsgCountVector.setName("spt_Sent");
    pcMsgCountVector.setName("pc_Sent");

    msgSendCount = 0;
    sptMsgCount = 0;
    pcMsgCount = 0;

    WATCH(msgSendCount);
    WATCH(sptMsgCount);
    WATCH(pcMsgCount);
}

void BasicNode::start_message_timer()
{
    event = new cMessage("event");
    scheduleAt((simTime() + 0.10), event);
}

void BasicNode::set_timer_transactions(void)
{
    if(transEventIndex < numOfTransEvents) {
        startTransaction = new cMessage("startTransaction");
        scheduleAt((startTimeTransactions + transactionEvent[transEventIndex].get_time()), startTransaction);
    }
}

void BasicNode::sendMessagesFromBuffer(void)
{
    msgSendCount++;
    msgSendCountVector.record(msgSendCount);
    sendSpanningTreeMessages();
    sendTransactionMessages();
    sendDistrictMessages();
}

void BasicNode::broadcastLeaderRequest()
{
    leader_election.broadcastLeaderRequest();
}

void BasicNode::handleMessage(cMessage *msg) {

    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    if (msg == event) {
        delete msg;
        internal_message_handling();


    } else if(msg == startTransaction) {

        paymentChannel.multi_path_send(transactionEvent[transEventIndex].get_destination(), transactionEvent[transEventIndex].get_amount(), 4);
        transEventIndex++;
        set_timer_transactions();
        delete msg;

    } else {
        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
        external_message_handling(basicmsg, msg);
        delete basicmsg;
    }
}

void BasicNode::internal_message_handling(void) {
    sendMessagesFromBuffer();

    if(state != BasicNode::FINISHED_STATE) {
        start_message_timer();
    }

    if(state == BasicNode::SPANNING_TREE_STATE) {
        internal_spanning_tree_handling();
    } else if(state == BasicNode::COORDINATE_SHARING_STATE) {
        check_coordinate_handling();
    } else if(state == BasicNode::SIMULATION_STATE) {

        if(simTime().dbl() > periodicUpdate) {
            EV << nodeId << "," << stats.transactionsCompleted << "\n";
            periodicUpdate += 50;
        }

        if(simulationSetup == false) {
            startTimeTransactions = simTime().dbl();
            set_timer_transactions();
            endTransactionTime = endTransactionTime + startTimeTransactions;
            periodicUpdate = startTimeTransactions + 50;
            simulationSetup = true;


            LinkCapacity *cp = district.get_all_link_capacities();

            int s = inputReader.read_capacity_file(nodeId, gateSize("out"), cp);

            EV << "NodeId: " << nodeId << " has capacities changed: " << s<< " current caps:\n";


            for(int i = 0; i < gateSize("out"); i++) {
                std::string res = cp[i].to_file();
                EV << res <<"\n";
            }
            EV << "numofTrans: " << paymentChannel.get_current_transaction_size() << "\n";

        } else {
            paymentChannel.check_for_dead_transactions();
            if(simTime().dbl() + 70 > endTransactionTime) {
                state = BasicNode::FINISHED_STATE;
            }
        }
    }

}

void BasicNode::internal_spanning_tree_handling(void) {
    spanningTrees[spanningTreeSearchIndex].check_queued_messages();
    if(spanningTrees[spanningTreeSearchIndex].is_node_root() and
            spanningTrees[spanningTreeSearchIndex].full_broadcast_finished()) {

        spanningTrees[spanningTreeSearchIndex].search_for_next_tree(spanningTreeSearchIndex);
        spanningTreeSearchIndex++;

        if(spanningTreeSearchIndex >= NUM_OF_TREES) {
            SpanningTree *p = spanningTrees;
            district.update_linked_nodes_from_spanningtree(p);
            coordinateMsgsSent = district.neighbours_coordinates_inquiry();
            state = BasicNode::COORDINATE_SHARING_STATE;
            if(SAVE_STATE){
                District *dp = &district;
                saveState.save(dp);
            }
        } else {
            spanningTrees[spanningTreeSearchIndex].wake_up();
        }
    }
}

void BasicNode::external_message_handling(BasicMessage *basicmsg, cMessage *msg) {
    if(basicmsg->getType() == LeaderElection::LEADER_MSG) {
        leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
    } else if(basicmsg->getType() == SpanningTree::MESSAGE_TYPE){
        process_spanning_tree_msg(basicmsg, msg);
    } else if(basicmsg->getType() == PaymentChannel::MESSAGE_TYPE) {
        std::string res = paymentChannel.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
        // EV << "Node: " << nodeId << " handle_message: " << res << "\n";
    } else if(basicmsg->getType() == Neighbours::MESSAGE_TYPE) {
        if(basicmsg->getSubType() == Neighbours::SEND_COORDINATES) {
            coordinateMsgsReceived++;
            check_coordinate_handling();
        }
        district.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
    }
}

void BasicNode::process_spanning_tree_msg(BasicMessage *basicmsg, cMessage *msg) {

    if(basicmsg->getSubType() == SpanningTree::SEARCH_NEXT_TREE) {
        spanningTreeSearchIndex = basicmsg->getNextTreeIndex() + 1;

        if(spanningTreeSearchIndex == NUM_OF_TREES) {
            SpanningTree *p = spanningTrees;
            district.update_linked_nodes_from_spanningtree(p);
            coordinateMsgsSent = district.neighbours_coordinates_inquiry();
            state = BasicNode::COORDINATE_SHARING_STATE;

            if(SAVE_STATE){
                District *dp = &district;
                EV_FATAL << saveState.save(dp) << "\n";
            }
        }
    }
    spanningTrees[basicmsg->getSpanningTreeIndex()].handle_message(basicmsg, msg->getArrivalGate()->getIndex(), simTime());
}

void BasicNode::check_coordinate_handling(void) {
    if(coordinateMsgsSent > 0) {
        if(coordinateMsgsSent == coordinateMsgsReceived) {
            state = BasicNode::SIMULATION_STATE;
        }
    }
}
// based on https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// Parses the Node_#ID and returns a string of #ID

std::string BasicNode::parseNodeID(const char* nodeName)
{
   std::string node_ID = nodeName;
   std::string delimiter = "node_";

   size_t pos = 0;

   pos = node_ID.find(delimiter);

   node_ID.erase(0, pos + delimiter.length());

   return node_ID;
}

void BasicNode::leaderInitialization(void) {
     leader_election.setId(nodeId);
     //leader_election.setAmountNeighbours(connected_neighbours.get_amount_of_neighbours());
     broadcastLeaderRequest();
}

void BasicNode::sendLeaderMessages(void) {
    int msg_in_leader_buffer = leader_election.getMessageCount();

    // EV << "Number of messages in leader buffer: " << msg_in_leader_buffer << "\n";

    for (int i = 0; i < msg_in_leader_buffer; i++)
    {
        BufferedMessage * bufMsg = leader_election.getMessage();
        send(bufMsg->get_message(), "out", bufMsg->get_out_gate_int());
        delete(bufMsg);
    }
}

void BasicNode::sendSpanningTreeMessages(void) {
    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        int msgSpanningTrees = spanningTrees[i].get_message_count();

        for (int j = 0; j < msgSpanningTrees; j++)
        {
            BufferedMessage * bufMsg = spanningTrees[i].get_message();

            BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());

            sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
            delete(bufMsg);
            sptMsgCount++;
            sptMsgCountVector.record(sptMsgCount);
        }
    }
}

void BasicNode::sendTransactionMessages(void) {
    int msgTransaction = paymentChannel.get_message_count();

    for (int i = 0; i < msgTransaction; i++) {
        BufferedMessage * bufMsg = paymentChannel.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());
        if(bufMsg->get_out_gate_int() < 0 or bufMsg->get_out_gate_int() > gateSize("out")) {
            EV << "Node: " << nodeId << " tried to send type message: " << basicmsg->getSubType() << "\n";
        }else {
            sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
        }

        delete(bufMsg);
        pcMsgCount++;
        pcMsgCountVector.record(pcMsgCount);
        //pcMsgCountVector.collect(pcMsgCount);
    }
}

void BasicNode::sendDistrictMessages(void) {
    int msgTransaction = district.get_message_count();

    for (int i = 0; i < msgTransaction; i++) {
        BufferedMessage * bufMsg = district.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());

        sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
        delete(bufMsg);
    }
}

void BasicNode::check_stat_flags(void) {

}

void BasicNode::finish() {

    EV << "Node id: " << nodeId << " has run transactions: " << numOfTransEvents << " number of completed: " << stats.transactionsCompleted << " number of failed: " << stats.transactionsFailed << "\n";
}
