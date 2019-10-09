#include "./QueuedMessage.h"

QueuedMessage::QueuedMessage(){}

QueuedMessage::~QueuedMessage(){
    delete(msg);
}

void QueuedMessage::set_initial_queue(BasicMessage *_msg, int _edge) {
    msg = _msg;
    edge = _edge;
}

void QueuedMessage::set_connect_message(int _type, int _edge, int lvl) {
    type = _type;
    edge = _edge;
    level = lvl;
}

void QueuedMessage::set_initiate_message(int _type, int _edge, int lvl, int f, int state) {

}

void QueuedMessage::set_report_message(int _type, int _edge, int w) {
    type = _type;
    edge = _edge;
    weight = w;
}

void QueuedMessage::set_test_message(int _type, int _edge, int l, int f) {
    type = _type;
    edge = _edge;
    level = l;
    fragmentName = f;
}

void QueuedMessage::set_weight(int w){
    weight = w;
}

void QueuedMessage::set_level(int l) {
    level = l;
}

void QueuedMessage::set_fragment_name(int f) {
    fragmentName = f;
}

int QueuedMessage::get_type(void) {
    return type;
}

int QueuedMessage::get_edge(void) {
    return edge;
}

int QueuedMessage::get_level(void) {
    return level;
}

int QueuedMessage::get_weight(void) {
    return weight;
}

int QueuedMessage::get_fragment_name(void) {
    return fragmentName;
}

BasicMessage * QueuedMessage::get_message(void) {
    return msg;
}
