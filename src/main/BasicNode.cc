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

}

BasicNode::~BasicNode()
{
    cancelAndDelete(event);
    delete broadcast_tree;
}

void BasicNode::initialize()
{
    initialize_parameters();
    start_message_timer();
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
    if(nodeId == 1) {
        fileWriter.initialize_file();
    }

    // Save state
    saveState.set_node_id_and_amount_of_nodes(nodeId, BasicNode::NUM_OF_NODES);

}

void BasicNode::payment_channel_initialization(void) {
    paymentChannel.set_district(&district);
    paymentChannel.set_node_id(nodeId);
}

void BasicNode::start_message_timer()
{
    event = new cMessage("event");
    scheduleAt((simTime() + 0.10), event);
}

void BasicNode::sendMessagesFromBuffer(void)
{
    sendSpanningTreeMessages();
    sendTransactionMessages();
    sendDistrictMessages();
}

void BasicNode::broadcastLeaderRequest()
{
    leader_election.broadcastLeaderRequest();
}

void BasicNode::handleMessage(cMessage *msg)
{

    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    if (msg == event)
    {
        delete msg;
        internal_message_handling();

        if(paymentChannel.transactions.get_current_trans() > 0) {

            EV << "NodeId: " << nodeId << " has currentTransactions: " << paymentChannel.transactions.get_current_trans() <<"\n";

        }
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
        EV_FATAL << "Node: " << nodeId << " is trying to run the simulation";
        run_simulation();
//        if(nodeId == 19 or nodeId == 6) {
//           district.set_all_capacities(1.0);
//        }
//        if(nodeId == 17 and simTime() == 61.0) {
//            std::string res = paymentChannel.multi_path_send(0, 75, 4);
//        }
        //std::string send = paymentChannel.multi_path_send(receiver, amount, 3);

        //state = BasicNode::FINISHED_STATE;
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
        EV << "Node: " << nodeId << " has received: " << res << "\n";
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

void BasicNode::run_simulation(void) {

    int sendTransactionProbability = rand();

    // 0.05 % chance of sending
    if(sendTransactionProbability < 10000000) {
        int amountProbability = rand() % 100;
        EV << amountProbability;
        double amount = 1;

        if(amountProbability < 5) {
            amount = 1000;
        } else if( amountProbability < 30) {
            amount = 900;
        } else if( amountProbability < 60) {
            amount = 400;
        }

        int nodeProbability = rand() % 100;
        int receiver = 0;

        if(nodeProbability < 5) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 2;
            } else {
                receiver = nodeId + 2;
            }
        } else if(nodeProbability < 25) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 3;
            } else {
                receiver = nodeId + 3;
            }
        } else if(nodeProbability < 45) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 4;
            } else {
                receiver = nodeId + 4;
            }
        } else if(nodeProbability < 65) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 5;
            } else {
                receiver = nodeId + 5;
            }
        } else if(nodeProbability < 85) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 6;
            } else {
                receiver = nodeId + 6;
            }
        } else if(nodeProbability < 95) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 7;
            } else {
                receiver = nodeId + 7;
            }
        } else if(nodeProbability < 100) {
            if(nodeProbability % 2 == 0) {
                receiver = nodeId - 8;
            } else {
                receiver = nodeId + 8;
            }
        }

        if(receiver < 0) {
            receiver = nodeId - receiver;
        }

        if(receiver > 22) {
            receiver = receiver - nodeId;
        }

        if(receiver < 0 or receiver > 22) {
            return;
        }

        EV << "Node: " << nodeId << " \nSending Amount: " << amount << " \nReceiver: " << receiver << "\n";

        if(nodeId == receiver) {
            EV << "Trying to send to self";
            return;
        }

        std::string send = paymentChannel.multi_path_send(receiver, amount, 3);
        EV << "Node: " << nodeId << " sending edge: " << send << "\n";

    }

    if(paymentChannel.get_current_transaction_index() > 1) {

            //EV << "node: " << nodeId << " has transaction string with capacities: " << transaction.capacities_to_string() << " \n delay:" << transaction.delay_to_string() << "\n";
    }

//    std::string time = simTime().str();

    //Gather capacities on the lines
//    std::string capacities = "";
//    for(int i = 0; i < gateSize("out"); i++) {
//        capacities += linkCapacities[i].to_file();
//    }

//    fileWriter.update_variables(time,
//                                paymentChannel.get_num_completed_transactions(),
//                                paymentChannel.get_num_of_total_transactions(),
//                                paymentChannel.get_num_forwarded_transactions(),
//                                paymentChannel.get_num_forwarded_completed_transactions(),
//                                paymentChannel.get_current_transaction_id(),
//                                paymentChannel.get_failed_transactions(),
//                                paymentChannel.get_capacity_failure(),
//                                paymentChannel.get_network_delay(),
//                                paymentChannel.get_crypto_delay(),
//                                paymentChannel.get_os_delay(),
//                                capacities);

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

        }
    }
}

void BasicNode::sendTransactionMessages(void) {
    int msgTransaction = paymentChannel.get_message_count();

    for (int i = 0; i < msgTransaction; i++) {
        BufferedMessage * bufMsg = paymentChannel.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());

        sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
        delete(bufMsg);
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
