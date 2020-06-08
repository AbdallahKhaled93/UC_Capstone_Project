#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>

#include "Server.h"
#include "Client.h"


Server::Server(std::string ip_address, uint16_t port, int max_client_count)
{
    _maxClientsCount = max_client_count;
    _clientsCount = 0;

    /* bind socket to a port */
    _address = std::make_unique<sockaddr_in>();
    (_address.get())->sin_family = AF_INET;
    (_address.get())->sin_port = htons(port);
    if(inet_pton(AF_INET, ip_address.c_str(), &((_address.get())->sin_addr)) <= 0)
    {
        throw std::runtime_error("Invalid server address");
    }

    std::cout << "Binding server socket to port " << port << std::endl;
    if(bind(_socketFD, (struct sockaddr*)_address.get(), sizeof(*(_address.get()))) < 0)
    {
        throw std::runtime_error("Error during address binding");
    }

    if(listen(_socketFD, max_client_count) < 0)
    {
        throw std::runtime_error("Error during listening for connection");
    }

    std::cout << "Server is listening on " << ip_address << ":" << port << std::endl;
}

void Server::acceptClientConnections()
{
    /*while (_clientsCount < _maxClientsCount)
    {
        _clientsCount++;
        _clientThreads.emplace_back(std::thread(&Server::handleClient, this));
    }*/
    handleClient();
    
}


void Server::handleClient()
{
    Client client;
    socklen_t clientAddressLength = sizeof(*((client.getAddressStruct()).get()));
    client.setSockedFD(
        accept(this->_socketFD, (struct sockaddr*)(client.getAddressStruct().get()), &clientAddressLength)
        );
    if (client.getSockedFD() == -1)
    {
        throw std::runtime_error("Could not accept client connection");
    }
    
    std::unique_lock<std::mutex> uLock;

    _clients.push_back(std::move(client));
    std::cout << "Connection with client " << _clients.back().getSockedFD() << " established" << std::endl;


}


void Server::sendMessage(std::string s)
{

}

std::string Server::receiveMessage()
{
    return "";
}

void Server::terminateConnection()
{

}