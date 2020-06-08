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
        Server(std::string ip_address, uint16_t port, int max_client_count);
        void acceptClientConnections();
        void sendMessage(std::string s) override;
        std::string receiveMessage() override;
        void terminateConnection() override;

    private:
        int _clientsCount;
        int _maxClientsCount;
        std::vector<Client> _clients;
        std::vector<std::thread> _clientThreads;
        std::mutex _mtxClientsVector;


        void handleClient();
};




#endif