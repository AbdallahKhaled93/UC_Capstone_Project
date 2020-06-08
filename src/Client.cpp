#include <arpa/inet.h>
#include <stdexcept>

#include "Client.h"


Client::Client()
{
    _address = std::make_unique<sockaddr_in>();
}

Client::Client(std::string ip_address, uint16_t port)
{
    _address = std::make_unique<sockaddr_in>();
}

Client::Client(Client &&client)
{
    _address = std::move(client._address);
    _socketFD = client._socketFD;

    client._socketFD = 0;
}


Client& Client::operator=(Client &&source)
{
    if (this == &source)
        return *this;

    _address = std::move(source._address);
    _socketFD = source._socketFD;

    source._socketFD = 0;
}

void Client::connectToServer(std::string ip_address, uint16_t port)
{
    sockaddr_in serverSocketAddress;
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_address.c_str(), &(serverSocketAddress.sin_addr)) <= 0)
    {
        throw std::runtime_error("Invalid server address");
    }

    if (connect(_socketFD, (struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
    {
        throw std::runtime_error("error during connection");
    }
}

void Client::sendMessage(std::string s)
{

}

std::string Client::receiveMessage()
{
    return "";
}

void Client::terminateConnection()
{

}


std::unique_ptr<sockaddr_in>& Client::getAddressStruct()
{
    return _address;
}

Client::~Client()
{

}