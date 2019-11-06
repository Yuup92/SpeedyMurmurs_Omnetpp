#include "./TransactionEvent.h"

TransactionEvent::TransactionEvent(void) {

}

void TransactionEvent::set_transaction(double time_, double amount_, int dest_) {
    time = time_;
    amount = amount_;
    destination = dest_;
}

double TransactionEvent::get_time(void) {
    return time;
}

double TransactionEvent::get_amount(void) {
    return amount;
}

int TransactionEvent::get_destination(void) {
    return destination;
}

std::string TransactionEvent::get_string(void) {
    std::string res = "";
    res += "time: " +std::to_string(time);
    res += ", destination: " + std::to_string(destination);
    res += ", amount: " + std::to_string(amount) + "\n";
    return res;
}
