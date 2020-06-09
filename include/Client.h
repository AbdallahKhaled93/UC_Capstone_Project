#ifndef _CLIENT_H
#define _CLIENT_H

#include "Node.h"

class Server;

class Client : public Node
{
    friend class Server;
    public:
        Client();
        ~Client() { close(_socketFD); };
        virtual void sendMessage(std::string &s) override; // overriden virtual function used to send a single message
        virtual std::string &receiveMessage() override; //  overriden function used to receive a single message
        void connectToServer(std::string ip_address, uint16_t port);

};


#endif