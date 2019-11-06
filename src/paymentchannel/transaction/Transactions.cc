#include "./Transactions.h"

Transactions::Transactions() {

    currentNumberOfTransactions = 0;
}

void Transactions::set_stats(Statistics *s) {
    stats =s;
}

int Transactions::get_current_trans(void) {
    return currentNumberOfTransactions;
}

int Transactions::get_trans_map_size(void) {
    return transactionMap.size();
}

bool Transactions::check_for_existing_transaction(int transId) {
    unordered_map<int,Transaction*>::const_iterator got = transactionMap.find(transId);
    if(got == transactionMap.end()) {
        return false;
    } else {
        return true;
    }
}

Transaction* Transactions::get_transaction_from_map(int transId) {
    unordered_map<int,Transaction*>::const_iterator got = transactionMap.find(transId);
    return got->second;
}

std::string Transactions::add_send_transaction(District *dist, MessageBuffer *msgBuf, double amount, int endNode, int numberOfPaths) {

    std::string res = "";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        Transaction *trans = new Transaction();
        trans->set_amount(amount);
        trans->set_stats(stats);
        trans->start_transaction(dist, msgBuf, endNode, numberOfPaths, 0);
        transactionMap.emplace(trans->get_trans_id(), trans);
        currentNumberOfTransactions++;
        return res;
    } else {
        //TODO
        // send cancel msg
        return res;
    }
}

std::string Transactions::add_path_to_transaction_id(MessageBuffer *msgBuf, int transId, TransactionPath *transPath) {
    std::string res = "";
    Transaction *trans = get_transaction_from_map(transId);
    res += trans->add_new_forward_path(msgBuf, transPath);
    return res;
}

std::string Transactions::new_forwarding_transaction(MessageBuffer *msgBuf, int transId, TransactionPath *transPath) {
    std::string res ="";

    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        Transaction *trans = new Transaction();
        trans->new_foward_transaction(msgBuf, transId, transPath);
        // Add new transaction to map
        transactionMap.emplace(trans->get_trans_id(), trans);
        currentNumberOfTransactions++;
        return res;
    } else {
        //TODO
        // Too many transactions
        return res;
    }


}

std::string Transactions::new_receiving_transaction(MessageBuffer *msgBuf, int transId, TransactionPath *transPath) {
    std::string res = "";

    // Search current transId so the multi-path transaction can be grouped together
    if(currentNumberOfTransactions < Transactions::MAX_TRANSACTIONS) {
        if(check_for_existing_transaction(transId)) {
            Transaction *trans = get_transaction_from_map(transId);
            trans->add_new_receiving_path(msgBuf, transId, transPath);
        } else {
            Transaction *trans = new Transaction();
            trans->new_receiving_transaction(msgBuf, transId, transPath);
            transactionMap.emplace(trans->get_trans_id(), trans);
            currentNumberOfTransactions++;
        }
    }

    return res;

}

bool Transactions::check_for_path_id(int pathId) {
//    for(int i = 0; i < currentNumberOfTransactions; i++) {
//        if(transactions[i].check_path_id(pathId) == true){
//            return true;
//        }
//    }
//    return false;
}

bool Transactions::check_for_trans_id(int transId) {
    int i;
    return check_transId_exists(transId, i);
}

Transaction * Transactions::get_transaction_from_path(int pathId) {
//    for(int i = 0; i < currentNumberOfTransactions; i++) {
//        if(transactions[i].check_path_id(pathId) == true) {
//            return &transactions[i];
//        }
//    }
}

Transaction * Transactions::get_transaction(int transId) {
//    Transaction * p = NULL;
//    for(int i = 0; i < currentNumberOfTransactions; i++) {
//        if(transactions[i].get_trans_id() == transId) {
//            return &transactions[i];
//        }
//    }
//    return p;
}

TransactionPath * Transactions::get_path(int transId, int pathId) {
//    TransactionPath * p = NULL;
//    for(int i = 0; i < currentNumberOfTransactions; i++) {
//        if(transactions[i].get_trans_id() == transId) {
//            return transactions[i].get_trans_path(pathId);
//        }
//    }
//    return p;
}

bool Transactions::remove_dead_transactions(void) {
    bool transactionRemoved = false;
    int k = 0;
    int keys[50];
    if(transactionMap.size() > 0) {
        for(pair<int, Transaction*> element : transactionMap) {
            element.second->check_alive_time();
            if(element.second->get_state() == Transaction::TRANSACTION_DEAD) {
                if(element.second->increment_death_counter() <= 0) {
                    keys[k] = element.first;
                    k++;
                }

            }
        }
    }

    if(k > 0){
        for(int i = 0; i < k; i++) {
            transactionMap.erase(keys[i]);
            transactionRemoved = true;
        }
    }

    currentNumberOfTransactions = currentNumberOfTransactions - k;

    return transactionRemoved;
}

bool Transactions::check_transId_exists(int transId, int &index) {
//    if(currentNumberOfTransactions > 0){
//        for(int i = 0; i < currentNumberOfTransactions; i++) {
//            if(transactions[i].get_trans_id() == transId) {
//                index = i;
//                return true;
//            }
//        }
//    }
//    return false;
}

bool Transactions::remove_transaction(int transId) {
//    bool transactionFound = false;
//    if(currentNumberOfTransactions > 0) {
//        for(int i = 0; i < currentNumberOfTransactions; i++) {
//            if(transactions[i].get_trans_id() == transId) {
//                transactionFound = true;
//                transactions[i].reset();
//                for(int j = i; j < currentNumberOfTransactions-1; j++) {
//                    if(transactions[j+1].get_trans_id() == 0) {
//                        break;
//                    }
//                    std::swap(transactions[j], transactions[j+1]);
//                }
//            }
//        }
//    }
//    if(transactionFound){
//        return true;
//    }
//    return false;
}
