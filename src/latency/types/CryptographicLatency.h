#ifndef CRYPTOGRAPHICLATENCY_H_
#define CRYPTOGRAPHICLATENCY_H_

class CryptographicLatency {

    public:
        CryptographicLatency();

        double calculate_delay_ms(void);
        double get_delay(void);

    private:
        double delay;

};

#endif
