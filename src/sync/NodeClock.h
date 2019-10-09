class NodeClock
{
    private:
        int scalar_time;

    public:
        NodeClock();
        int get_scalar_time();
        void increment_time();
};
