#ifndef LATENCY_H_
#define LATENCY_H_

#include "./types/NetworkLatency.h"
#include "./types/ComputerLatency.h"
#include "./types/CryptographicLatency.h"


class Latency {

    public:
        Latency();

        double calculate_delay_ms(bool);

        double get_network_delay(void);
        double get_os_delay(void);
        double get_crypto_delay(void);

    private:
        int delay;

        NetworkLatency networkLatency;
        ComputerLatency computerLatency;
        CryptographicLatency cryptographicLatency;


};

#endif
