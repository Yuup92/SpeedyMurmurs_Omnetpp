#ifndef MESSAGEBUFFER_H__
#define MESSAGEBUFFER_H__

#include <list>
#include "./BufferedMessage.h"
#include "../basicmsg_m.h"



class MessageBuffer
{
    public:
        MessageBuffer();
        ~MessageBuffer();

        void addMessage(BufferedMessage*);
//        void addMessage(BasicMessage, int);

        int getMessageCount(void);
        BufferedMessage * getMessage(void);


    private:
        // Reason for using a list not a vector:
        // https://baptiste-wicht.com/posts/2012/11/cpp-benchmark-vector-vs-list.html
        int messages_in_buffer;
        std::list<BufferedMessage*> messages;
};

#endif
