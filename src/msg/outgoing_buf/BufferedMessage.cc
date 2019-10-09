#include "./BufferedMessage.h"

BufferedMessage::BufferedMessage(void){};

BufferedMessage::BufferedMessage(cMessage * msg, int gate)
{
    message = msg;
    outGateInt = gate;
    delay = 0;
}

BufferedMessage::BufferedMessage(cMessage * msg, int gate, double d) {
    message = msg;
    outGateInt = gate;
    delay = d;
}

int BufferedMessage::get_out_gate_int(void)
{
    return outGateInt;
}

double BufferedMessage::get_delay(void)
{
    return delay;
}

cMessage * BufferedMessage::get_message(void)
{
    return message;
}

