#include "../../msg/outgoing_buf/MessageBuffer.h"
#include "../../msg/outgoing_buf/BufferedMessage.h"

class LeaderElection
{
    public:
        LeaderElection();
        static const int LEADER_MSG = 1;


        void setMsgBuf(MessageBuffer);
        void setId(int);
        void setAmountNeighbours(int);

        bool isLeader(void);

        void broadcastLeaderRequest(void);
        void broadcastElectedLeader(void);
        void handleMessage(BasicMessage*, int);

        int getMessageCount(void);
        BufferedMessage * getMessage(void);
        MessageBuffer getMessageBuffer(void);

    private:
        bool leader;

        MessageBuffer msg_buf;
        int nodeId;
        int amountNeighbours;
        int sentMsgs;

        static BasicMessage * generateLeaderMessage(int);
        static BasicMessage * generateLeaderAckMessage(int);
        static BasicMessage * generateElectedLeaderMessage(int);

};
