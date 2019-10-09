#include "../../msg/basicmsg_m.h"

class QueuedMessage {

    public:
        QueuedMessage();
        ~QueuedMessage();
        void set_weight(int);
        void set_level(int);
        void set_fragment_name(int);

        void set_initial_queue(BasicMessage *, int);
        void set_connect_message(int _type, int _edge, int lvl);
        void set_initiate_message(int _type, int _edge, int lvl, int f, int state);
        void set_report_message(int _type, int _edge, int w);
        void set_test_message(int _type, int _edge, int l, int f);

        int get_type(void);
        int get_edge(void);
        int get_level(void);
        int get_weight(void);
        int get_fragment_name(void);
        int get_state(void);
        BasicMessage * get_message(void);

    private:
        BasicMessage *msg;

        int type;
        int edge;
        int level;
        int weight;
        int fragmentName;
        int state;

};
