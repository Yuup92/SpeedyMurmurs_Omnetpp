#ifndef NETWORKLATENCY_H_
#define NETWORKLATENCY_H_

#include <stdlib.h>
#include <random>

class NetworkLatency {

    public:
        NetworkLatency();

        double calculate_delay_ms(void);
        double get_delay(void);

    private:
        double delay;
        std::default_random_engine generator;


};

#endif
