#include "./TransactionMsg.h"

TransactionMsg::TransactionMsg(){

}

// Use to be called initial_send_request
BasicMessage * TransactionMsg::path_initiation_request(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Path initiation request");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::INI_PATH_QUERY);

    msg->setEndNodeId(transaction->get_end_node_id());
    msg->setAmount(transaction->get_path_trans_amount());
    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    msg->setNeighbourhoodIndex(transaction->get_neighbourhood());

    return msg;
}

BasicMessage * TransactionMsg::path_request_accept(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Accept initial path request");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::INI_PATH_QUERY_ACCEPT);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::pend_transaction_request(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Request Pending Capacity");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::PEND_CAPACITY_REQUEST);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());
    msg->setTotalNumberOfPaths(transaction->get_num_of_total_paths());

    return msg;
}

BasicMessage * TransactionMsg::accept_pending_request(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Accept Pending Capacity");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::ACCEPT_CAPACITY_REQUEST);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::reject_pending_request(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Reject Pending Capacity");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::REJECT_CAPACITY_REQUEST);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::finish_transaction(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Finalize PaymentChannel");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::CLOSE_TRANSACTION);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::close_transaction(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Finalize PaymentChannel");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::ACCEPT_TRANSACTION_CLOSE);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::capacity_error(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Capacity Error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::CAPACITY_ERROR);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::capacity_error(int transId, int pathId) {
    char msgname[40];
    sprintf(msgname, "Capacity Error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::CAPACITY_ERROR);

    msg->setTransactionId(transId);
    msg->setPathTransactionId(pathId);

    return msg;
}

BasicMessage * TransactionMsg::timeout_error(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Timeout Error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::TIMEOUT);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::error(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Error error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::ERROR);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}

BasicMessage * TransactionMsg::transaction_fail(TransactionPath *transaction) {
    char msgname[40];
    sprintf(msgname, "Transaction Fail");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(TransactionMsg::MESSAGE_TYPE);
    msg->setSubType(TransactionMsg::FAILURE);

    msg->setTransactionId(transaction->get_transaction_id());
    msg->setPathTransactionId(transaction->get_path_trans_id());

    return msg;
}



