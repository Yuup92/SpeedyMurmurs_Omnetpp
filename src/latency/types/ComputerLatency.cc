#include "./ComputerLatency.h"

ComputerLatency::ComputerLatency() {
    delay = 0;
}

double ComputerLatency::get_delay(void) {
    double tmp = delay;
    delay = 0;
    return tmp;
}

double ComputerLatency::calculate_delay_ms(void) {

    std::normal_distribution<double> distribution(0.0005, 0.005);
    delay = 0;//distribution(generator) + 0.015;
    return delay;

}
