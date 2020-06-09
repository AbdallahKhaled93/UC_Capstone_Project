#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

#include "Client.h"



Client::Client()
{
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

    std::cout << "Connection with server established" << std::endl;
}


void Client::sendMessage(std::string &s)
{
    /* check if there is a connection */
    send(_socketFD, s.c_str(), s.size(), 0);

}

std::string &Client::receiveMessage()
{
    int byteCount = recv(_socketFD, _sendBuffer, 1024, 0);
    if(byteCount)
    {
        _sendBuffer[byteCount] = '\0';
    }
    else
    {
        _sendBuffer[0] = '\0';
    }


    _sendString = std::string(_sendBuffer);
    return _sendString;
}

