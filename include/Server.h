#ifndef _SERVER_H
#define _SERVER_H

#include <mutex>
#include <thread>
#include <vector>
/* header files for unix socket dev */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Node.h"

class Server : public Node
{
    public:
        Server(std::string ip_address, uint16_t port);
        ~Server() { close(_socketFD); };
        virtual void sendMessage(std::string &s) override; // overriden virtual function used to send a single message
        virtual std::string &receiveMessage() override; //  overriden function used to receive a single message
        void acceptClientConnections();

    private:
        int _otherSocketFD;
};

#endif