#include "./MessageBuffer.h"

MessageBuffer::MessageBuffer()
{
    messages_in_buffer = 0;
//    messages = new std::list<BufferedMessage>;
}

MessageBuffer::~MessageBuffer()
{

}

void MessageBuffer::addMessage(BufferedMessage * buf_msg)
{
    messages.push_back(buf_msg);
    messages_in_buffer++;
}

//void MessageBuffer::addMessage(BasicMessage msg, int dst)
//{
//    BufferedMessage * buf_msg = new BufferedMessage(msg, dst);
//    messages.push_back(buf_msg);
//    messages_in_buffer++;
//}

int MessageBuffer::getMessageCount(void)
{
    return messages_in_buffer;
}

BufferedMessage * MessageBuffer::getMessage(void)
{
    BufferedMessage * res = messages.front();
    messages.pop_front();
    messages_in_buffer--;
    return res;
}
