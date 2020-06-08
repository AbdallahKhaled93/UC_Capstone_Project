#ifndef _NODE_H
#define _NODE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <memory>

/* Abstract class node */

class Node
{
    public:
        Node();
        ~Node();
        int getSockedFD();
        void setSockedFD(int socket_fd);
        virtual void sendMessage(std::string s) = 0;
        virtual std::string receiveMessage() = 0;
        virtual void terminateConnection() = 0;


    protected:
        std::unique_ptr<sockaddr_in> _address;
        int _socketFD;

};


#endif