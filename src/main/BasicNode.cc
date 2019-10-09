#ifndef BASICNODE_H_
#define BASICNODE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <cstdlib>

#include "../paymentchannel/PaymentChannel.h"
#include "../msg/basicmsg_m.h"
#include "../sync/NodeClock.h"

#include "../connection/District.h"
#include "../connection/Neighbours.h"

#include "../msg/outgoing_buf/BufferedMessage.h"

#include "../alg/leader_election/LeaderElection.h"
#include "../alg/spanning_tree/SpanningTree.h"

#include "../output/FileWriter.h"
#include "../output/SaveState.h"

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    public:
        static const int NUM_OF_TREES = 10;
        static const int NUM_OF_NODES = 23;
        static const int STATIC_CAPACITY = 100;
        static const bool RUN_SPANNINGTREE = false;
        static const bool SAVE_STATE = false;
        static const bool SAVE_RESULTS = false;

        BasicNode();
        virtual ~BasicNode();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void start_message_timer();

        virtual void handleMessage(cMessage *msg) override;

        // Starts the search for a leader
        virtual void broadcastLeaderRequest();

    private:
        NodeClock clock;
        int nodeId;

        bool firstSend;
        bool nodeStateLoaded;
        bool startSimulation;


        // NUM of trees must be equal to the number of SpanningTree and Neighbours
        SpanningTree spanningTrees[NUM_OF_TREES];
        District district;

        LeaderElection leader_election;
        PaymentChannel paymentChannel;

        FileWriter fileWriter;
        SaveState saveState;

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual void sendMessagesFromBuffer(void);

        virtual std::string parseNodeID(const char* nodeName);

        void leaderInitialization(void);

        void sendLeaderMessages(void);
        void sendSpanningTreeMessages(void);
        void sendTransactionMessages(void);
        void sendDistrictMessages(void);

        void run_simulation(void);
};

Define_Module(BasicNode);

// TODO sendMessageBuffer periodically?
BasicNode::BasicNode()
{
    nodeId = 0;

    // Set timer pointer to nullptr
    event = broadcast_tree = nullptr;

    startSimulation = false;

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

    district.set_num_of_connections(gateSize("out"));
    district.set_num_of_neighbourhoods(NUM_OF_TREES);
    district.set_node_id(nodeId);

    fileWriter.set_node_id(nodeId, gateSize("out"));
//    if(nodeId == 1) {
//        fileWriter.initialize_file();
//    }

    if(SAVE_RESULTS) {

    }

    start_message_timer();

    // Save state
    saveState.set_node_id_and_amount_of_nodes(nodeId, BasicNode::NUM_OF_NODES);

    // Neighbours
    if(BasicNode::RUN_SPANNINGTREE) {
        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
            spanningTrees[i].set_node_id(nodeId);
            spanningTrees[i].set_spanning_tree_index(i);
            spanningTrees[i].set_num_neighbours(gateSize("out"));
            spanningTrees[i].wake_up();
        }
    } else {
        District *dp = &district;
        std::string res = saveState.loadstate(dp);
        startSimulation = true;
    }

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
        sendMessagesFromBuffer();
        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
            spanningTrees[i].check_queued_messages();
        }


        // Finish Initialization process
        if(simTime() > 50 and not startSimulation and RUN_SPANNINGTREE) {
            SpanningTree *p = spanningTrees;
            district.update_linked_nodes_from_spanningtree(p);
            district.neighbours_coordinates_inquiry();
            if(SAVE_STATE){
                District *dp = &district;
                saveState.save(dp);
            }
            startSimulation = true;
        }

        if(startSimulation) {

            if(nodeId == 17 and simTime() == 0.1) {
                EV << "Pre node: 17 " << district.get_neighbourhood(0)->get_upstream_linked_node(0, 10)->to_string();
                std::string res = paymentChannel.multi_path_send(0, 10, 2);
                EV << "Node 17: " << res << "\n";
            }
        }

        if(simTime() < 52) {
            start_message_timer();
        }

    } else {
        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
        if(basicmsg->getType() == LeaderElection::LEADER_MSG) {
            leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
        } else if(basicmsg->getType() == SpanningTree::MESSAGE_TYPE){
            spanningTrees[basicmsg->getSpanningTreeIndex()].handle_message(basicmsg, msg->getArrivalGate()->getIndex(), simTime());
        } else if(basicmsg->getType() == PaymentChannel::MESSAGE_TYPE) {
            std::string res = paymentChannel.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
        } else if(basicmsg->getType() == Neighbours::MESSAGE_TYPE) {
            district.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
        }
        delete basicmsg;
    }
}

// based on https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// Parses the Node_#ID and returns a string of #ID
std::string BasicNode::parseNodeID(const char* nodeName)
{
   std::string node_ID = nodeName;
   std::string delimiter = "_";

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
        double amount = 1;

        if(amountProbability < 5) {
            amount = 20;
        } else if( amountProbability < 30) {
            amount = 10;
        } else if( amountProbability < 60) {
            amount = 5;
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

        std::string send = paymentChannel.send(receiver, amount);
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

// TODO
// Fix or remove
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
        BufferedMessage * buf_msg = leader_election.getMessage();
        send(buf_msg->get_message(), "out", buf_msg->get_out_gate_int());
        delete(buf_msg);
    }
}

void BasicNode::sendSpanningTreeMessages(void) {
    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        int msgSpanningTrees = spanningTrees[i].get_message_count();

        for (int j = 0; j < msgSpanningTrees; j++)
        {
            BufferedMessage * bufMsg = spanningTrees[i].get_message();

            BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());
            EV << nodeId << ": SpanningTree sending message type: " << basicmsg->getSubType() << " to address: " << bufMsg->get_out_gate_int() << " \n";
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

    EV << "Node: " << nodeId << " Amount of district messages: " << msgTransaction << "\n";

    for (int i = 0; i < msgTransaction; i++) {
        BufferedMessage * bufMsg = district.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());

        sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
        delete(bufMsg);
    }
}

#endif
