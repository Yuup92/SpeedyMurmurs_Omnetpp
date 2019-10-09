#include "./Transactions.h"

Transactions::Transactions() {

    currentNumberOfTransactions = 0;
}

std::string Transactions::add_send_transaction(District *d, MessageBuffer *msgBuf, double amount, int endNode, int amountOfPaths) {

    std::string res = "";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        res = transactions[currentNumberOfTransactions].start_transaction(d, msgBuf, amount, endNode, amountOfPaths);
        int transIndex = currentNumberOfTransactions;
        currentNumberOfTransactions++;
        return res;
    } else {
        //TODO do something
        return res;
    }
}

std::string Transactions::add_forward_transaction(MessageBuffer *msg, int transId, TransactionPath *transPath){

    std::string res = "";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {

        for(int i = 0; i < currentNumberOfTransactions; i++) {
            if(transactions[i].get_trans_id() == transId) {
                transactions[i].forward_transaction(msg, transId, transPath);
                return res;
            }
        }

        res = transactions[currentNumberOfTransactions].forward_transaction(msg, transId, transPath);
        currentNumberOfTransactions++;
        return res;
    } else {
        // TODO add an error message or something
    }

}

std::string Transactions::add_receiving_transaction(MessageBuffer *msg, int transId, TransactionPath *transPath) {
    std::string res = "";

    // Search current transId so the multi-path transaction can be grouped together

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        int i;
        if(checkTransId(transId, i)) {
            transactions[i].receiving_transaction(msg, transId, transPath);
        } else {
            res = transactions[currentNumberOfTransactions].receiving_transaction(msg, transId, transPath);
            currentNumberOfTransactions++;
        }
    }

    return res;

}

std::string Transactions::incoming_path_query_accept(TransactionPath *path) {

}

Transaction * Transactions::get_transaction(int transId) {
    for(int i = 0; i < currentNumberOfTransactions; i++) {
        if(transactions[i].get_trans_id() == transId) {
            return &transactions[i];
        }
    }
}

TransactionPath * Transactions::get_path(int transId, int pathId) {

    for(int i = 0; i < currentNumberOfTransactions; i++) {
        if(transactions[i].get_trans_id() == transId) {
            return transactions[i].get_trans_path(pathId);
        }
    }

}

bool Transactions::checkTransId(int transId, int &index) {
    for(int i = 0; i < currentNumberOfTransactions; i++) {
        if(transactions[i].get_trans_id() == transId) {
            index = i;
            return true;
        }
    }
    return false;
}
