#ifndef CONCURRENCYBLOCKING_H_
#define CONCURRENCYBLOCKING_H_

// http://www.cplusplus.com/doc/tutorial/polymorphism/

class ConcurrencyBlocking {

    public:
        ConcurrencyBlocking();
        void update_concurrency(bool, int, int, int, int);
        bool is_blocked(void);
        void release(void);

        int get_transaction_id(void);
        int get_node_id(void);
        int get_outgoing_edge(void);
        int get_amount(void);

        void release_concurrency(void);

    private:
        bool currentlyBlocking;
        int idTransactionBlocking;
        int nodeIdBlocking;
        int currentEdgeDirection;
        int amount;


};

#endif
