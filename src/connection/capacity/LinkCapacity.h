#ifndef LINKCAPACITY_H_
#define LINKCAPACITY_H_

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "../../paymentchannel/concurrency/Concurrency.h"

struct pending_payment {
    int transactionId;
    double amount;
    bool increaseLink;
};

class LinkCapacity {

    public:
        LinkCapacity();
        LinkCapacity(int);

        void set_outgate(int);
        int get_outgate(void);

        void set_connected_node_id(int);
        int get_connected_node_id(void);

        double get_current_capacity(void);
        void set_current_capacity(double);

        double get_current_virtual_capacity(void);

        bool add_pending_transaction_increase(double, int);
        bool add_pending_transaction_decrease(double, int);

        bool complete_transaction(int);

        void remove_pending_transaction_index(int);
        void remove_pending_transaction_transaction_id(int);

        bool update_increase(double);
        bool update_decrease(double);

        std::string to_file(void);

    private:
        int outgate;
        int connectedNodeId;
        double capacity;
        double virtualCapacity;

        int numOfPayments;
        int numOfPaymentsPending;
        pending_payment pendingPayments[2000];
        Concurrency concurrency[50];

};

#endif
