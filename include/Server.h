#ifndef _SERVER_H
#define _SERVER_H

/* header files for unix socket dev */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <vector>

#include "Node.h"

class Client;

class Server : public Node
{
    public:
        Server(std::string ip_address, uint16_t port);
        virtual void sendMessage(std::string &s) override; // overriden virtual function used to send a single message
        virtual char*receiveMessage() override; //  overriden function used to receive a single message
        void acceptClientConnections();
        void terminateConnection() override;
        void Polling(); // function used to keep polling on received message and them in a queue
};




#endif