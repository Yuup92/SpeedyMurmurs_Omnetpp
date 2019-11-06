#ifndef TRANSACTIONPATH_H_
#define  TRANSACTIONPATH_H_

#include <stdlib.h>
#include "../../connection/LinkedNode.h"


class TransactionPath {

public:

    static const int SENDER = 100;
    static const int FORWARDER = 125;
    static const int RECEIVER = 150;

    // SENDER
    static const int QUERY_STATE = 100;
    static const int PEND_TRANSACTION = 110;
    static const int PUSH_TRANSACTION = 120;

    // FORWARDER
    static const int JOINING_PATH = 200;
    static const int JOINED_PATH = 210;
    static const int CLOSE_PATH = 220;
    static const int PEND_PATH = 230;
    static const int PENDING_ACCEPTED = 250;
    static const int PENDING_FAILED = 240;
    static const int FINISH_TRANSACTION = 270;
    static const int CLOSE_TRANSACTION = 280;


    TransactionPath();
    void reset(void);
    void sending_path(double, LinkedNode*, int, int, int);

    void forwarding_path(double, LinkedNode*, LinkedNode*, int, int, int, int);
    void set_forwarding_path(TransactionPath*);

    void receiving_path(double, LinkedNode*, int, int, int);
    void set_receiving_path(TransactionPath*);

    void set_amount(double);
    double get_amount(void);

    int get_end_node_id(void);
    void set_end_node_id(int);

    double get_path_trans_amount(void);
    void set_path_trans_amount(double);

    int get_transaction_id(void);
    void set_transaction_id(int);

    int get_path_trans_id(void);
    void set_path_trans_id(int);

    int get_neighbourhood(void);
    void set_neighbourhood(int);

    int get_path_outgate_towards_receiver(void);
    void set_path_outgate_towards_receiver(int);

    int get_path_outgate_towards_sender(void);
    void set_path_outgate_towards_sender(int);

    int get_execution_role(void);
    void set_execution_role(int);

    int get_state(void);
    void set_state(int);

    int get_num_of_total_paths(void);
    void set_num_of_total_paths(int);

    bool get_transaction_pended(void);
    void set_transaction_pended(bool);

    bool is_completed(void);
    void set_done(void);
    void set_completed(bool);

    void close_path(void);
    bool is_path_closed(void);

    void set_sending_link(LinkedNode *);
    LinkedNode * get_link_towards_sender(void);

    void set_receiving_link(LinkedNode *);
    LinkedNode * get_link_towards_receiver(void);

private:

    int executionRole;
    int state;

    double amount;
    double fees;
    int endNode;
    int neighbourhood;

    int transactionId;
    int pathId;

    int numOfPathsForTransaction;

    bool pendTransaction;
    bool done;
    bool pathClosed;

    LinkedNode *linkTowardsSender;
    LinkedNode *linkTowardsReceiver;

};

#endif
