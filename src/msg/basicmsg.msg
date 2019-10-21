//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

message BasicMessage
{
    int source;
    int destination;
    int scalar_clock;
    int srcNodeId;
    
    int pulseNum;
    
    int type;
    int subType;
    
    // Spanning Tree Params:
    int nodeId;
    int spanningTreeIndex;
    int spanningTreeId;
    int spanningTreeIndexList;
    int nextTreeIndex;
    int weightEdgeSpanningTree;
    
    int levelNode;
    int fragmentName;
    int nodeState;
    int bestWeight;
    
    int indexChildrenIds;
    int listChildrenIds[1000];
    int lengthVectorCordinates;
    int vectorCordinates[1000];
    string childrenIds;
    int depth;

    
    // Transaction
    int endNodeId;
    double amount;
    int transactionId;
    
    int pathTransactionId;
    int totalNumberOfPaths;
    
    int neighbourhoodIndex;
    
    // Otherthings...
    
    int inspection;
    
    int lowestIdFragment;
    int rootNodeId;
    
    int leaderId;
    
    bool ack;
        
    int root_node;
    bool start_spanning_tree = false;
    
    bool spanning_request = false;
    
    bool spanning_request_ack = false;
    int spanning_tree_level;
    
    bool spanning_decline_request = false;
    
    bool down_broadcast = false;    
    bool up_broadcast_reply = false;
    
}