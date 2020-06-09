#ifndef _NODE_H
#define _NODE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <thread>


/* Abstract class node */
class Node
{
    public:
        Node();
        ~Node();
        int getSockedFD();
        void setSockedFD(int socket_fd);
        virtual void sendMessage(std::string &s) = 0; // pure virtual function used to send a single message
        virtual std::string &receiveMessage() = 0; //  pure virtual function used to receive a single message


    protected:
        sockaddr_in _address;
        int _socketFD;
        char _sendBuffer[1024];
        std::string _sendString;
};


#endif