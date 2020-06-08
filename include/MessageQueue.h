#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H

#include <string>
#include <mutex>
#include <deque>
#include <condition_variable>

/* Message Queue class */
class MessageQueue
{
public:
    std::string receive();
    void send(std::string &&msg);
private:
    std::deque<std::string> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif