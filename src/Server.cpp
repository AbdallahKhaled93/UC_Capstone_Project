#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>

#include "Server.h"
#include "Client.h"


Server::Server(std::string ip_address, uint16_t port) :  _otherSocketFD(0)
{
    int opt = 1;
    /* bind socket to a port */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_address.c_str(), &(_address.sin_addr)) <= 0)
    {
        throw std::runtime_error("Invalid server address");
    }

    // Forcefully attaching socket to the port
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    { 
        throw std::runtime_error("Set socket optiosn errors");
    }

    std::cout << "Binding server socket to port " << port << std::endl;
    if(bind(_socketFD, (struct sockaddr*)&_address, sizeof(_address)) < 0)
    {
        throw std::runtime_error("Error during address binding");
    }

    if(listen(_socketFD, 1) < 0)
    {
        throw std::runtime_error("Error during listening for connection");
    }

    std::cout << "Server is listening on " << ip_address << ":" << port << std::endl;
}

void Server::acceptClientConnections()
{
    int addrlen = sizeof(_address);

    _otherSocketFD = accept(this->_socketFD, (struct sockaddr*)&_address, (socklen_t*)&addrlen);

    if (_otherSocketFD == -1)
    {
        throw std::runtime_error("Could not accept client connection");
    }

    std::cout << "Connection with socket " << _otherSocketFD << " established" << std::endl;
}

void Server::sendMessage(std::string &s)
{
    /* check if there is a connection */
    if(_otherSocketFD > 0)
    {
        send(_otherSocketFD, s.c_str(), s.size(), 0);
    }
}

std::string &Server::receiveMessage()
{
    int byteCount;
    /* check if there is a connection */
    if(_otherSocketFD > 0)
    {
        byteCount = recv(_otherSocketFD, _sendBuffer, 1024, 0);
        if(byteCount)
        {
            _sendBuffer[byteCount] = '\0';
        }
        else
        {
            _sendBuffer[0] = '\0';
        }
    }
    else
    {
        _sendBuffer[0] = '\0';
    }


    _sendString = std::string(_sendBuffer);
    return _sendString;
}