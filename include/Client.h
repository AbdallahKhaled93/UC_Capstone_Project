#ifndef _CLIENT_H
#define _CLIENT_H

#include "Node.h"

class Server;

class Client : public Node
{
    friend class Server;
    public:
        Client();
        Client(std::string ip_address, uint16_t port);
        Client(Client &client) = delete; // prevent making a copy of the client due to the unique ptr
        Client(Client &&client); // move constructor
        Client &operator=(Client &client) = delete; // prevent making a copy of the client due to the unique ptr
        Client &operator=(Client &&client);
        ~Client();
        void connectToServer(std::string ip_address, uint16_t port);
        void sendMessage(std::string s) override;
        std::string receiveMessage()  override;
        void terminateConnection() override;

    private:
        std::unique_ptr<sockaddr_in>& getAddressStruct();
};


#endif