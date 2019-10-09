#ifndef TRANSACTIONMSG_H_
#define TRANSACTIONMSG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "./TransactionPath.h"
#include "../../msg/outgoing_buf/BufferedMessage.h"
#include "../../msg/outgoing_buf/MessageBuffer.h"

class TransactionMsg {

    public:
        // Message Types
        static const int MESSAGE_TYPE = 3;

        static const int CONCURRENCY_BLOCKING = 1;
        static const int CONCURRENCY_NON_BLOCKING = 2;

        static const int MULTIPATH_QUERY = 50;

        static const int INI_PATH_QUERY = 100;
        static const int INI_PATH_QUERY_ACCEPT = 101;
        static const int TRANSACTION_REJECT = 110;

        static const int PEND_CAPACITY_REQUEST = 200;
        static const int ACCEPT_CAPACITY_REQUEST = 201;
        static const int REJECT_CAPACITY_REQUEST = 210;

        static const int CLOSE_TRANSACTION = 300;
        static const int ACCEPT_TRANSACTION_CLOSE = 305;

        static const int CAPACITY_ERROR = 600;
        static const int TIMEOUT = 605;
        static const int ERROR = 610;

        TransactionMsg();
        static BasicMessage* path_initiation_request(TransactionPath*);
        static BasicMessage* path_request_accept(TransactionPath*);

        static BasicMessage* pend_transaction_request(TransactionPath*);
        static BasicMessage* accept_pending_request(TransactionPath*);
        static BasicMessage* reject_pending_request(TransactionPath*);

        static BasicMessage* finish_transaction(TransactionPath*);
        static BasicMessage* close_transaction(TransactionPath*);
};

#endif
