#include "./NetworkLatency.h"

NetworkLatency::NetworkLatency(void){
    delay = 0;
}

double NetworkLatency::calculate_delay_ms(void) {

    std::lognormal_distribution<double> distribution(0.002, 0.7);
    delay = distribution(generator) / 10.0;
    return delay;

}

double NetworkLatency::get_delay(void) {
    double tmp = delay;
    delay = 0;
    return tmp;
}
