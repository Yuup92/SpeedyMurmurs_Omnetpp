#include "./SaveState.h"

SaveState::SaveState() {
    nodeId = 0;
    nodesInSystem = 0;
    amountOfNeighbourhoods = 0;
    fileName = "";

    linkCapacityIndex = 0;

    updatedAmountNeighbourhoods = false;
}

void SaveState::set_node_id_and_amount_of_nodes(int id, int amountNodes) {
    nodeId = id;
    std::string dir("savestate");
    fileName = dir + "/N_" + std::to_string(nodeId);
    nodesInSystem = amountNodes;
}

std::string SaveState::save(District *district) {
    std::string res = "";
    std::string numNodes("NUMBERNODES:");
    std::string numNeigh("NUMBERNEIGHBOURHOODS:");

    res += "State saved: " + get_time();
    res += numNodes + std::to_string(nodesInSystem) + "\n";
    res += numNeigh + std::to_string(District::NUM_OF_TREES) + "\n";
    res += get_neighbourhood_string(district);

    saveFile.open(fileName.c_str(), std::ios_base::out);
    saveFile << res;
    saveFile.close();

    return res;
}

std::string SaveState::loadstate(District *district) {

    int total = 0;

    bool firstRead = false;

    std::string res = "";

    int neighboorhoodPos = 4;
    int linkedNodePos = 5;

    int outgoingEdgePos = 7;
    int outgoingEdgeIdPos = 8;
    int numOfChildrenPos = 9;
    int capacityPos = 10;
    int capacityIdPos = 11;
    int childrenPos = 12;
    int rootPos = 13;
    int updatePos = -1;

    int neighbourhoodIndex;
    int linkedNodesAmount;
    int linkedNodeIndex;

    int numLinkedNodes;

    std::string line;
    int i = 0;
    loadFile.open(fileName.c_str());


    if(loadFile.is_open()) {
        while( getline(loadFile, line) ) {
            int outgoingEdge;
            int outgoingEdgeId;
            double capacity;
            int capacityId;
            int numberOfChild;
            int children[3000];
            bool edge;

            if(i == 1) {
                int numNodes = get_line_int(line);
                if(numNodes == not nodesInSystem) {
                    return "false";
                }

            } else if (i == 2) {
                int numNeighbourhoods = get_line_int(line);
                if(numNeighbourhoods =! amountOfNeighbourhoods) {
                    amountOfNeighbourhoods = numNeighbourhoods;
                    updatedAmountNeighbourhoods = true;
                }
            } else if(i == neighboorhoodPos) {
                neighbourhoodIndex = get_line_int(line);
            } else if(i == linkedNodePos) {
                linkedNodesAmount = get_line_int(line);
                total += linkedNodesAmount;
                linkedNodeIndex = 0;
                // Calculating updatePos ad linkedNodePos
                // Order of these caluclations is important
                if(firstRead){
                    outgoingEdgePos = outgoingEdgePos + 3;
                    outgoingEdgeIdPos = outgoingEdgeIdPos + 3;
                    capacityPos = capacityPos + 3;
                    capacityIdPos = capacityIdPos + 3;
                    numOfChildrenPos = numOfChildrenPos + 3;
                    childrenPos = childrenPos + 3;
                    rootPos = rootPos + 3;
                } else {
                    firstRead = true;
                }

                if(linkedNodesAmount == 1) {
                    linkedNodePos = linkedNodePos + 11;
                    neighboorhoodPos = neighboorhoodPos + 11;
                } else {
                    linkedNodePos = linkedNodePos + linkedNodesAmount*8 + 3;
                    neighboorhoodPos = neighboorhoodPos + linkedNodesAmount*8 + 3;
                }


            } else if(i == outgoingEdgePos) {
                outgoingEdge = get_line_int(line);
                outgoingEdgePos += 8;
            } else if(i == outgoingEdgeIdPos) {
                outgoingEdgeId = get_line_int(line);
                outgoingEdgeIdPos += 8;
            }  else if(i == numOfChildrenPos) {
                numberOfChild = get_line_int(line);
                numOfChildrenPos += 8;
            } else if(i == capacityPos) {
                capacity = get_line_double(line);
                capacityPos += 8;
          }  else if(i == capacityIdPos) {
                capacityId = get_line_int(line);
                capacityIdPos += 8;
                // TODO
                update_link_capacity(district->get_all_link_capacities(), capacity, capacityId);
            } else if(i == childrenPos) {

                if(numberOfChild > 0) {
                    int index = 0;
                    int pos = line.find(":") + 1;
                    int pos1 = line.find_first_of(';');
                    std::string num = line.substr(pos, pos1-pos);
                    std::string rest = line.substr(pos1);
                    children[index] = std::stoi(num);
                    index++;

                    for(int i = 1; i < numberOfChild; i++) {
                        int pos2 = rest.find_first_of(";") + 1;
                        std::string rest1 = rest.substr(pos2);
                        int pos3 = rest1.find_first_of(";");
                        num = rest.substr(pos2, pos3-pos2 + 1);
                        rest = rest.substr(pos3 + 1);
                        children[index] = std::stoi(num);
                        index++;
                    }
                }

                childrenPos += 8;

            } else if(i == rootPos) {

                int pos = line.find("EDGEROOT");

                if(pos) {
                    edge = false;
                } else {
                    edge = true;
                }

                rootPos += 8;

                district->get_neighbourhood(neighbourhoodIndex)->update_linked_nodes(linkedNodeIndex,
                                                                                outgoingEdge,
                                                                                outgoingEdgeId,
                                                                                numberOfChild,
                                                                                children,
                                                                                edge);

//                for(int i = 0 ; i < numberOfChild; i++) {
//                    res += "i: " + std::to_string(i) +  " Child: " + std::to_string(children[i]) + "\n";
//                }

                linkedNodeIndex++;
                district->get_neighbourhood(neighbourhoodIndex)->set_linked_node_updates(true);
                district->get_neighbourhood(neighbourhoodIndex)->set_num_linked_nodes(linkedNodesAmount);
            }

            i++;
        }
    }

    loadFile.close();

    district->update_linked_nodes_from_file();

    return res;
}

std::string SaveState::get_time(void) {
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string t(std::ctime(&end_time));
    return t;
}

std::string SaveState::get_neighbourhood_string(District *district) {

    std::string res = "";

    for(int i = 0; i < District::NUM_OF_TREES; i++) {
        std::string neigh("NEIGHBOURHOODNUM:");
        res += "\n" + neigh + std::to_string(i) + "\n";
        res += (district->get_neighbourhood(i))->to_string();
    }

    return res;
}

int SaveState::get_line_int(std::string line) {
    int pos = line.find(":") + 1;
    std::string numS = line.substr(pos);
    int num = std::stoi(numS);
    return num;
}

double SaveState::get_line_double(std::string line) {
    int pos = line.find(":") + 1;
    std::string numS = line.substr(pos);
    double num = std::stod(numS);
    return num;
}

int SaveState::update_line_pos(int reference, int pointer) {

    int amount = 0;

    if(reference == 1) {
        amount = pointer + 8;
    } else {
        amount = pointer + 8;
    }

    return amount;

}

void SaveState::update_link_capacity(LinkCapacity *linkcapacities, double cap, int id) {

    if(linkCapacityIndex == 0) {
        linkcapacities[linkCapacityIndex].set_current_capacity(cap);
        linkcapacities[linkCapacityIndex].set_connected_node_id(id);
        linkCapacityIndex++;
        return;
    } else {
        // check if the capacity linked to the id has already been updated, if so return otherwise add it to the list
        for(int i = 0; i  < linkCapacityIndex; i++) {
            if(linkcapacities[i].get_connected_node_id() == id) {
                return;
            }
        }
        linkcapacities[linkCapacityIndex].set_current_capacity(cap);
        linkcapacities[linkCapacityIndex].set_connected_node_id(id);
        linkCapacityIndex++;
    }

}
