#ifndef TRANSACTIONEVENT_H_
#define TRANSACTIONEVENT_H_

#include <stdlib.h>
#include <string>
#include <stdio.h>

using namespace std;

class TransactionEvent {

    public:
        TransactionEvent(void);
        void set_transaction(double, double, int);

        double get_time(void);
        double get_amount(void);
        int get_destination(void);

        std::string get_string(void);

    private:
        double time;
        double amount;
        int destination;
};

#endif
