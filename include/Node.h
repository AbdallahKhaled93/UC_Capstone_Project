#ifndef _NODE_H
#define _NODE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <thread>

#include "MessageQueue.h"

/* Abstract class node */
class Node
{
    public:
        Node();
        ~Node();
        int getSockedFD();
        void setSockedFD(int socket_fd);
        virtual void sendMessage(std::string &s) = 0; // pure virtual function used to send a single message
        virtual char*receiveMessage() = 0; //  pure virtual function used to receive a single message
        std::shared_ptr<MessageQueue> getMessageQueue(); // returns shared pointer to the message queue
        virtual void terminateConnection() = 0;


    protected:
        sockaddr_in _address;
        int _socketFD;
        int _otherSocketFD;
        char _sendBuffer[1024];
        std::string _sendString;
        std::shared_ptr<MessageQueue> _receivedMsgs = std::make_shared<MessageQueue>();
        std::thread receptionThread;
        int temp = 0;
};


#endif