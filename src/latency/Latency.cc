#include "./Latency.h"

Latency::Latency() {
    delay = 0;
}

double Latency::calculate_delay_ms(bool cryptography) {

    double totalDelay = 0.0;

    totalDelay += networkLatency.calculate_delay_ms();
    totalDelay += computerLatency.calculate_delay_ms();

    //if(cryptography) {
        //totalDelay += crytographicLatency.calculate_delay_ms();
    //}


    return totalDelay;

}

double Latency::get_network_delay(void) {
    return networkLatency.get_delay();
}

double Latency::get_os_delay(void) {
    return computerLatency.get_delay();
}

double Latency::get_crypto_delay(void) {
    return cryptographicLatency.get_delay();
}
