#include "./CryptographicLatency.h"

CryptographicLatency::CryptographicLatency() {
    delay = 0;
}

double CryptographicLatency::calculate_delay_ms(void) {
    delay = 0.015;
    return delay;
}

double CryptographicLatency::get_delay(void) {
    double tmp = delay;
    delay = 0;
    return tmp;
}
