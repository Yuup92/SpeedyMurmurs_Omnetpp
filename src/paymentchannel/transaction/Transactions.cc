#include "./Transactions.h"

Transactions::Transactions() {

    currentNumberOfTransactions = 0;
}

int Transactions::get_current_trans(void) {
    return currentNumberOfTransactions;
}

std::string Transactions::add_send_transaction(District *d, MessageBuffer *msgBuf, double amount, int endNode, int amountOfPaths) {

    std::string res = "";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        transactions[currentNumberOfTransactions].set_amount(amount);
        res = transactions[currentNumberOfTransactions].start_transaction(d, msgBuf, endNode, amountOfPaths, 0);
        int transIndex = currentNumberOfTransactions;
        currentNumberOfTransactions++;
        return res;
    } else {
        //TODO
        // send cancel msg
        return res;
    }
}

std::string Transactions::add_forward_transaction(MessageBuffer *msg, int transId, TransactionPath *transPath){

    std::string res = "";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {

        int transaction;
        if(check_transId_exists(transId, transaction)) {
            transactions[transaction].forward_transaction(msg, transId, transPath);
        } else {
            res = transactions[currentNumberOfTransactions].forward_transaction(msg, transId, transPath);
            currentNumberOfTransactions++;
        }
        return res;
    } else {
        // TODO add an error message or something
    }
    return res;
}

std::string Transactions::add_receiving_transaction(MessageBuffer *msg, int transId, TransactionPath *transPath) {
    std::string res = "";

    // Search current transId so the multi-path transaction can be grouped together
    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        int transaction;
        if(check_transId_exists(transId, transaction)) {
            transactions[transaction].receiving_transaction(msg, transId, transPath);
        } else {
            res = transactions[currentNumberOfTransactions].receiving_transaction(msg, transId, transPath);
            currentNumberOfTransactions++;
        }
    }

    return res;

}

bool Transactions::check_for_trans_id(int transId) {
    int i;
    return check_transId_exists(transId, i);
}

Transaction * Transactions::get_transaction(int transId) {
    Transaction * p = NULL;
    for(int i = 0; i < currentNumberOfTransactions; i++) {
        if(transactions[i].get_trans_id() == transId) {
            return &transactions[i];
        }
    }
    return p;
}

TransactionPath * Transactions::get_path(int transId, int pathId) {
    TransactionPath * p = NULL;
    for(int i = 0; i < currentNumberOfTransactions; i++) {
        if(transactions[i].get_trans_id() == transId) {
            return transactions[i].get_trans_path(pathId);
        }
    }
    return p;
}

bool Transactions::remove_dead_transactions(void) {
    bool transactionRemoved = false;
    int k = 0;
    if(currentNumberOfTransactions > 0) {
        for(int i = 0; i < currentNumberOfTransactions; i++) {
            if(transactions[i].get_state() == Transaction::TRANSACTION_DEAD) {
                transactionRemoved = remove_transaction(transactions[i].get_trans_id());
                if(transactionRemoved) {
                    k++;
                    transactionRemoved = false;
                }

            }
        }
    }
    if(k > 0){
        currentNumberOfTransactions = currentNumberOfTransactions - k;
    }

    return transactionRemoved;
}

bool Transactions::check_transId_exists(int transId, int &index) {
    if(currentNumberOfTransactions > 0){
        for(int i = 0; i < currentNumberOfTransactions; i++) {
            if(transactions[i].get_trans_id() == transId) {
                index = i;
                return true;
            }
        }
    }
    return false;
}

bool Transactions::remove_transaction(int transId) {
    bool transactionFound = false;
    if(currentNumberOfTransactions > 0) {
        for(int i = 0; i < currentNumberOfTransactions; i++) {
            if(transactions[i].get_trans_id() == transId) {
                transactionFound = true;
                transactions[i].reset();
                for(int j = i; j < currentNumberOfTransactions-1; j++) {
                    if(transactions[j+1].get_trans_id() == 0) {
                        break;
                    }
                    std::swap(transactions[j], transactions[j+1]);
                }
            }
        }
    }
    if(transactionFound){
        return true;
    }
    return false;
}
