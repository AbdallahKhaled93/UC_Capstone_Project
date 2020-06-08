#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>

#include "Server.h"
#include "Client.h"


Server::Server(std::string ip_address, uint16_t port)
{

    /* bind socket to a port */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_address.c_str(), &(_address.sin_addr)) <= 0)
    {
        throw std::runtime_error("Invalid server address");
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
    receptionThread = std::thread(&Server::Polling, this);
}

void Server::sendMessage(std::string &s)
{
    /* check if there is a connection */
    if(_otherSocketFD > 0)
    {
        send(_socketFD, s.c_str(), s.size(), 0);
    }
}

char* Server::receiveMessage()
{
    int byteCount;
    /* check if there is a connection */
    if(_otherSocketFD > 0)
    {
        byteCount = read(_otherSocketFD, _sendBuffer, 1024);
        if(byteCount)
        {
            _sendBuffer[byteCount] = '\0';
            return _sendBuffer;
        }
        else
        {
            _sendBuffer[0] = '\0';
            return _sendBuffer;
        }
    }
    else
    {
        _sendBuffer[0] = '\0';
        return _sendBuffer;
    }
}

void Server::Polling()
{
    while(true)
    {
        /* liberate cpu */
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        _sendString = std::string(receiveMessage());
        if(_sendString != "")
        {
            std::cout << _sendString << std::endl;
            _receivedMsgs->send(std::move(_sendString));
        }

    }
}

void Server::terminateConnection()
{

}