#include <string>
#include <iostream>
#include "MessageQueue.h"



std::string MessageQueue::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this](){ return !(this->_queue.empty()); });

    std::string message = std::move(_queue.front());
    _queue.pop_front();

    return message;
}


void MessageQueue::send(std::string &&msg)
{
    std::lock_guard<std::mutex> gLock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
    std::cout << "1" << std::endl;
}